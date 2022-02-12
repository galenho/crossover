#include "common.h"
#include "mutex.h"   

#ifdef WIN32
Mutex::Mutex()
{
	InitializeCriticalSection(&cs);
}
Mutex::~Mutex()
{
	DeleteCriticalSection(&cs);
}

void Mutex::Lock()
{
	EnterCriticalSection(&cs);
}

void Mutex::UnLock()
{
	LeaveCriticalSection(&cs);
}

#else

Mutex::Mutex()
{
	pthread_mutex_init(&mutex, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex);
}

void Mutex::Lock()
{
	pthread_mutex_lock(&mutex);
}

void Mutex::UnLock()
{
	pthread_mutex_unlock(&mutex);
}

#endif
