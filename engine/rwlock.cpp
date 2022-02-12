#include "common.h"
#include "rwlock.h"   

#ifdef WIN32
RwLock::RwLock()
{
	InitializeSRWLock(&lock_);
}
RwLock::~RwLock()
{
	
}

void RwLock::ReadLock()
{
	AcquireSRWLockShared(&lock_);
}

void RwLock::ReadUnLock()
{
	ReleaseSRWLockShared(&lock_);
}

void RwLock::WriteLock()
{
	AcquireSRWLockExclusive(&lock_);
}

void RwLock::WriteUnLock()
{
	ReleaseSRWLockExclusive(&lock_);
}


#else

RwLock::RwLock()
{
	pthread_rwlock_init(&lock_, NULL);
}
RwLock::~RwLock()
{
	pthread_rwlock_destroy(&lock_);
}

void RwLock::ReadLock()
{
	pthread_rwlock_rdlock(&lock_);
}

void RwLock::ReadUnLock()
{
	pthread_rwlock_unlock(&lock_);
}

void RwLock::WriteLock()
{
	pthread_rwlock_wrlock(&lock_);
}

void RwLock::WriteUnLock()
{
	pthread_rwlock_unlock(&lock_);
}

#endif
