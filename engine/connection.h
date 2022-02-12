/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "common.h"
#include "database_defines.h"
#include "safe_queue.h"
#include "thread_base.h"
#include "event.h"

class Connection : public ThreadBase
{
public:
	Connection();
	virtual ~Connection();

	bool Init( const string& hostname, 
			   uint16 port, 
		       const string& username, 
			   const string& password, 
			   const string& database,
			   const string& charset_name);

	void set_conn_idx(uint32 conn_idx);
	uint32 get_conn_idx();
	
	Statement* CreateStatement();

	void AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag);
	void InsertFristQuery(Statement* stmt, HandleInfo handler);

	bool Ping();

	void BeginTransaction();
	void RollbackTransaction();
	void CommitTransaction();

	virtual void Shutdown();

	void WakeUp();

protected:
	virtual bool Run();

	bool ReConnect();

public:
	MYSQL* conn_;
	// memcache的引用计数
	int32 memcache_ref_count_;

private:
	uint32 conn_idx_;

	DBTransactionFlag flag_;

	string hostname_;
	string username_;
	string password_;
	string database_;
	uint16 port_;

	string charset_name_;

	SafeQueue<AsyncQuery*> query_list_;

	Event event_;
};

#endif //_CONNECTION_H_
