#include "common.h"
#include "scheduler_impl.h"
#include "network.h"
#include "guard.h"

Scheduler_impl::Scheduler_impl()
{
	io_thread_count_ = 0;

	work_thread_ = NULL;
}

Scheduler_impl::~Scheduler_impl()
{
	if (work_thread_)
	{
		delete work_thread_;
		work_thread_ = NULL;
	}
}

void Scheduler_impl::set_io_thread_count(uint32 thread_count)
{
	io_thread_count_ = thread_count;
}

bool Scheduler_impl::startup(init_handler_type init_handler)
{
	if (io_thread_count_ > 0)
	{
		SocketMgr::get_instance()->Init(io_thread_count_);
	}
	
	// 激活工作线程
	work_thread_ = new WorkThread();
	work_thread_->set_init_handler(init_handler);
	work_thread_->Activate();

	return true;
}

bool Scheduler_impl::shutdown()
{
	if (io_thread_count_ > 0)
	{
		SocketMgr::get_instance()->Close();
	}

	work_thread_->Shutdown();

	return true;
}

void Scheduler_impl::CleanupNetwork()
{
#ifdef WIN32
	if (io_thread_count_ > 0)
	{
		WSACleanup();
	}
#endif
}

uint32 Scheduler_impl::add_timer(uint32 interval, HandleInfo handler)
{
	return work_thread_->add_timer(interval, handler);
}

void Scheduler_impl::remove_timer(uint32 index)
{
	work_thread_->remove_timer(index);
}

void Scheduler_impl::stop_all_timer()
{
	work_thread_->stop_all_timer();
}

void Scheduler_impl::PushTask(Task* task)
{
	work_thread_->PushTask(task);
}

void Scheduler_impl::add_thread_ref(string thread_name)
{
	Guard guard(thread_count_lock_);
	thread_name_map_.insert(make_pair(thread_name, thread_name));
}	

void Scheduler_impl::remove_thread_ref(string thread_name)
{
	Guard guard(thread_count_lock_); 
	thread_name_map_.erase(thread_name);
}

int32 Scheduler_impl::get_thread_count()
{
	Guard guard(thread_count_lock_);
	return thread_name_map_.size();
}

void Scheduler_impl::PrintThreadInfos()
{
	Guard guard(thread_count_lock_);
	hash_map<string, string>::iterator it = thread_name_map_.begin();
	for (; it != thread_name_map_.end(); it++)
	{
		string thread_name = it->first;
		printf("%s\n", thread_name.c_str());
	}
}