#include "network.h"
#include "timer.h"
#include "guard.h"

#ifdef CONFIG_USE_IOCP
void HandleConnectComplete(Socket* s, uint32 len, bool is_success)
{
	PRINTF_ERROR("HandleConnectComplete fd = %d, conn_idx = %d, status = %d, len = %d", s->GetFd(), s->GetConnectIdx(), s->status_, len);

	if (is_success)
	{
		s->status_ = socket_status_connectted;

		SocketMgr::get_instance()->AddSocket(s); //加入socket列表
		bool ret = s->SetupReadEvent();
		if (ret) //投递一个读成功
		{
			s->OnConnect(true);
		}
		else
		{
			SocketOps::CloseSocket(s->GetFd()); //直接调用关闭closesocket
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			SocketMgr::get_instance()->RemoveSocket(s->GetConnectIdx());
		}
	}
	else
	{
		SocketOps::CloseSocket(s->GetFd()); //直接调用关闭closesocket
		s->status_ = socket_status_closed;
		s->OnConnect(false);
	}
}

void HandleReadComplete(Socket* s, uint32 len, bool is_success)
{
	PRINTF_ERROR("HandleReadComplete fd = %d, conn_idx = %d, status = %d, len = %d", s->GetFd(), s->GetConnectIdx(), s->status_, len);

	// 释放引用-1
	REF_RELEASE(s);

	if (s->status_ != socket_status_connectted && s->status_ != socket_status_closing)
	{
		return;
	}

	if (s->status_ == socket_status_connectted)
	{
		if (len)
		{
			s->GetReadBuffer().IncrementWritten(len);
			s->OnRead();

			bool ret = s->SetupReadEvent();
			if (!ret)
			{
				//PRINTF_ERROR("SetupReadEvent is fail, fd = %d, conn_idx = %d", s->GetFd(), s->GetConnectIdx());
				SocketMgr::get_instance()->CloseSocket(s);
			}
			else
			{
				//PRINTF_ERROR("SetupReadEvent is success, fd = %d, conn_idx = %d", s->GetFd(), s->GetConnectIdx());
			}
		}
		else
		{
			PRINTF_ERROR("HandleReadComplete SocketMgr::get_instance()->CloseSocket, fd = %d, conn_idx = %d", s->GetFd(), s->GetConnectIdx());
			if (s->GetSocketType() == SOCKET_TYPE_TCP)
			{
				SocketMgr::get_instance()->CloseSocket(s);
			}
			else //UDP
			{
				if (s->is_udp_connected_)
				{
					SocketMgr::get_instance()->CloseSocket(s);
				}
				else
				{
					closesocket(s->GetFd()); //直接调用关闭closesocket

					//PRINTF_ERROR("closesocket fd = %d", fd);

					s->status_ = socket_status_closed;
					s->OnConnect(false);
				}
			}
		}
	}
	else if (s->status_ == socket_status_closing)
	{
		// galen: 这个时候已经调用了closesocket函数了, 有可能socket的fd已经分配给新的连接了
		//        所以服务器发过来的后续包就不应该再投递接收了, 否则会造成fd冲突, OnRead读到的是空包
		SocketMgr::get_instance()->CloseSocket(s);
	}
}

