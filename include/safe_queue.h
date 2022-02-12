/*
* 3D MMORPG Server
* Copyright (C) 2009-2014 RedLight Team
* author: 
*/

#ifndef _SAFE_QUEUE_H_
#define _SAFE_QUEUE_H_

#include <deque>
#include "mutex.h"

template <class T>
class SafeQueue
{
public:
	~SafeQueue()
	{
	}

	inline void push_back(const T& element)
	{
		mutex.Lock();
		queue.push_back(element);
		mutex.UnLock();
	}

	inline void push_front(const T& element)
	{
		mutex.Lock();
		queue.push_front(element);
		mutex.UnLock();
	}

	inline bool pop(T& element)
	{
		mutex.Lock();
		if (queue.size() > 0)
		{
			element = queue.at(0);
			queue.pop_front();
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline bool front(T& element)
	{
		mutex.Lock();
		if (queue.size() > 0)
		{
			element = queue.at(0);
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline bool pop_front(T& element)
	{
		mutex.Lock();
		if (queue.size() > 0)
		{
			queue.pop_front();
			mutex.UnLock();
			return true;
		}
		else
		{
			mutex.UnLock();
			return false;
		}
	}

	inline size_t size()
	{
		mutex.Lock();
		size_t c = queue.size();
		mutex.UnLock();
		return c;
	}

	inline bool empty()
	{
		// return true only if sequence is empty
		mutex.Lock();
		bool isEmpty = queue.empty();
		mutex.UnLock();
		return isEmpty;
	}

protected:
	std::deque<T> queue;
	Mutex mutex;
};

#endif //_SAFE_QUEUE_H_
