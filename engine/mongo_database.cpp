#include "mongo_database.h"
#include "mongo_database_impl.h"

MongoDatabase::MongoDatabase()
{
	imp_ = new MongoDatabase_impl();
}

MongoDatabase::~MongoDatabase()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}

bool MongoDatabase::Initialize(const string uri, const string db_name, uint32 conn_count)
{
	return imp_->Initialize(uri, db_name, conn_count);
}

void MongoDatabase::Ping()
{
	imp_->Ping();
}

void MongoDatabase::Shutdown()
{
	return imp_->Shutdown();
}

void MongoDatabase::PushTask(MongoTask* task, uint32 conn_idx)
{
	imp_->PushTask(task, conn_idx);
}

uint32 MongoDatabase::get_next_connect()
{
	return imp_->get_next_connect();
}

uint32 MongoDatabase::get_free_connect()
{
	return imp_->get_free_connect();
}

void MongoDatabase::add_connect_ref( uint32 conn_idx )
{
	imp_->add_connect_ref(conn_idx);
}

void MongoDatabase::remove_connect_ref( uint32 conn_idx )
{
	imp_->remove_connect_ref(conn_idx);
}