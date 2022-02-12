/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _CLOG_H
#define _CLOG_H

#include "common.h"
#include "mutex.h"
#include "thread_base.h"
#include "safe_queue.h"
#include "event.h"

#ifdef WIN32
#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#else
#define TRED 1
#define TGREEN 2
#define TYELLOW 3
#define TNORMAL 4
#define TWHITE 5
#define TBLUE 6
#endif

struct LogData
{
	int8  op_type;  // 1:日志数据操作 2:保存操作
	int32 data_len;
	char* data;

	LogData()
	{
		op_type = 1;
		data_len = 0;
		data = NULL;
	}
};

class CLog : public ThreadBase
{
public:
	enum LogLevel
	{
		LOG_LEVEL_DEBUG = 0,
		LOG_LEVEL_INFO  = 1,
		LOG_LEVEL_WARN  = 2,
		LOG_LEVEL_ERROR = 3
	};

	CLog(int32 log_level = LOG_LEVEL_INFO, int32 buff_size = 1024 * 1024, int32 write_buff_size = 1024);
	virtual ~CLog();

	void set_log_name( string file_name );
	void set_log_level(int32 log_level);
	
	void WriteLogFile( int32 log_level, const char* ftm, ...);
	void Save();

	void Start();
	void StopWaitExit();

	void WakeUp();

protected:	
	bool Run();

private:
	void Color(unsigned int color);

private:
	int32 log_level_;
	string file_name_;

	ofstream fout;

	char* buffer_;
	int32 buff_size_;  //初始化大小
	int32 write_buff_size_; //多大写入文件一次

#ifdef WIN32
	HANDLE stdout_handle, stderr_handle;
#endif

	Mutex mutex;

	SafeQueue<LogData> log_list_;

	bool is_thread_exit_; //线程是否退出

	Event event_;
};

#define LOG_DEBUG(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_DEBUG, ftm, ##__VA_ARGS__)
#define LOG_INFO(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_INFO, ftm, ##__VA_ARGS__)
#define LOG_WARN(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_WARN, ftm, ##__VA_ARGS__)
#define LOG_ERROR(logger, ftm, ...)	logger->WriteLogFile(CLog::LOG_LEVEL_ERROR, ftm, ##__VA_ARGS__)

#endif //_CLOG_H

