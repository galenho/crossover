#include "timer.h"
#include "task.h"

IntervalTimer::IntervalTimer(uint32 index, uint32 cur_time, uint32 interval, HandleInfo handler)
{
	index_ = index;
	interval_ = interval;
	last_time_ = cur_time;
	handler_ = handler;
	is_delete_ = false;
}

IntervalTimer::~IntervalTimer()
{
	TimerTask::DeleteTimer(handler_, index_);
}

bool IntervalTimer::Update(uint32 cur_time)
{
	if ((int32)(cur_time - last_time_) >= (int32)interval_)
	{
		last_time_ = cur_time;

		return true;
	}
	else
	{
		return false;
	}
}
