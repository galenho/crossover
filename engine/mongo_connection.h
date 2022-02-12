/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _MONGO_CONNECTION_H_
#define _MONGO_CONNECTION_H_

#include "common.h"
#include "safe_queue.h"
#include "thread_base.h"
#include "mongo_task.h"
#include "event.h"

class MongoConnection : public ThreadBase
{
public:
	MongoConnection();
	virtual ~MongoConnection();

	bool Init( const string& uri, const string& db_name);
	
	void set_conn_idx(uint32 conn_idx);
	uint32 get_conn_idx();

	void PushTask(MongoTask* task);

	void ProcessCommandSimple(CommandSimpleMongoTask* query);
	void ProcessCreateIndex(CreateIndexMongoTask* query);
	void ProcessDropIndex(DropIndexMongoTask* query);
	void ProcessDrop(DropMongoTask* query);

	void ProcessFindOne(FindOneMongoTask* query);
	void ProcessFind(FindMongoTask* query);
	void ProcessUpdateOne(UpdateOneMongoTask* query);
	void ProcessUpdateMany(UpdateManyMongoTask* query);
	void ProcessInsertOne(InsertOneMongoTask* query);
	void ProcessInsertMany(InsertManyMongoTask* query);
	void ProcessDeleteOne(DeleteOneMongoTask* query);
	void ProcessDeleteMany(DeleteManyMongoTask* query);

	void ProcessFindAndModify(FindAndModifyMongoTask* query);

	void Ping();
	void WakeUp();

	virtual void Shutdown();

protected:
	virtual bool Run();

	bool ReConnect();

public:
	mongoc_client_t* client_;
	mongoc_database_t* database_;

	// memcache的引用计数
	int32 memcache_ref_count_;

private:
	uint32 conn_idx_;

	string uri_;
	string db_name_;

	SafeQueue<MongoTask*> query_list_;

	Event event_;
};

#endif //_MONGO_CONNECTION_H_
