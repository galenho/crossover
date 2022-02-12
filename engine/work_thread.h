/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include "thread_base.h"
#include "mutex.h"
#include "task.h"
#include "safe_queue.h"
#include "timer.h"
#include "event.h"

class WorkThread : public ThreadBase
{
public:
	WorkThread();
	virtual ~WorkThread();

	virtual void Shutdown();

public:
	void set_init_handler(init_handler_type init_handler);

	void PushTask(Task* task);

	uint32 add_timer(uint32 interval, HandleInfo handler);
	void remove_timer(uint32 index);
	void stop_all_timer();

	void WakeUp();

protected:
	virtual bool Run();

	uint32 MakeGeneralTimerID();
	
private:
	init_handler_type init_handler_;
	SafeQueue<Task*> task_list_;
	
	Event event_;

	hash_map<uint32, IntervalTimer*> timer_list_;
	vector<IntervalTimer*> del_timer_list_;

	uint32 cur_time_;
	uint32 auto_timer_idx_;
};

#endif //_WORK_THREAD_H_