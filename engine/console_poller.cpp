#include "singleton.h"
#include "console_poller.h"
#include "scheduler.h"
#include "task.h"

initialiseSingleton(ConsolePoller);
ConsolePoller::ConsolePoller()
{
	is_running_ = false;

	set_name("ConsolePoller thread");
}

ConsolePoller::~ConsolePoller()
{
	
}

void ConsolePoller::set_callback_handler(HandleInfo handle_console_input )
{
	handle_console_input_ = handle_console_input;
}

bool ConsolePoller::Run()
{
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);  
	DWORD mode;  
	GetConsoleMode(hStdin, &mode);  
	mode &= ~ENABLE_QUICK_EDIT_MODE;  //移除快速编辑模式
	mode &= ~ENABLE_INSERT_MODE;      //移除插入模式
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, mode);
#endif

	// 线程开始运行
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	size_t i = 0;
	size_t len;
	char cmd[300];
#ifndef WIN32
	struct pollfd input;

	input.fd = 0;
	input.events = POLLIN | POLLPRI;
	input.revents = 0;
#endif

	while (is_running_)
	{
#ifdef WIN32
		// Read in single line from "stdin"
		memset(cmd, 0, sizeof(cmd));
		if (fgets(cmd, 300, stdin) == NULL)
			continue;

		if (!is_running_)
			break;
#else
		int ret = poll(&input, 1, 1000);
		if (ret < 0)
		{
			break;
		}
		else if (ret == 0)
		{
			if (is_running_)	// timeout
				continue;
			else
				break;
		}

		ret = read(0, cmd, sizeof(cmd));
		if (ret <= 0)
		{
			break;
		}
#endif

		len = strlen(cmd);
		for (i = 0; i < len; ++i)
		{
			if (cmd[i] == '\n' || cmd[i] == '\r')
				cmd[i] = '\0';
		}

		string str_cmd = cmd;

		InputTask* task = new InputTask();
		task->Init(handle_console_input_, str_cmd);
		Scheduler::get_instance()->PushTask(task);
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return true;
}

void ConsolePoller::DestoryLocal()
{
	Singleton<ConsolePoller>::Destroy();
}

void ConsolePoller::Stop()
{
	is_running_ = false;

#ifdef WIN32
	DWORD dwTmp;
	INPUT_RECORD ir[2];
	
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 288;
	ir[0].Event.KeyEvent.uChar.AsciiChar = 13;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = 13;
	ir[0].Event.KeyEvent.wVirtualScanCode = 28;

	ir[1].EventType = KEY_EVENT;
	ir[1].Event.KeyEvent.bKeyDown = FALSE;
	ir[1].Event.KeyEvent.dwControlKeyState = 288;
	ir[1].Event.KeyEvent.uChar.AsciiChar = 13;
	ir[1].Event.KeyEvent.wRepeatCount = 1;
	ir[1].Event.KeyEvent.wVirtualKeyCode = 13;
	ir[1].Event.KeyEvent.wVirtualScanCode = 28;
	
	WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dwTmp);
#endif
}
