/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _MUTEX_H
#define _MUTEX_H																		

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class Mutex
{
public:
	Mutex();
	~Mutex();

	void Lock();
	void UnLock();

protected:
#ifdef WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t mutex;
#endif
};

#endif //_MUTEX_H
