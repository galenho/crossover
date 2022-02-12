/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef __CONSOLE_POLLER_HEADER__
#define __CONSOLE_POLLER_HEADER__

#include "thread_base.h"
class ConsolePoller : public ThreadBase, public Singleton<ConsolePoller>
{
public:
	ConsolePoller();
	virtual ~ConsolePoller();

	static void DestoryLocal();

	void set_callback_handler(HandleInfo handle_console_input);
	void Stop();

protected:	
	bool Run();
	
private:
	HandleInfo handle_console_input_;
};

#endif // __WS_CONSOLE_POLLER_HEADER__