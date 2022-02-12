/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_

#if defined _WIN32
#include <windows.h>
#include <Process.h>
#else
#include <pthread.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#endif

#include <string>
using namespace std;

class ThreadBase
{
public:
	ThreadBase();
	virtual ~ThreadBase() {}

	void Activate();
	virtual bool Run();
	virtual void Shutdown();

	void set_name(string name);

protected:
	bool is_running_;

	bool is_high_priority_;

#if defined _WIN32
	HANDLE handle;
#else
	pthread_t id;
	pthread_t new_thread;
#endif

	string thread_name_;
};

#endif //_THREAD_BASE_H_

