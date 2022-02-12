#include "thread_base.h"

#if defined _WIN32
#include <time.h>
#endif

typedef void* (*tc_thread_func)(void*);

void _Run_( void *pArguments )
{
	ThreadBase* p =(ThreadBase*)pArguments;
	if(!p)
		return;

#if defined _WIN32
	srand((unsigned int)time(NULL) + GetCurrentThreadId());
#endif

	p->Run();
}

ThreadBase::ThreadBase()
{
	is_running_ = false;
}

void ThreadBase::Activate()
{
	is_running_ = true;

#if defined _WIN32
	handle = CreateThread( NULL,0, (LPTHREAD_START_ROUTINE)_Run_,this,0,0 ) ;
#else
	new_thread = pthread_create(&id,NULL,(tc_thread_func)_Run_,this);
#endif
}

bool ThreadBase::Run()
{
	return true;
}

void ThreadBase::Shutdown()
{
#if defined _WIN32
	CloseHandle(handle);
#else
	//pthread_cancel(new_thread);
	//pthread_join(new_thread, NULL);
#endif
}

void ThreadBase::set_name( string name )
{
	thread_name_ = name;
}