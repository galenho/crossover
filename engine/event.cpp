#include "common.h"
#include "event.h"   

Event::Event()
{
#if defined _WIN32
	event_ = ::CreateEvent(0, false, false, NULL);
#else
	sem_init(&event_, 0, 0);
	is_block_ = true;
#endif
}

Event::~Event()
{
#if defined _WIN32
	
#else
	sem_destroy(&event_);
#endif
}

int Event::Wait()
{
#if defined _WIN32
	DWORD ret = ::WaitForSingleObject(event_, -1);
	if (ret == WAIT_TIMEOUT)
	{
		assert(false);
		return 1;
	}
	else if (ret == WAIT_OBJECT_0)
	{
		return 0;
	}
	else if (ret == WAIT_ABANDONED)
	{
		return -1;
	}
	return -1;
#else
	lock_.WriteLock();
	is_block_ = true;
	lock_.WriteUnLock();

	int ret = sem_wait(&event_);
	
	lock_.WriteLock();
	is_block_ = false;
	lock_.WriteUnLock();

	return ret;
#endif
}

int Event::Wait(int mill)
{
	if (mill <= 0)
	{
		assert(false);
	}

#if defined _WIN32
	DWORD ret = WaitForSingleObject(event_, mill);
	if (ret == WAIT_TIMEOUT)
	{
		return 1;
	}
	else if (ret == WAIT_OBJECT_0)
	{
		return 0;
	}
	else if (ret == WAIT_ABANDONED)
	{
		return -1;
	}
	return -1;
#else
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return -1;
	}

	lock_.WriteLock();
	is_block_ = true;
	lock_.WriteUnLock();

	int64 mills = ts.tv_nsec;
	mills += mill * (1000 * 1000);
	ts.tv_sec += int(mills / (1000 * 1000 * 1000));
	ts.tv_nsec = int(mills % (1000 * 1000 * 1000));
	int ret = sem_timedwait(&event_, &ts);

	lock_.WriteLock();
	is_block_ = false;
	lock_.WriteUnLock();

	if (-1 == ret)
	{
		if (errno == ETIMEDOUT)
		{
			return 1;
		}
		printf("sem_timedwait error %d\n", errno);
	}
	return ret;

#endif
}

void Event::Notify()
{
#if defined _WIN32
	::SetEvent(event_);
#else
	lock_.ReadLock();
	if (is_block_)
	{
		sem_post(&event_);
	}
	lock_.ReadUnLock();
#endif
}