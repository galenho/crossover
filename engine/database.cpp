#include "database.h"
#include "database_impl.h"

Database::Database()
{
	imp_ = new Database_impl();
}

Database::~Database()
{
	if (imp_)
	{
		delete imp_;
		imp_ = NULL;
	}
}


bool Database::Initialize( const string hostname, uint16 port, 
						   const string username, const string password, 
						   const string database, const string charset_name,
						   uint32 conn_count )
{
	return imp_->Initialize(hostname, port, username, password, database, charset_name, conn_count);
}

void Database::AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag /*= DB_TRANSACTION_NONE*/)
{
	imp_->AddQuery(stmt, handler, flag);
}

void Database::InsertFristQuery(Statement* stmt, HandleInfo handler)
{
	imp_->InsertFristQuery(stmt, handler);
}

void Database::Ping()
{
	imp_->Ping();
}

void Database::Shutdown()
{
	return imp_->Shutdown();
}

uint32 Database::get_next_connect()
{
	return imp_->get_next_connect();
}

uint32 Database::get_free_connect()
{
	return imp_->get_free_connect();
}

void Database::add_connect_ref( uint32 conn_idx )
{
	imp_->add_connect_ref(conn_idx);
}

void Database::remove_connect_ref( uint32 conn_idx )
{
	imp_->remove_connect_ref(conn_idx);
}

Statement* Database::CreateStatement(uint32 conn_idx)
{
	return imp_->CreateStatement(conn_idx);
}