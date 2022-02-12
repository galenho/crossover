/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"
#include "guard.h"
#include "scheduler.h"

#ifdef CONFIG_USE_EPOLL
Socket::Socket( SOCKET wakeup_fd, SocketIOThread* work_thread )
{
	is_tcp_client_ = false;
	fd_ = wakeup_fd;
	work_thread_ = work_thread;
}
#endif

Socket::Socket(SOCKET fd,
			   uint32 conn_idx,
			   const HandleInfo onconnected_handler,
			   const HandleInfo onclose_handler,
			   const HandleInfo onrecv_handler,
			   uint32 sendbuffersize, 
			   uint32 recvbuffersize,
			   bool is_parse_package) 
{
	if (fd == 0) //说明是TcpClient连接
	{
		is_tcp_client_ = true;
	}
	else //说明是TcpServer连接
	{
		is_tcp_client_ = false;
	}

	is_parse_package_ = is_parse_package;

	fd_ = fd;
	conn_idx_ = conn_idx;

	onconnected_handler_ = onconnected_handler;
	onclose_handler_ = onclose_handler;
	onrecv_handler_ = onrecv_handler;

	// Allocate Buffers
	readBuffer.Allocate(recvbuffersize);
	recvbuffersize_ = recvbuffersize;

	writeBuffer.Allocate(sendbuffersize);

	write_lock_ = 0;

	work_thread_ = SocketMgr::get_instance()->get_free_work_thread();
	SocketMgr::get_instance()->add_socket_ref(work_thread_);

	// Check for needed fd allocation.
	if (fd_ == 0)
	{
		fd_ = SocketOps::CreateTCPFileDescriptor();
	}

	status_ = socket_status_closed;

	//PRINTF_INFO("new fd = %d, conn_idx = %d", fd_, conn_idx_);
}

Socket::~Socket()
{
	//PRINTF_INFO("delete fd = %d, conn_idx = %d", fd_, conn_idx_);
	if (work_thread_)
	{
		SocketMgr::get_instance()->remove_socket_ref(work_thread_);
		work_thread_ = NULL;
	}
}

void Socket::Update( uint32 cur_time )
{
	
}

string Socket::GetRemoteIP()
{
	char* ip = (char*)inet_ntoa(m_client.sin_addr);
	if (ip != NULL)
		return string(ip);
	else
		return string("noip");
}

bool Socket::Connect(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	SocketOps::Blocking(fd_);
	if (connect(fd_, (const sockaddr *)&m_client, sizeof(m_client)) == -1)
	{
		return false;
	}
	
	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
#ifdef CONFIG_USE_IOCP
	if (CreateIoCompletionPort((HANDLE)fd_, work_thread_->GetCompletionPort(), (ULONG_PTR)this, 0) == 0)
	{
		return false;
	}
#endif

	return true;
}

#ifdef CONFIG_USE_IOCP
typedef bool (PASCAL *connect_ex_fn)(SOCKET,
									 const sockaddr*, int, void*, DWORD, DWORD*, OVERLAPPED*);

connect_ex_fn get_connect_ex(SOCKET fd)
{
	GUID guid = { 0x25a207b9, 0xddf3, 0x4660,
	{ 0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e } };

	void *ptr = NULL;
	DWORD bytes = 0;
	if (::WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid), &ptr, sizeof(ptr), &bytes, 0, 0) != 0)
	{
		// Set connect_ex_ to a special value to indicate that ConnectEx is
		// unavailable. That way we won't bother trying to look it up again.
		ASSERT(false);
	}

	return reinterpret_cast<connect_ex_fn>(ptr);
}

bool Socket::ConnectEx(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);
	
	SocketOps::Nonblocking(fd_);

	// set common parameters on the file descriptor
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
	if (CreateIoCompletionPort((HANDLE)fd_, work_thread_->GetCompletionPort(), (ULONG_PTR)this, 0) == 0)
	{
		return false;
	}
	//-------------------------------------------------------------
	sockaddr_in a;
	memset(&a, 0, sizeof(a));
	a.sin_family = AF_INET;
	int ret = ::bind(fd_, (const sockaddr*)&a, sizeof(a));
	DWORD last_error = ::WSAGetLastError();
	//-------------------------------------------------------------
	if (ret != 0) 
	{
		OnConnect(false);
		PRINTF_ERROR("ConnectEx bind fail");
		return false;
	}
	else //绑定成功
	{
		connect_event_.SetEvent(SOCKET_IO_EVENT_CONNECT_COMPLETE);
		connect_ex_fn connect_ex = get_connect_ex(fd_);
		bool result = connect_ex(fd_, (const sockaddr*)&m_client, sizeof(m_client), 0, 0, 0, &connect_event_.overlap_);
		if (!result)
		{
			last_error = ::WSAGetLastError();
			if ( last_error != ERROR_SUCCESS && last_error != ERROR_IO_PENDING )  // 调用失败
			{
				OnConnect(false);
				return false;
			}
		}
	}

	return true;
}
#else
bool Socket::ConnectEx(const char* address, uint16 port)
{
	struct hostent* ci = gethostbyname(address);
	if (ci == 0)
	{
		return false;
	}

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);
	
	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	int ret = connect(fd_, (const sockaddr *)&m_client, sizeof(m_client));
	if (ret == -1) //直接连接成功是不可能的， 因为它是异步connect
	{
		return true;
	}

	return false;
}

