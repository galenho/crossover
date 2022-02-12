#include "mongo_connection.h"
#include "resultset.h"
#include "statement.h"
#include "scheduler.h"
#include "task.h"
#include "lua_fix.h"

MongoConnection::MongoConnection()
{
	conn_idx_ = 0;

	memcache_ref_count_ = 0;

	client_ = NULL;
	database_ = NULL;
}

MongoConnection::~MongoConnection()
{
	if (database_)
	{
		mongoc_database_destroy(database_);
	}

	if (client_)
	{
		mongoc_client_destroy(client_);
	}

	mongoc_cleanup();
}

bool MongoConnection::Init(const string& uri, const string& db_name)
{
	uri_ = uri;
	db_name_ = db_name;
	
	return ReConnect();
}

void MongoConnection::set_conn_idx(uint32 conn_idx)
{
	conn_idx_ = conn_idx;
}

bool MongoConnection::ReConnect()
{
	mongoc_init();

	client_ = mongoc_client_new(uri_.c_str());
	if (!client_)
	{
		mongoc_cleanup();
		return false;
	}

	database_ = mongoc_client_get_database(client_, db_name_.c_str());
	if (!database_)
	{
		mongoc_client_destroy(client_);
		mongoc_cleanup();
		return false;
	}

	return true;
}

uint32 MongoConnection::get_conn_idx()
{
	return conn_idx_;
}

void MongoConnection::PushTask(MongoTask* task)
{
	query_list_.push_back(task);
	WakeUp();
}

void MongoConnection::WakeUp()
{
	event_.Notify();
}

void MongoConnection::Ping()
{
	CommandSimpleMongoTask* query = new CommandSimpleMongoTask();
	query->command_bson = BCON_NEW("ping", BCON_INT32(1));
	query->task_type = MONGO_TASK_TYPE_COMMAND_SIMPLE;
	PushTask(query);
}

