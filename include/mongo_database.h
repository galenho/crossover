/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _MONGO_DATABASE_H_
#define _MONGO_DATABASE_H_

#include "common.h"
#include "mongo_task.h"

class MongoDatabase_impl;
class MongoDatabase
{
public:
	MongoDatabase();
	~MongoDatabase();

	bool Initialize(const string uri, const string db_name, uint32 conn_count);

	void Shutdown();

	void PushTask(MongoTask* task, uint32 conn_idx);

	// 得到下一个连接, 一般是未指定执行连接线程的sql语句用到
	uint32 get_next_connect();

	// 一般是给memcache分配, 因为memcache有new和delete
	uint32 get_free_connect();
	// 增加某连接线程的引用计数
	void add_connect_ref(uint32 conn_idx);
	// 移除某连接线程的引用计数
	void remove_connect_ref(uint32 conn_idx);

	void Ping();

private:
	MongoDatabase_impl *imp_;
};

#endif //_MONGO_DATABASE_H_