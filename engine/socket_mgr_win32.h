/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*
*
* 核心类: (1) 采用引用计数机制来防止重复删除Socket对象或野指针
*             需要加引用的地方主要有4处 ---- 读投递, 写投递, hash_map引用, 逻辑处理引用 
*		  (2) 采用状态机锁来控制Socket的状态
*		  (3) 采用读写缓冲区来实现双向异步通讯
*		  (4) 外部只能采用Disconnect函数来主动关闭Socket
*/
#ifndef _SOCKETMGR_WIN32_H_
#define _SOCKETMGR_WIN32_H_

#ifdef CONFIG_USE_IOCP
#include "thread_base.h"
#include "singleton.h"
#include "rwlock.h"

class Socket;
//----------------------------------------------------------------------------
typedef void(*OperationHandler)(Socket* s, uint32 len, bool is_success);

void HandleConnectComplete(Socket* s, uint32 len, bool is_success);
void HandleReadComplete(Socket* s, uint32 len, bool is_success);
void HandleWriteComplete(Socket* s, uint32 len, bool is_success);
void HandleClose(Socket* s, uint32 len, bool is_success);
void HandleDelaySend(Socket* s, uint32 len, bool is_success);

void HandleShutdown(Socket* s, uint32 len, bool is_success);

static OperationHandler ophandlers[MAX_SOCKET_IO_EVENTS] =
{
	&HandleConnectComplete, &HandleReadComplete, &HandleWriteComplete, &HandleClose, &HandleDelaySend, &HandleShutdown
};

//----------------------------------------------------------------------------
class SocketIOThread : public ThreadBase
{
public:
	SocketIOThread();
	virtual ~SocketIOThread();

	bool Init();

	virtual bool Run();
	void Shutdown();

	inline HANDLE GetCompletionPort()
	{
		return completion_port_;
	}

public:
	// socket的引用计数
	int32 socket_ref_count_;

private:
	HANDLE completion_port_;
};
//----------------------------------------------------------------------------
class SocketMgr : public Singleton<SocketMgr>
{
public:
	SocketMgr();
	~SocketMgr();

	bool Init(uint32 thread_count);
	bool Close();

	void Update(uint32 cur_time);

	uint32 Connect( const string& ip, uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize);

	uint32 ConnectEx( const string& ip, uint16 port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize,
		bool is_parse_package);

	bool Send(uint32 conn_idx, const void* content, uint32 len);
	bool SendMsg(uint32 conn_idx, const void* content, uint32 len);
	void Disconnect(uint32 conn_idx);

	void Accept( SOCKET aSocket, 
		sockaddr_in& address, 
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize, 
		uint32 recvbuffersize,
		bool is_parse_package);

	bool AcceptUDP(sockaddr_in& remote_address,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		uint16& local_port);

	uint32 ConnectUDP(const string& ip, uint16 port, uint16& local_port,
		const HandleInfo onconnected_handler,
		const HandleInfo onclose_handler,
		const HandleInfo onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		bool is_server_build = false);

	void CloseSocket(Socket* s);
	void ShutdownThreads();

	void RemoveSocket(uint32 conn_idx);

	uint32 MakeGeneralConnID();

	void AddSocket(Socket* s);

	char* GetIpAddress(uint32 conn_idx);
	
	SocketIOThread* get_free_work_thread();
	void add_socket_ref( SocketIOThread* work_thread );
	void remove_socket_ref( SocketIOThread* work_thread );

private:
	void SpawnIOThreads();

public:
	hash_map<uint32, Socket*> socket_map_;
	RwLock socket_lock;

private:
	vector<SocketIOThread*> io_threads_;
	uint32 thread_count_;

	Mutex  conn_idx_mutex_;
	uint32 auto_conn_idx_;  //用conn_idx来做为socket_map_的Key, 因为端口事件检测使用的是指针ptr形式的

	// Socket的引用计数锁
	Mutex socket_ref_mutex_;
};

#endif
#endif //_SOCKETMGR_WIN32_H_