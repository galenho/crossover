/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "socket_defines.h"
#include "timer.h"
#include "referable.h"
#include "scheduler.h"
#include "sequence_buffer.h"
#include "ikcp.h"

#ifdef CONFIG_USE_EPOLL
#include "socket_mgr_linux.h"
#else
#include "socket_mgr_win32.h"
#endif

enum SocketStatus
{
	socket_status_closed		= 1,
	socket_status_listening		= 2,
	socket_status_connecting	= 3,
	socket_status_connectted	= 4,
	socket_status_closing		= 5
};

class Socket : public Referable
{
public:
#ifdef CONFIG_USE_EPOLL
	Socket( SOCKET wakeup_fd, SocketIOThread* work_thread );
#endif

	Socket( SocketType socket_type,
			SOCKET fd, 
			uint32 conn_idx,
			const HandleInfo onconnected_handler,
			const HandleInfo onclose_handler,
			const HandleInfo onrecv_handler,
			uint32 sendbuffersize, 
			uint32 recvbuffersize,
			bool is_parse_package = true);

	~Socket();

	virtual void AddRef();
	virtual bool Release();

	void Update(uint32 cur_time);

	// Open a connection to another machine.
	bool Connect(const char* address, uint16 port);
	bool ConnectEx(const char* address, uint16 port);

	bool ConnectUDP(const char* address, uint16 port, uint16& local_port);

	// Accept from the already-set fd.
	void Accept(sockaddr_in* address);

	// Locks sending mutex, adds bytes, unlocks mutex.
	bool Send(const void* buff, uint32 len);
	bool SendMsg(const void* buff, uint32 len);

	bool SendUDP(const void* buff, uint32 len);

	// Burst system - Pushes event to queue - do at the end of write events.
	bool BurstPush();


	/* Client Operations */

	// Get the client's ip in numerical form.
	string GetRemoteIP();
	inline uint32 GetRemotePort()
	{
		return ntohs(m_client.sin_port);
	}

	inline SOCKET GetFd()
	{
		return fd_;
	}

	inline uint32 GetConnectIdx()
	{
		return conn_idx_;
	}

	inline SocketType GetSocketType()
	{
		return socket_type_;
	}

	/* Platform-specific methods */
#ifdef CONFIG_USE_IOCP
	bool SetupReadEvent();
#endif

	bool WriteCallback();

	void Disconnect();

	void OnConnect(bool is_success);
	void OnDisconnect();
	void OnRead();

	inline sockaddr_in& GetRemoteStruct()
	{
		return m_client;
	}
	
	inline sequence_buffer& GetReadBuffer()
	{
		return readBuffer;
	}
	inline sequence_buffer& GetWriteBuffer()
	{
		return writeBuffer;
	}

	inline in_addr GetRemoteAddress()
	{
		return m_client.sin_addr;
	}

public:
	inline void IncSendLock()
	{
		write_lock_++;
	}
	inline void DecSendLock()
	{
		write_lock_--;
	}
	inline bool AcquireSendLock()
	{
		if (write_lock_)
		{
			return false;
		}
		else
		{
			IncSendLock();
			return true;
		}
	}

	/* Win32 - IOCP Specific Calls */
#ifdef CONFIG_USE_IOCP
public:
	OverlappedStruct connect_event_;
	OverlappedStruct read_event_;
	OverlappedStruct write_event_;
	OverlappedStruct close_event_;
	OverlappedStruct delay_send_event_;

#endif

	/* Linux - EPOLL Specific Calls */
#ifdef CONFIG_USE_EPOLL
public:
	// Posts a epoll event with the specifed arguments.
	bool PostEvent(uint32 events);

	
	inline bool HasSendLock()
	{
		return (write_lock_ != 0);
	}

#endif

private:
	static int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);
	void send_udp_package(const char* buf, int len);

	static int udp_input(const char* buf, int len, ikcpcb* kcp, void* user);
	void on_udp_package_recv(const char* buf, int len);

public:
	sequence_buffer readBuffer;
	sequence_buffer writeBuffer;
	Mutex write_mutex_;

	SocketStatus status_;
	Mutex status_mutex_;

	uint32 recvbuffersize_;

	bool is_udp_connected_; // UDPר�е�״̬
	ikcpcb* p_kcp_; // --own
	Mutex kcp_mutex_;

	bool is_parse_package_;
	bool is_tcp_client_;

	SocketIOThread* work_thread_;

	Mutex ref_mutex_;

protected:
	SOCKET fd_;
	uint32 conn_idx_;
	sockaddr_in m_client;

	SocketType socket_type_;

	HandleInfo onconnected_handler_;
	HandleInfo onclose_handler_;
	HandleInfo onrecv_handler_;

private:
	int32 write_lock_;
};

#endif // _SOCKET_H_
