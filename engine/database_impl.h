/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _DATABASE_IMPL_H_
#define _DATABASE_IMPL_H_

#include "database_defines.h"
#include "mutex.h"
#include "connection.h"

class Database_impl
{
public:
	Database_impl();
	virtual ~Database_impl();

	bool Initialize(const string& hostname, 
					uint16 port, 
					const string& username, 
					const string& password,
					const string& database, 
					const string& charset_name,
					uint32 conn_count);

	void Shutdown();

	uint32 get_next_connect();

	uint32 get_free_connect();
	void add_connect_ref(uint32 conn_idx);
	void remove_connect_ref(uint32 conn_idx);

	Statement* CreateStatement(uint32 conn_idx);

	void AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag);
	void InsertFristQuery(Statement* stmt, HandleInfo handler);
	
	void Ping();

protected:
	vector<Connection*> connect_lists;
	int32 conn_count_;

	// memcache的引用计数锁
	Mutex memcache_ref_mutex_;

	// 下一次可用的连接/线程
	int32 next_conn_idx_;
};

#endif //_DATABASE_IMPL_H_
