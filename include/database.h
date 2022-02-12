/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "database_defines.h"

class Database_impl;
class Database
{
public:
	Database();
	~Database();

	bool Initialize(const string hostname, 
					uint16 port, 
					const string username, 
					const string password,
					const string database,
					const string charset_name,
					uint32 conn_count);

	void Shutdown();

	// 得到下一个连接, 一般是未指定执行连接线程的sql语句用到
	uint32 get_next_connect();

	// 一般是给memcache分配, 因为memcache有new和delete
	uint32 get_free_connect();
	// 增加某连接线程的引用计数
	void add_connect_ref(uint32 conn_idx);
	// 移除某连接线程的引用计数
	void remove_connect_ref(uint32 conn_idx);

	Statement* CreateStatement(uint32 conn_idx);

	// sql执行接口(队列最后)
	void AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag = DB_TRANSACTION_NONE);

	// sql执行接口(队列最前)
	void InsertFristQuery(Statement* stmt, HandleInfo handler);

	void Ping();

private:
	Database_impl *imp_;
};

#endif //_DATABASE_H_