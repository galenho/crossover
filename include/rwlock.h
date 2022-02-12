/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/
/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author:
*/

#ifndef _RWLOCK_H
#define _RWLOCK_H

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class RwLock
{
public:
	RwLock();
	~RwLock();

	void ReadLock();
	void ReadUnLock();

	void WriteLock();
	void WriteUnLock();

private:
#ifdef WIN32
	SRWLOCK lock_;
#else
	pthread_rwlock_t lock_;
#endif
};

#endif //_RWLOCK_H
