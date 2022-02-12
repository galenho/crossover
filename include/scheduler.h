/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _SHEDULER_H_
#define _SHEDULER_H_

#include "common.h"
#include "singleton.h"
#include "clog.h"
#include "task.h"

//--------------------------------------------------------------------------
// 线程调度器(用于同一时刻只能由一条活动线程来处理消息)
// 单例, 负责卡住IO线程, Accept线程, 它自己就是Update线程, 负责时钟函数触发
//--------------------------------------------------------------------------
class Scheduler_impl;

typedef function<void (uint32)> timer_handler_type;
typedef function<void (void)> post_handler_type;

class Scheduler : public Singleton<Scheduler>
{
public:
	Scheduler();
	~Scheduler();

	void set_log(CLog* log);

	uint32 add_timer(uint32 interval, HandleInfo handler);
	void remove_timer(uint32 index);
	void stop_all_timer();

	void set_io_thread_count(uint32 thread_count);

	bool startup(init_handler_type init_handler);
	bool shutdown();

	void CleanupNetwork();

	void PushTask(Task* task);

	void add_thread_ref(string thread_name);
	void remove_thread_ref(string thread_name);
	int32 get_thread_count();
	void PrintThreadInfos();

	CLog* get_logger();

private:
	Scheduler_impl *imp_;
	CLog *log_;
};

#define PRINTF_DEBUG(ftm, ...)	Scheduler::get_instance()->get_logger()->WriteLogFile(CLog::LOG_LEVEL_DEBUG, ftm, ##__VA_ARGS__);
#define PRINTF_INFO(ftm, ...)	Scheduler::get_instance()->get_logger()->WriteLogFile(CLog::LOG_LEVEL_INFO, ftm, ##__VA_ARGS__);
#define PRINTF_WARN(ftm, ...)	Scheduler::get_instance()->get_logger()->WriteLogFile(CLog::LOG_LEVEL_WARN, ftm, ##__VA_ARGS__);
#define PRINTF_ERROR(ftm, ...)	Scheduler::get_instance()->get_logger()->WriteLogFile(CLog::LOG_LEVEL_ERROR, ftm, ##__VA_ARGS__);

#endif //_SHEDULER_H_