void HandleWriteComplete(Socket* s, uint32 len, bool is_success)
{
	PRINTF_ERROR("HandleWriteComplete fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

	// 释放引用-1
	REF_RELEASE(s);

	if (s->status_ != socket_status_connectted)
	{
		return;
	}
	else
	{
		s->write_mutex_.Lock();		// Lock

		s->GetWriteBuffer().Remove(len);
		s->WriteCallback();

		s->write_mutex_.UnLock();
	}
}

void HandleClose(Socket* s, uint32 len, bool is_success)
{
	PRINTF_ERROR("HandleClose fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

	REF_RELEASE(s);

	if (s->status_ == socket_status_connectted)
	{
		s->status_ = socket_status_closing;
		SocketOps::CloseSocket(s->GetFd());
	}
}

void HandleDelaySend(Socket* s, uint32 len, bool is_success)
{
	PRINTF_ERROR("HandleDelaySend fd = %d, conn_idx = %d, status = %d", s->GetFd(), s->GetConnectIdx(), s->status_);

	REF_RELEASE(s);

	if (s->status_ == socket_status_connectted)
	{
		s->BurstPush();
	}
}

void HandleShutdown(Socket* s, uint32 len, bool is_success)
{

}
//-------------------------------------------------------------------------------
SocketIOThread::SocketIOThread()
{
	completion_port_ = NULL;
	socket_ref_count_ = 0;
}

SocketIOThread::~SocketIOThread()
{
	
}

bool SocketIOThread::Init()
{
	completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);
	return true;
}

void SocketIOThread::Shutdown()
{
	is_running_ = false;

	OverlappedStruct* ov = new OverlappedStruct(SOCKET_IO_THREAD_SHUTDOWN);
	PostQueuedCompletionStatus(completion_port_, 0, (ULONG_PTR)0, &ov->overlap_);
}

bool SocketIOThread::Run()
{
	// 线程开始运行
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	HANDLE cp = completion_port_;
	DWORD bytes_transferred;
	Socket* s;
	OverlappedStruct* ov;
	LPOVERLAPPED overlapped;

	int ok; //galen: 返回值需要做处理

	while (is_running_)
	{
#ifndef _WIN64
		ok = GetQueuedCompletionStatus(cp, &bytes_transferred, (LPDWORD)&s, &overlapped, INFINITE);
#else
		ret = GetQueuedCompletionStatus(cp, &bytes_transferred, (PULONG_PTR)&s, &overlapped, INFINITE);
#endif

		DWORD last_error = ::GetLastError();

		if (ok)
		{
			//(1) 如果函数从完成端口取出一个成功I/O操作的完成包，返回值为非0。
			//    函数在指向lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped的参数中存储相关信息。

			ov = CONTAINING_RECORD(overlapped, OverlappedStruct, overlap_);
		
			if (ov->event_ == SOCKET_IO_THREAD_SHUTDOWN)
			{
				delete ov;

				// 线程结束运行
				Scheduler::get_instance()->remove_thread_ref(thread_name_);
				return true;
			}

			if (s == NULL)
			{
				continue;
			}
			//---------------------------------------------------------------------------------
			REF_ADD(s);

			s->status_mutex_.Lock();

			if (ov->event_ >= SOCKET_IO_EVENT_CONNECT_COMPLETE && ov->event_ < MAX_SOCKET_IO_EVENTS)
			{
				ophandlers[ov->event_](s, bytes_transferred, true);
			}

			s->status_mutex_.UnLock();

			REF_RELEASE(s);
			
		}
		else
		{
			ov = CONTAINING_RECORD(overlapped, OverlappedStruct, overlap_);

			if(ov == NULL) // 第(2)种情况
			{
				//(2) 如果 *lpOverlapped为空并且函数没有从完成端口取出完成包，返回值则为0。
				//    函数则不会在lpNumberOfBytes and lpCompletionKey所指向的参数中存储信息。
				//	  调用GetLastError可以得到一个扩展错误信息。如果函数由于等待超时而未能出列完成包，GetLastError返回WAIT_TIMEOUT. 

				continue;
			}
			else 
			{
				if (s == NULL)
				{
					continue;
				}

				//---------------------------------------------------------------------------------
				if (bytes_transferred == 0) // 第(4)种情况
				{
					//(4) 如果关联到一个完成端口的一个socket句柄被关闭了，则GetQueuedCompletionStatus返回ERROR_SUCCESS,并且lpNumberOfBytes等于0
					REF_ADD(s);

					s->status_mutex_.Lock();

					ophandlers[ov->event_](s, bytes_transferred, false);

					s->status_mutex_.UnLock();

					REF_RELEASE(s);
				}
				else // 第(3)种情况
				{
					//(3) 如果 *lpOverlapped不为空并且函数从完成端口出列一个失败I/O操作的完成包，返回值为0。
					//    函数在指向lpNumberOfBytesTransferred, lpCompletionKey, and lpOverlapped的参数指针中存储相关信息。
					//    调用GetLastError可以得到扩展错误信息 
					if (overlapped)
					{
						ASSERT(false);
					}
				}
			}
		}
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return true;
}

//-------------------------------------------------------------------------------
initialiseSingleton(SocketMgr);
SocketMgr::SocketMgr()
{
	auto_conn_idx_ = 1;
	thread_count_ = 0;
}

SocketMgr::~SocketMgr()
{
	
}

uint32 SocketMgr::MakeGeneralConnID()
{
	Guard guard(conn_idx_mutex_);
	uint32 conn_idx = INVALID_INDEX;

	socket_lock.ReadLock();
	while(true)
	{
		hash_map<uint32, Socket*>::iterator it = socket_map_.find(auto_conn_idx_);
		if (it != socket_map_.end())
		{
			++auto_conn_idx_;
			if (auto_conn_idx_ == INVALID_INDEX)
			{
				auto_conn_idx_ = 1; //从头开始
			}
		}
		else
		{
			break;
		}
	}
	socket_lock.ReadUnLock();
	
	conn_idx = auto_conn_idx_;
	++auto_conn_idx_;
	if (auto_conn_idx_ == INVALID_INDEX)
	{
		auto_conn_idx_ = 1;
	}

	return conn_idx;
}

bool SocketMgr::Init(uint32 thread_count)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	thread_count_ = thread_count;
	SpawnIOThreads();

	return true;
}

bool SocketMgr::Close()
{
	ShutdownThreads();
	return true;
}

void SocketMgr::Update( uint32 cur_time )
{
	socket_lock.ReadLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.begin();
	for (; it != socket_map_.end(); it++)
	{
		Socket* s = it->second;

		REF_ADD(s);
		s->Update(cur_time);
		REF_RELEASE(s);
	}
	socket_lock.ReadUnLock();
}

void SocketMgr::SpawnIOThreads()
{
	for (uint32 x = 0; x < thread_count_; ++x)
	{
		SocketIOThread *work_thread = new SocketIOThread();
		bool ret = work_thread->Init();
		ASSERT(ret);

		string name = "work_thread" + x;
		work_thread->set_name(name);
		work_thread->Activate();
		io_threads_.push_back(work_thread);
	}
}

void SocketMgr::ShutdownThreads()
{
	for (uint32 i = 0; i < thread_count_; ++i)
	{
		io_threads_[i]->Shutdown();
	}
}

uint32 SocketMgr::Connect( const string& ip, uint16 port,
						const HandleInfo onconnected_handler,
						const HandleInfo onclose_handler,
						const HandleInfo onrecv_handler,
						uint32 sendbuffersize, 
						uint32 recvbuffersize)
{
	uint32 conn_idx = MakeGeneralConnID();
	Socket *s = new Socket( SOCKET_TYPE_TCP,
							0,
							conn_idx,
							onconnected_handler, 
							onclose_handler, 
							onrecv_handler,
							sendbuffersize, 
							recvbuffersize );

	s->status_mutex_.Lock();
	s->status_ = socket_status_connecting;

	bool ret = s->Connect(ip.c_str(), port);
	if (!ret)
	{
		s->status_mutex_.UnLock();
		delete s;
		s = NULL;
		return INVALID_INDEX;
	}
	else
	{
		AddSocket(s); //加入socket列表
		s->status_ = socket_status_connectted;

		bool ret = s->SetupReadEvent();
		if (ret) //投递一个读成功
		{
			s->OnConnect(true);
			s->status_mutex_.UnLock();
			return conn_idx;
		}
		else
		{
			SocketOps::CloseSocket(s->GetFd()); //直接调用关闭closesocket
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			s->status_mutex_.UnLock();
			RemoveSocket(s->GetConnectIdx());
			return INVALID_INDEX;
		}
	}
}

uint32 SocketMgr::ConnectEx( const string& ip, uint16 port,
						  const HandleInfo onconnected_handler,
						  const HandleInfo onclose_handler,
						  const HandleInfo onrecv_handler,
						  uint32 sendbuffersize, 
						  uint32 recvbuffersize,
						  bool is_parse_package)
{
	uint32 conn_idx = MakeGeneralConnID();
	Socket *s = new Socket(SOCKET_TYPE_TCP,
		0,
		conn_idx,
		onconnected_handler, 
		onclose_handler, 
		onrecv_handler,
		sendbuffersize, 
		recvbuffersize,
		is_parse_package);

	s->status_mutex_.Lock();
	s->status_ = socket_status_connecting;
	s->status_mutex_.UnLock();

	bool ret = s->ConnectEx(ip.c_str(), port);
	
	if (!ret)
	{	
		delete s;
		s = NULL;
		return conn_idx;
	}
	else
	{
		return conn_idx;
	}
}

void SocketMgr::Accept(
	SOCKET aSocket, 
	sockaddr_in& address, 
	const HandleInfo onconnected_handler,
	const HandleInfo onclose_handler,
	const HandleInfo onrecv_handler,
	uint32 sendbuffersize, 
	uint32 recvbuffersize,
	bool is_parse_package)
{
	Socket *s = new Socket(SOCKET_TYPE_TCP,
		aSocket,
		MakeGeneralConnID(),
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize, 
		recvbuffersize,
		is_parse_package);

	s->Accept(&address);

	AddSocket(s); //加入socket列表

	s->status_mutex_.Lock();

	s->status_ = socket_status_connectted;
	bool ret = s->SetupReadEvent();
	if (ret) //投递一个读成功
	{	
		s->OnConnect(true);
	
		s->status_mutex_.UnLock();
	}
	else
	{
		SocketOps::CloseSocket(s->GetFd()); //直接调用关闭closesocket
		s->status_mutex_.UnLock();

		RemoveSocket(s->GetConnectIdx());
	}
}

bool SocketMgr::AcceptUDP(
	sockaddr_in& remote_address, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize, 
	uint16& local_port)
{
	// 分配一个临时的socket
	string ip = inet_ntoa(remote_address.sin_addr);
	uint16 port = htons(remote_address.sin_port);

	uint32 conn_idx = ConnectUDP(ip, port, local_port,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize,
		true);

	if (conn_idx != INVALID_INDEX)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint32 SocketMgr::ConnectUDP(
	const string& ip, 
	uint16 port, 
	uint16& local_port, 
	const HandleInfo onconnected_handler, 
	const HandleInfo onclose_handler, 
	const HandleInfo onrecv_handler, 
	uint32 sendbuffersize, 
	uint32 recvbuffersize, 
	bool is_server_build /*= false*/)
{
	uint32 conn_idx = MakeGeneralConnID();

	Socket* s = new Socket(SOCKET_TYPE_UDP,
		0,
		conn_idx,
		onconnected_handler,
		onclose_handler,
		onrecv_handler,
		sendbuffersize,
		recvbuffersize);

	s->status_mutex_.Lock();
	s->status_ = socket_status_connecting;

	bool ret = s->ConnectUDP(ip.c_str(), port, local_port);
	if (!ret)
	{
		s->status_mutex_.UnLock();
		delete s;
		s = NULL;
		return INVALID_INDEX;
	}
	else
	{
		AddSocket(s); //加入socket列表
		s->status_ = socket_status_connectted;

		bool ret = s->SetupReadEvent();
		if (ret) //投递一个读成功
		{
			s->status_mutex_.UnLock();

			if (is_server_build) //如果是服务器主动创建的
			{
				s->is_udp_connected_ = true;
				s->OnConnect(true);
			}

			return conn_idx;
		}
		else
		{
			SocketOps::CloseSocket(s->GetFd()); //直接调用关闭closesocket
			s->status_ = socket_status_closed;
			s->OnConnect(false);
			s->status_mutex_.UnLock();
			RemoveSocket(s->GetConnectIdx());
			return INVALID_INDEX;
		}
	}
}

//------------------------------------------------------------------------------
void SocketMgr::AddSocket(Socket* s)
{
	socket_lock.WriteLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(s->GetConnectIdx());
	if (it != socket_map_.end())
	{
		ASSERT(false);
	}
	else
	{
		if (socket_map_.insert(make_pair(s->GetConnectIdx(), s)).second)
		{
			REF_ADD(s);
			//PRINTF_DEBUG("socket count = %d", socket_map_.size());
		}
		else
		{
			ASSERT(false);
		}
	}

	socket_lock.WriteUnLock();
}

void SocketMgr::RemoveSocket(uint32 conn_idx)
{
	socket_lock.WriteLock();

	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket *s = it->second;
		REF_RELEASE(s);
	
		socket_map_.erase(it);

		//PRINTF_DEBUG("socket count = %d", socket_map_.size());
	}

	socket_lock.WriteUnLock();
}

void SocketMgr::CloseSocket(Socket* s)
{
	if (s->status_ == socket_status_closing || s->status_ == socket_status_connectted)
	{
		SocketStatus old_status = s->status_;

		s->status_ = socket_status_closed;
		s->OnDisconnect();

		SOCKET fd = s->GetFd();
		RemoveSocket(s->GetConnectIdx());

		if (old_status == socket_status_connectted)
		{
			SocketOps::CloseSocket(fd);
		}
	}
}

bool SocketMgr::Send(uint32 conn_idx, const void* content, uint32 len)
{
	Socket* s = NULL;

	socket_lock.ReadLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		s = it->second;
		REF_ADD(s);
	}
	socket_lock.ReadUnLock();
	//--------------------------------------------------------------------------
	if (s)
	{

		bool ret = s->Send(content, len);
		if (!ret)
		{
			REF_RELEASE(s);
		}
		
		return ret;
	}
	else
	{
		return false;
	}
}

