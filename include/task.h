/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _TASK_H_
#define _TASK_H_

#include "mutex.h"
#include "database_defines.h"
#include "mongo_task.h"

class TimerTask
{
public:
	static void process(HandleInfo handle, uint32 index);
	static void DeleteTimer(HandleInfo handle, uint32 index);
};

//--------------------------------------------------------------

class Task
{
public:
	Task();
	virtual ~Task();

public:
	virtual void process() = 0;
};

class InputTask : public Task
{
public:
	InputTask();
	virtual ~InputTask();

	void Init(HandleInfo handle, string& cmd);

	virtual void process();

public:
	HandleInfo handle_;
	string cmd_;
};
//--------------------------------------------------------------

class DBTask : public Task
{
public:
	DBTask();
	virtual ~DBTask();

	void Init(HandleInfo handle, ResultSet* rs);

	virtual void process();

public:
	HandleInfo handle_;
	ResultSet* rs_;
};
//--------------------------------------------------------------

class MongoDBTask : public Task
{
public:
	MongoDBTask();
	virtual ~MongoDBTask();

	void Init(HandleInfo handle, MongoResultSet* rs, bool is_one_table = false);

	virtual void process();

public:
	HandleInfo handle_;
	MongoResultSet* rs_;
	bool is_one_table_;
};
//--------------------------------------------------------------

class SocketConnectTask : public Task
{
public:
	SocketConnectTask();
	virtual ~SocketConnectTask();

	void Init(HandleInfo connect_handle, uint32 conn_idx, bool is_success);
	virtual void process();

public:
	HandleInfo connect_handle_;
	uint32 conn_idx_;
	bool is_success_;
};

class SocketReadTask : public Task
{
public:
	SocketReadTask();
	virtual ~SocketReadTask();

	void Init(HandleInfo handle, uint32 conn_idx, char* data, uint32 data_len);
	virtual void process();

public:
	HandleInfo handle_;
	uint32 conn_idx_;
	char* data_;
	uint32 data_len_;
};

class SocketCloseTask : public Task
{
public:
	SocketCloseTask();
	virtual ~SocketCloseTask();

	void Init(HandleInfo close_handle, uint32 conn_idx);
	virtual void process();

public:
	HandleInfo close_handle_;
	uint32 conn_idx_;
};

class SocketClientDeleteTask : public Task
{
public:
	SocketClientDeleteTask();
	virtual ~SocketClientDeleteTask();

	void Init(HandleInfo connect_handle, HandleInfo recv_handle, HandleInfo close_handle);
	virtual void process();

public:
	HandleInfo connect_handle_;
	HandleInfo recv_handle_;
	HandleInfo close_handle_;
};
//--------------------------------------------------------------

class HttpTask : public Task
{
public:
	HttpTask();
	virtual ~HttpTask();

	void Init(HandleInfo handle, bool is_success, string& recv_buff, uint32 use_time);
	virtual void process();

public:
	HandleInfo handle_;
	bool is_success_;
	string recv_buff_;
	uint32 use_time_;
};

#endif //_TASK_H_