bool MongoConnection::Run()
{
	// 线程开始运行
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	MongoTask* query = NULL;

	while (is_running_)
	{
		while (query_list_.pop(query))
		{
			switch (query->task_type)
			{
			case MONGO_TASK_TYPE_COMMAND_SIMPLE:
				ProcessCommandSimple((CommandSimpleMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_CREATE_INDEX:
				ProcessCreateIndex((CreateIndexMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_DROP_INDEX:
				ProcessDropIndex((DropIndexMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_DROP:
				ProcessDrop((DropMongoTask*)query);
				break;

			case MONGO_TASK_TYPE_FIND_ONE:
				ProcessFindOne((FindOneMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_FIND:
				ProcessFind((FindMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_UPDATE_ONE:
				ProcessUpdateOne((UpdateOneMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_UPDATE_MANY:
				ProcessUpdateMany((UpdateManyMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_INSERT_ONE:
				ProcessInsertOne((InsertOneMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_INSERT_MANY:
				ProcessInsertMany((InsertManyMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_DELETE_ONE:
				ProcessDeleteOne((DeleteOneMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_DELETE_MANY:
				ProcessDeleteMany((DeleteManyMongoTask*)query);
				break;
			case MONGO_TASK_TYPE_FIND_AND_MODIFY:
				ProcessFindAndModify((FindAndModifyMongoTask*)query);
				break;
			default:
				break;
			}
			
			delete query;
			query = NULL;
		}

		event_.Wait();
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return true;
}

void MongoConnection::ProcessCommandSimple(CommandSimpleMongoTask* query)
{
	bson_t reply = BSON_INITIALIZER;

	bson_error_t error;

	bool ret = mongoc_client_command_simple(client_, db_name_.c_str(), query->command_bson, NULL, &reply, &error);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	if (ret)
	{
		bson_t* t = bson_copy(&reply);
		rs->doc_list.push_back(t);
	}

	task->Init(query->handler, rs, true);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessCreateIndex(CreateIndexMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	const mongoc_index_opt_t* opt = NULL;
	bool ret = mongoc_collection_create_index(collection, query->key_bson, opt, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessDropIndex(DropIndexMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	const mongoc_index_opt_t* opt = NULL;
	bool ret = mongoc_collection_drop_index(collection, query->index_name.c_str(), &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessDrop(DropMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bool ret = mongoc_collection_drop(collection, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessFindOne(FindOneMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	mongoc_cursor_t* cursor = mongoc_collection_find(collection,
		(mongoc_query_flags_t)query->query_flags,
		query->skip,
		query->limit,
		query->batch_size,
		query->query_bson,
		query->fields_bson,
		NULL);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = true;

	const bson_t* doc = NULL;
	while (mongoc_cursor_next(cursor, &doc))
	{
		bson_t* t = bson_copy(doc); //此处必须copy, doc会自动释放
		rs->doc_list.push_back(t);  //只找一条记录
		break;
	}

	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(collection);

	task->Init(query->handler, rs, true);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessFind(FindMongoTask* query)
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第2步
	//---------------------------------------------
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	mongoc_cursor_t* cursor = mongoc_collection_find(collection,
													 (mongoc_query_flags_t)query->query_flags,
													 query->skip,
													 query->limit,
													 query->batch_size,
													 query->query_bson,
													 query->fields_bson,
													 NULL);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = true;

	const bson_t* doc = NULL;
	while (mongoc_cursor_next(cursor, &doc))
	{
		bson_t* t = bson_copy(doc); //此处必须copy, doc会自动释放
		rs->doc_list.push_back(t);
	}

	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(collection);

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessUpdateOne(UpdateOneMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_update = mongoc_collection_create_bulk_operation(collection, false, NULL);
	mongoc_bulk_operation_update_one(bulk_update, query->filter_bson, query->update_bson, false);
	bool ret = mongoc_bulk_operation_execute(bulk_update, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessUpdateMany(UpdateManyMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_insert = mongoc_collection_create_bulk_operation(collection, false, NULL);
	mongoc_bulk_operation_update(bulk_insert, query->filter_bson, query->update_bson, false);
	bool ret = mongoc_bulk_operation_execute(bulk_insert, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessInsertOne(InsertOneMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_insert = mongoc_collection_create_bulk_operation(collection, true, NULL);
	mongoc_bulk_operation_insert(bulk_insert, query->doc_bson);
	bool ret = mongoc_bulk_operation_execute(bulk_insert, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessInsertMany(InsertManyMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_insert = mongoc_collection_create_bulk_operation(collection, true, NULL);
	for (uint32 n = 0; n < query->doc_bson_list.size(); n++)
	{
		mongoc_bulk_operation_insert(bulk_insert, query->doc_bson_list[n]);
	}
	bool ret = mongoc_bulk_operation_execute(bulk_insert, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessDeleteOne(DeleteOneMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_insert = mongoc_collection_create_bulk_operation(collection, true, NULL);
	mongoc_bulk_operation_remove_one(bulk_insert, query->doc_bson);
	bool ret = mongoc_bulk_operation_execute(bulk_insert, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessDeleteMany(DeleteManyMongoTask* query)
{
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply = BSON_INITIALIZER;
	mongoc_bulk_operation_t* bulk_insert = mongoc_collection_create_bulk_operation(collection, true, NULL);
	for (uint32 n = 0; n < query->doc_bson_list.size(); n++)
	{
		mongoc_bulk_operation_remove(bulk_insert, query->doc_bson_list[n]);
	}
	bool ret = mongoc_bulk_operation_execute(bulk_insert, &reply, &error);

	mongoc_collection_destroy(collection);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	task->Init(query->handler, rs);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::ProcessFindAndModify(FindAndModifyMongoTask* query)
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第2步
	//---------------------------------------------
	mongoc_collection_t* collection = mongoc_database_get_collection(database_, query->table_name.c_str());
	assert(collection);

	bson_error_t error;
	bson_t reply;

	bool ret = mongoc_collection_find_and_modify(collection,
		query->query_bson,
		NULL,
		query->update_bson,
		NULL,
		false,
		true,
		true,
		&reply,
		&error);

	MongoDBTask* task = new MongoDBTask();
	MongoResultSet* rs = new MongoResultSet();
	rs->is_success = ret;

	if (ret)
	{
		bson_iter_t iter;
		bson_iter_init(&iter, &reply);
		if (bson_iter_find(&iter, "value")) //找出"value"节点
		{
			const uint8_t* buf = NULL;
			uint32_t len = 0;
			bson_iter_document(&iter, &len, &buf);
			bson_t rec;
			bson_init_static(&rec, buf, len);

			bson_t* t = bson_copy(&rec); //此处必须copy, reply会自动释放
			rs->doc_list.push_back(t);

			bson_destroy(&rec);
		}
	}
	else
	{

	}

	mongoc_collection_destroy(collection);

	task->Init(query->handler, rs, true);
	Scheduler::get_instance()->PushTask(task);
}

void MongoConnection::Shutdown()
{
	is_running_ = false;

	WakeUp();
}