#endif

void Socket::Accept(sockaddr_in* address)
{
	memcpy(&m_client, address, sizeof(*address));

	// set common parameters on the file descriptor
	SocketOps::Nonblocking(fd_);
	SocketOps::DisableBuffering(fd_);

	// IOCP stuff
#ifdef CONFIG_USE_IOCP
	CreateIoCompletionPort((HANDLE)fd_, work_thread_->GetCompletionPort(), (ULONG_PTR)this, 0);
#endif
}

void Socket::OnConnect(bool is_success)
{
	TcpConnectTask* task = new TcpConnectTask();
	task->Init(onconnected_handler_, onclose_handler_, onrecv_handler_, conn_idx_, is_success, is_tcp_client_);
	Scheduler::get_instance()->PushTask(task);
}

void Socket::OnRead()
{
	// 解包 = uint32 + content_len，并移除
	uint32 packet_len = GetReadBuffer().GetSize();
	uint32 cursor = 0;
	char* buffer_start = (char*)(GetReadBuffer().GetBufferStart());

	//---------------------------------------------------------------------------------------------
	// 不需要解包
	if (!is_parse_package_)
	{
		TcpReadTask* task = new TcpReadTask();
		task->Init(onrecv_handler_, conn_idx_, buffer_start, packet_len);
		Scheduler::get_instance()->PushTask(task);

		GetReadBuffer().Remove(packet_len);
		return;
	}

	//---------------------------------------------------------------------------------------------
	uint32 len = 0;
	while (packet_len > cursor + 4) //包体不能为0，所以要 >
	{
		// (1) 解析包头
		len = *((uint32*)(buffer_start + cursor));

		// 判断一下包头的内容长度是否符合限定范围, 到时候再改
		if (len > 0 && len <= recvbuffersize_ - 4)
		{

		}
		else
		{
			PRINTF_ERROR("read packet is error! fd = %d, conn_idx = %d", fd_, conn_idx_);
			SocketMgr::get_instance()->Disconnect(conn_idx_);

			break;
		}

		// (2) 解析包体
		if (packet_len >= cursor + 4 + len) //解包成功
		{
			cursor += 4;

			TcpReadTask* task = new TcpReadTask();
			task->Init(onrecv_handler_, conn_idx_, buffer_start + cursor, len);
			Scheduler::get_instance()->PushTask(task);
			
			cursor += len;
		}
		else
		{
			break;
		}
	}

	if (cursor)
	{
		GetReadBuffer().Remove(cursor);
	}
}

void Socket::OnDisconnect()
{
	TcpCloseTask* task = new TcpCloseTask();
	task->Init(onconnected_handler_, onrecv_handler_, onclose_handler_, conn_idx_, is_tcp_client_);
	Scheduler::get_instance()->PushTask(task);
}

void Socket::Disconnect()
{
#ifdef CONFIG_USE_IOCP
	close_event_.SetEvent(SOCKET_IO_EVENT_CLOSE);
	PostQueuedCompletionStatus(work_thread_->GetCompletionPort(), 0, (ULONG_PTR)this, &close_event_.overlap_);
#else
	SocketEvent event;
	event.s = this;
	event.customized_events = SOCKET_IO_EVENT_CLOSE;
	work_thread_->event_queue_.push_back(event);
	work_thread_->WakeUp();
#endif
}

bool Socket::Send(const void* Bytes, uint32 Size)
{
	write_mutex_.Lock();

	bool ret = writeBuffer.Write(Bytes, Size);
	write_mutex_.UnLock();

#ifdef CONFIG_USE_IOCP
	if (ret)
	{
		delay_send_event_.SetEvent(SOCKET_IO_EVENT_DELAY_SEND);
		PostQueuedCompletionStatus(work_thread_->GetCompletionPort(), 0, (ULONG_PTR)this, &delay_send_event_.overlap_);
	}
#else
	if (ret)
	{
		SocketEvent event;
		event.s = this;
		event.customized_events = SOCKET_IO_EVENT_DELAY_SEND;
		work_thread_->event_queue_.push_back(event);
		work_thread_->WakeUp();
	}
#endif

	return ret;
}

bool Socket::SendMsg(const void* Bytes, uint32 Size)
{
	write_mutex_.Lock();

	// 拼包
	bool ret = writeBuffer.WriteMsg(&Size, 4, Bytes, Size);
	write_mutex_.UnLock();

#ifdef CONFIG_USE_IOCP
	if (ret)
	{
		delay_send_event_.SetEvent(SOCKET_IO_EVENT_DELAY_SEND);
		PostQueuedCompletionStatus(work_thread_->GetCompletionPort(), 0, (ULONG_PTR)this, &delay_send_event_.overlap_);
	}
#else
	if (ret)
	{
		SocketEvent event;
		event.s = this;
		event.customized_events = SOCKET_IO_EVENT_DELAY_SEND;
		work_thread_->event_queue_.push_back(event);
		work_thread_->WakeUp();
	}
#endif

	return ret;
}