bool SocketMgr::SendMsg(uint32 conn_idx, const void* content, uint32 len)
{
	PRINTF_ERROR("SendMsg, conn_idx = %d", conn_idx);

	Socket* s = NULL;

	socket_lock.ReadLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		s = it->second;
		REF_ADD(s);
	}
	socket_lock.ReadUnLock();
	//--------------------------------------------------------------------------
	if (s)
	{
		bool ret = false;

		if (s->GetSocketType() == SOCKET_TYPE_TCP)
		{
			ret = s->SendMsg(content, len);
		}
		else
		{
			if (s->is_udp_connected_)
			{
				ret = s->SendUDP(content, len); //不带有包头的消息
			}
			else
			{
				ret = s->SendMsg(content, len);
			}

		}

		if (!ret)
		{
			REF_RELEASE(s);
		}

		return ret;
	}
	else
	{
		return false;
	}
}

void SocketMgr::Disconnect(uint32 conn_idx)
{
	Socket* s = NULL;

	socket_lock.ReadLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		s = it->second;
		REF_ADD(s);
	}
	socket_lock.ReadUnLock();
	//------------------------------------------------------------------------
	if (s)
	{
		s->Disconnect();
	}
}

char* SocketMgr::GetIpAddress( uint32 conn_idx )
{
	char* ip_addr = NULL;
	socket_lock.ReadLock();
	hash_map<uint32, Socket*>::iterator it = socket_map_.find(conn_idx);
	if (it != socket_map_.end())
	{
		Socket* s = it->second;
		ip_addr = inet_ntoa(s->GetRemoteStruct().sin_addr);	
	}
	socket_lock.ReadUnLock();
	return ip_addr;
}
//------------------------------------------------------------------------------
SocketIOThread* SocketMgr::get_free_work_thread()
{
	ASSERT(io_threads_.size() > 0);

	Guard guard(socket_ref_mutex_);
	int32 min_count = io_threads_[0]->socket_ref_count_;
	SocketIOThread* work_thread = io_threads_[0];
	for (uint32 i = 1; i < io_threads_.size(); ++i)
	{
		if (io_threads_[i]->socket_ref_count_ < min_count)
		{
			min_count = io_threads_[i]->socket_ref_count_;
			work_thread = io_threads_[i];
		}
	}
	return work_thread;
}

void SocketMgr::add_socket_ref( SocketIOThread* work_thread )
{
	Guard guard(socket_ref_mutex_);
	work_thread->socket_ref_count_++;
}

void SocketMgr::remove_socket_ref( SocketIOThread* work_thread )
{
	Guard guard(socket_ref_mutex_);
	work_thread->socket_ref_count_--;
}
#endif