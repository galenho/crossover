#include "timer.h"
#include "lua_fix.h"

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
	if (handler_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handler_.fun_id);
	}

	if (handler_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handler_.param_id);

	}
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
