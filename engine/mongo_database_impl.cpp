#include "mongo_database_impl.h"
#include "guard.h"

MongoDatabase_impl::MongoDatabase_impl()
{
	conn_count_ = 0;
	next_conn_idx_ = 0;
}

MongoDatabase_impl::~MongoDatabase_impl()
{
	for (uint32 n=0; n < connect_lists.size(); n++)
	{
		MongoConnection *conn = connect_lists[n];
		delete conn;
		conn = NULL;
	}

	connect_lists.clear();
}

bool MongoDatabase_impl::Initialize(const string& uri, const string& db_name, uint32 conn_count)
{
	conn_count_ = conn_count;

	for (int32 i = 0; i < conn_count_; ++i)
	{
		MongoConnection *conn = new MongoConnection();
		string name = "MongoConnection " + to_string(i);
		conn->set_name(name);
		conn->set_conn_idx(i);
		if (!conn->Init(uri, db_name))
		{
			return false;
		}

		conn->Activate();
		connect_lists.push_back(conn);
	}

	return true;
}

uint32 MongoDatabase_impl::get_next_connect()
{
	int32 cur_conn_idx = next_conn_idx_;
	next_conn_idx_ = (cur_conn_idx + 1) % connect_lists.size();
	return cur_conn_idx;
	
	return 0;
}

uint32 MongoDatabase_impl::get_free_connect()
{
	ASSERT(connect_lists.size() > 0);

	Guard guard(memcache_ref_mutex_);
	int32 min_count = connect_lists[0]->memcache_ref_count_;
	uint32 conn_idx = 0;
	for (uint32 i = 1; i < connect_lists.size(); ++i)
	{
		if (connect_lists[i]->memcache_ref_count_ < min_count)
		{
			min_count = connect_lists[i]->memcache_ref_count_;
			conn_idx = i;
		}
	}

	return conn_idx;
}

void MongoDatabase_impl::add_connect_ref( uint32 conn_idx )
{
	ASSERT(conn_idx < connect_lists.size());

	Guard guard(memcache_ref_mutex_);
	connect_lists[conn_idx]->memcache_ref_count_++;
}

void MongoDatabase_impl::remove_connect_ref( uint32 conn_idx )
{
	ASSERT(conn_idx < connect_lists.size());

	Guard guard(memcache_ref_mutex_);
	ASSERT(connect_lists[conn_idx]->memcache_ref_count_ > 0);
	connect_lists[conn_idx]->memcache_ref_count_--;
}

void MongoDatabase_impl::Shutdown()
{
	for (uint32 n=0; n < connect_lists.size(); n++)
	{
		MongoConnection* conn = connect_lists[n];
		conn->Shutdown();
	}
}

void MongoDatabase_impl::PushTask(MongoTask* task, uint32 conn_idx)
{
	MongoConnection* conn = NULL;
	if (conn_idx == INVALID_INDEX) //任意一条线程
	{
		conn = connect_lists[get_next_connect()];
	}
	else
	{
		if (conn_idx >= connect_lists.size())
		{
			return;
		}

		conn = connect_lists[conn_idx];
	}

	conn->PushTask(task);
}

void MongoDatabase_impl::Ping()
{
	for (uint32 n = 0; n < connect_lists.size(); n++)
	{
		MongoConnection* conn = connect_lists[n];
		conn->Ping();
	}
}
