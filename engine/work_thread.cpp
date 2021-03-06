#include "common.h"
#include "work_thread.h"
#include "scheduler.h"
#include "network.h"

WorkThread::WorkThread()
{
	auto_timer_idx_ = 1;

	init_handler_ = NULL;
}

WorkThread::~WorkThread()
{
	del_timer_list_.clear();
	//-----------------------------------------------------------------------------------------
	hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.begin();
	for (; it != timer_map_.end(); ++it)
	{
		IntervalTimer* timer = it->second;
		delete timer;
		timer = NULL;
	}
	timer_map_.clear();
}

void WorkThread::set_init_handler(init_handler_type init_handler)
{
	init_handler_ = init_handler;
}

bool WorkThread::Run()
{
	// 线程开始运行
	set_name("work thread");
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	// 初始化函数
	if (init_handler_)
	{
		init_handler_();
	}

	int32 wait_time = 100;				//每帧100ms
	uint32 last_time = getMSTime();		//上一帧的时间点

	while (is_running_)
	{
		uint32 cur_time = getMSTime();

		if ((int32)(cur_time - last_time) >= wait_time)
		{
			last_time = cur_time;
		}
		else
		{ 
			int32 time = (int32)(last_time + wait_time - cur_time);
			if (time <= 0)
			{
				last_time = cur_time;
			}
			else
			{
				event_.Wait(time);
				last_time = getMSTime();
			}
		}
		
		//------------------------------------------------------------------------
		// (1) 处理定时器
		//------------------------------------------------------------------------
		for (uint32 n = 0; n < del_timer_list_.size(); n++)
		{
			IntervalTimer* timer = del_timer_list_[n];
			hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.find(timer->get_index());
			if (it != timer_map_.end())
			{
				timer_map_.erase(it);
			}
			delete timer;
			timer = NULL;
		}
		del_timer_list_.clear();

		hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.begin();
		for (; it != timer_map_.end(); ++it)
		{
			IntervalTimer* timer = it->second;
			if (timer->IsDelete())
			{
				continue;
			}

			bool ret = timer->Update(cur_time);
			if (ret)
			{
				TimerTask::process(timer->handler_, timer->index_);
			}
		}
		
		//------------------------------------------------------------------------
		// (2) 处理消息队列
		//------------------------------------------------------------------------
		Task* task = NULL;
		while (task_list_.pop(task))
		{
			task->process();

			delete task;
			task = NULL;
		}

		//------------------------------------------------------------------------
		// (3) 处理Socket
		//------------------------------------------------------------------------
		SocketMgr::get_instance()->Update(cur_time);
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return false;
}

void WorkThread::Shutdown()
{
	is_running_ = false;
	WakeUp();
}

void WorkThread::WakeUp()
{
	event_.Notify();
}

void WorkThread::PushTask(Task* task)
{
	task_list_.push_back(task);
	WakeUp();
}

uint32 WorkThread::MakeGeneralTimerID()
{
	uint32 timer_idx = INVALID_INDEX;
	while (true)
	{
		hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.find(auto_timer_idx_);
		if (it != timer_map_.end())
		{
			++timer_idx;
			if (timer_idx == INVALID_INDEX)
			{
				timer_idx = 1; //从头开始
			}
		}
		else
		{
			break;
		}
	}

	//-----------------------------------------------------------------
	timer_idx = auto_timer_idx_;
	++auto_timer_idx_;
	if (auto_timer_idx_ == INVALID_INDEX)
	{
		auto_timer_idx_ = 1;
	}

	return timer_idx;
}

uint32 WorkThread::add_timer(uint32 interval, HandleInfo handler)
{
	uint32 timer_idx = MakeGeneralTimerID();
	IntervalTimer* timer = new IntervalTimer(timer_idx, getMSTime(), interval, handler);
	timer_map_.insert(make_pair(timer_idx, timer));

	WakeUp();

	return timer_idx;
}

void WorkThread::remove_timer(uint32 index)
{
	hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.find(index);
	if (it != timer_map_.end())
	{
		IntervalTimer* timer = it->second;
		if (!timer->IsDelete())
		{
			timer->Delete();
			del_timer_list_.push_back(timer);
		}
	}
}

void WorkThread::stop_all_timer()
{
	hash_map<uint32, IntervalTimer*>::iterator it = timer_map_.begin();
	for (; it != timer_map_.end(); ++it)
	{
		IntervalTimer* timer = it->second;
		if (!timer->IsDelete())
		{
			timer->Delete();
			del_timer_list_.push_back(timer);
		}
	}
}