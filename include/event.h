/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _EVENT_H
#define _EVENT_H																		

#ifdef WIN32
#include <windows.h>
#else
#include<semaphore.h>
#include "rwlock.h"
#endif

class Event
{
public:
	Event();
	~Event();
	
	// 阻塞等待 (注: 返回值 0: ok, 1: timeout, -1: error)
	int Wait();

	// 带超时的阻塞等待 (注: 返回值 0: ok, 1: timeout, -1: error)
	int Wait(int mill);

	// 消息通知
	void Notify();

private:
#if defined _WIN32
	HANDLE	event_;
#else
	sem_t	event_;
	RwLock  lock_;
	bool    is_block_;
#endif
};

#endif //_EVENT_H
