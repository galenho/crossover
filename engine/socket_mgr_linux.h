/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SOCKETMGR_LINUX_H_
#define _SOCKETMGR_LINUX_H_

#include "socket_defines.h"
#ifdef CONFIG_USE_EPOLL

#define THREAD_EVENT_SIZE 4096
#define SOCKET_HOLDER_SIZE 30000
#include "thread_base.h"
#include "rwlock.h"
#include "safe_queue.h"
#include <sys/eventfd.h>

class Socket;
class SocketIOThread;

/*
struct EveryEvent
{
	bool is_epoll_event;
	uint32 events;

	EveryEvent()
	{
		is_epoll_event = true;
		events = 0;
	}
};
*/

struct SocketEvent
{
	uint32 customized_events;
	Socket* s;

	SocketEvent()
	{
		customized_events = 0;
		s = NULL;
	}
};

class SocketIOThread : public ThreadBase
{
public:
	SocketIOThread();
	virtual ~SocketIOThread();

	bool Init();

	bool Run();
	void Shutdown();

	void WakeUp();

	inline int GetEpollFd()
	{
		return epoll_fd;
	}

private:
	void HandleDelayEvent();

public:
	SafeQueue<SocketEvent> event_queue_;

	// socket的引用计数
	int32 socket_ref_count_;

private:
	int epoll_fd;

	// epoll event struct
	struct epoll_event events[THREAD_EVENT_SIZE];

	Socket* wakeup_s_;
};

class SocketMgr : public Singleton<SocketMgr>
{
public:
	friend class SocketIOThread;

	SocketMgr();

	~SocketMgr();

	uint32 MakeGeneralConnID();

	bool Init(uint32 thread_count);
	bool Close();
	
	void Update(uint32 cur_time);

	void Accept( SOCKET aSocket, 
				 sockaddr_in& address, 
				 const HandleInfo onconnected_handler,
				 const HandleInfo onclose_handler,
				 const HandleInfo onrecv_handler,
				 uint32 sendbuffersize, 
				 uint32 recvbuffersize,
				 bool is_parse_package);

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

	bool AcceptUDP(sockaddr_in& remote_address,
		const connected_handler_type onconnected_handler,
		const close_handler_type onclose_handler,
		const recv_handler_type onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		uint16& local_port);

	uint32 ConnectUDP(const string& ip, uint16 port, uint16& local_port,
		const connected_handler_type onconnected_handler,
		const close_handler_type onclose_handler,
		const recv_handler_type onrecv_handler,
		uint32 sendbuffersize,
		uint32 recvbuffersize,
		bool is_server_build = false);

	void Disconnect(uint32 conn_idx);
	void CloseSocket(Socket* s);

	bool Send(uint32 conn_idx, const void* content, uint32 len);
	bool SendMsg(uint32 conn_idx, const void* content, uint32 len);

	void SpawnIOThreads();
	void ShutdownThreads();

	void AddSocket(Socket* s);
	void RemoveSocket(uint32 conn_idx);

	char* GetIpAddress(uint32 conn_idx);

	SocketIOThread* get_free_work_thread();
	void add_socket_ref( SocketIOThread* work_thread );
	void remove_socket_ref( SocketIOThread* work_thread );

public:
	hash_map<uint32, Socket*> socket_map_;
	RwLock socket_lock;
	
private:
	uint32 thread_count_;
	std::vector<SocketIOThread* > io_threads_;

	Mutex  conn_idx_mutex_;
	uint32 auto_conn_idx_;  //用conn_idx来做为socket_map_的Key, 因为端口事件检测使用的是指针ptr形式的

	// Socket的引用计数锁
	Mutex socket_ref_mutex_;
};

#endif

#endif //_SOCKETMGR_LINUX_H_