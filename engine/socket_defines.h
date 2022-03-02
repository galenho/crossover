/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SOCKET_DEFINES_H_
#define _SOCKET_DEFINES_H_

#include "common.h"
using namespace std;

/* Implementation Selection */
#ifdef WIN32
#define CONFIG_USE_IOCP
#else

// unix defines
#define SOCKET int
#define SD_BOTH SHUT_RDWR

// epoll
#include <sys/epoll.h>
#define CONFIG_USE_EPOLL

#endif

enum SocketIOEvent
{
	SOCKET_IO_EVENT_CONNECT_COMPLETE= 0,
	SOCKET_IO_EVENT_READ_COMPLETE	= 1,
	SOCKET_IO_EVENT_WRITE_COMPLETE	= 2,
	SOCKET_IO_EVENT_CLOSE			= 3,
	SOCKET_IO_EVENT_DELAY_SEND		= 4,
	SOCKET_IO_THREAD_SHUTDOWN		= 5,
	MAX_SOCKET_IO_EVENTS
};

enum SocketType
{
	SOCKET_TYPE_TCP = 1,
	SOCKET_TYPE_UDP = 2
};

/* IOCP Defines */
#ifdef CONFIG_USE_IOCP

class OverlappedStruct
{
public:
	OVERLAPPED overlap_;
	SocketIOEvent event_;

	OverlappedStruct(SocketIOEvent ev) : event_(ev)
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
	};

	OverlappedStruct()
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
	}

	__forceinline void SetEvent(SocketIOEvent ev)
	{
		memset(&overlap_, 0, sizeof(OVERLAPPED));
		event_ = ev;
	}
};

#endif

#endif
