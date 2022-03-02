/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: galen
*/

#include "network.h"
#ifdef CONFIG_USE_IOCP

namespace SocketOps
{
	// Create file descriptor for socket i/o operations.
	SOCKET CreateTCPFileDescriptor()
	{
		// create a socket for use with overlapped i/o.
		return ::WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
	}

	// Create file descriptor for socket i/o operations.
	SOCKET CreateUDPFileDescriptor()
	{
		// create a socket for use with overlapped i/o.
		return ::WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
	}

	// Disable blocking send/recv calls.
	bool Nonblocking(SOCKET fd)
	{
		u_long arg = 1;
		return (::ioctlsocket(fd, FIONBIO, &arg) == 0);
	}

	// Disable blocking send/recv calls.
	bool Blocking(SOCKET fd)
	{
		u_long arg = 0;
		return (ioctlsocket(fd, FIONBIO, &arg) == 0);
	}

	// Disable nagle buffering algorithm
	bool DisableBuffering(SOCKET fd)
	{
		uint32 arg = 1;
		return (setsockopt(fd, 0x6, TCP_NODELAY, (const char*)&arg, sizeof(arg)) == 0);
	}

	// Enable nagle buffering algorithm
	bool EnableBuffering(SOCKET fd)
	{
		uint32 arg = 0;
		return (setsockopt(fd, 0x6, TCP_NODELAY, (const char*)&arg, sizeof(arg)) == 0);
	}

	// Set internal buffer size to socket.
	bool SetSendBufferSize(SOCKET fd, uint32 size)
	{
		return (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size)) == 0);
	}

	// Set internal buffer size to socket.
	bool SetRecvBufferSize(SOCKET fd, uint32 size)
	{
		return (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size)) == 0);
	}

	// Closes a socket fully.
	void CloseSocket(SOCKET fd)
	{
		//shutdown(fd, SD_BOTH);

		/*
			SO_LINGER
			此选项指定函数close对面向连接的协议如何操作（如TCP）。缺省close操作是立即返回，如果有数据残留在套接口缓冲区中则系统将试着将这些数据发送给对方。
			SO_LINGER选项用来改变此缺省设置。使用如下结构：
			struct linger 
			{
				int l_onoff; // 0 = off, nozero = on 
				int l_linger; // linger time 
			};
			有下列三种情况：
			l_onoff为0，则该选项关闭，l_linger的值被忽略，等于缺省情况，close立即返回； 
			l_onoff为非0，l_linger为0，则套接口关闭时TCP夭折连接，TCP将丢弃保留在套接口发送缓冲区中的任何数据并发送一个RST给对方，而不是通常的四分组终止序列，这避免了TIME_WAIT状态； 
			l_onoff 为非0，l_linger为非0，当套接口关闭时内核将拖延一段时间（由l_linger决定）。如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，直 到（a）所有数据发送完且被对方确认，之后进行正常的终止序列（描述字访问计数为0）或（b）延迟时间到。此种情况下，应用程序检查close的返回值是非常重要的，如果在数据发送完并被确认前时间到，close将返回EWOULDBLOCK错误且套接口发送缓冲区中的任何数据都丢失。close的成功返回仅告诉我们发送的数据（和FIN）已由对方TCP确认，它并不能告诉我们对方应用进程是否已读了数据。如果套接口设为非阻塞的，它将不等待close完成。 
			l_linger的单位依赖于实现，4.4BSD假设其单位是时钟滴答（百分之一秒），但Posix.1g规定单位为秒。
		*/

		int retv;
		LINGER LingerStruct;
		LingerStruct.l_onoff = 1;
		LingerStruct.l_linger = 0;

		retv = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&LingerStruct, sizeof(LingerStruct));
		if (retv == SOCKET_ERROR)
		{
			//PRINTF_ERROR("setsockopt Error:%d", WSAGetLastError());
		}

		retv = closesocket(fd);
		//PRINTF_ERROR("closesocket fd = %d", fd);

		if (retv == SOCKET_ERROR)
		{
			//PRINTF_ERROR("closesocket Error:%d", WSAGetLastError());
		}
	}

	// Sets reuseaddr
	void ReuseAddr(SOCKET fd)
	{
		uint32 option = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, 4);
	}
}

#endif
