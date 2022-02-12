/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _MONGO_DATABASE_IMPL_H_
#define _MONGO_DATABASE_IMPL_H_

#include "common.h"
#include "mutex.h"
#include "mongo_connection.h"

class MongoDatabase_impl
{
public:
	MongoDatabase_impl();
	virtual ~MongoDatabase_impl();

	bool Initialize(const string& uri, const string& db_name, uint32 conn_count);

	void Shutdown();

	void PushTask(MongoTask* task, uint32 conn_idx);

	uint32 get_next_connect();

	uint32 get_free_connect();
	void add_connect_ref(uint32 conn_idx);
	void remove_connect_ref(uint32 conn_idx);

	void Ping();

protected:
	vector<MongoConnection*> connect_lists;
	int32 conn_count_;

	// memcache的引用计数锁
	Mutex memcache_ref_mutex_;

	// 下一次可用的连接/线程
	int32 next_conn_idx_;
};

#endif //_MONGO_DATABASE_IMPL_H_
