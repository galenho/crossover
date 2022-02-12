#include "scheduler.h"
#include "scheduler_impl.h"

initialiseSingleton(Scheduler);

Scheduler::Scheduler()
{
	log_ = NULL;
	imp_ = new Scheduler_impl();
}

Scheduler::~Scheduler()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

void Scheduler::set_log(CLog* log)
{
	log_ = log;
}

void Scheduler::set_io_thread_count(uint32 thread_count)
{
	imp_->set_io_thread_count(thread_count);
}

bool Scheduler::startup(init_handler_type init_handler)
{
	return imp_->startup(init_handler);
}

bool Scheduler::shutdown()
{
	return imp_->shutdown();
}

void Scheduler::CleanupNetwork()
{
	return imp_->CleanupNetwork();
}

void Scheduler::PushTask(Task* task)
{
	return imp_->PushTask(task);
}

uint32 Scheduler::add_timer(uint32 interval, HandleInfo handler )
{
	return imp_->add_timer(interval, handler);
}

void Scheduler::remove_timer( uint32 index )
{
	imp_->remove_timer(index);
}

void Scheduler::stop_all_timer()
{
	imp_->stop_all_timer();
}

CLog* Scheduler::get_logger()
{
	return log_;
}

void Scheduler::add_thread_ref(string thread_name)
{
	imp_->add_thread_ref(thread_name);
}

void Scheduler::remove_thread_ref(string thread_name)
{
	imp_->remove_thread_ref(thread_name);
}

int32 Scheduler::get_thread_count()
{
	return imp_->get_thread_count();
}

void Scheduler::PrintThreadInfos()
{
	imp_->PrintThreadInfos();
}

