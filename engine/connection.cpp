#include "connection.h"
#include "resultset.h"
#include "statement.h"
#include "scheduler.h"
#include "task.h"

Connection::Connection()
{
	conn_idx_ = 0;
	conn_ = NULL;
	port_ = 0;

	flag_ = DB_TRANSACTION_NONE;
	conn_ = NULL;
	memcache_ref_count_ = 0;
}

Connection::~Connection()
{
	if (conn_)
	{
		mysql_close(conn_);
		conn_ = NULL;
	}
}

bool Connection::Init( const string& hostname,
						  uint16 port, 
						  const string& username, 
						  const string& password, 
						  const string& database,
						  const string& charset_name)
{
	hostname_ = hostname;
	username_ = username;
	password_ = password;
	database_ = database;
	port_ = port;

	charset_name_ = charset_name;

	return ReConnect();
}

void Connection::set_conn_idx(uint32 conn_idx)
{
	conn_idx_ = conn_idx;
}

bool Connection::ReConnect()
{
	MYSQL* temp = mysql_init(NULL);
	if (mysql_options(temp, MYSQL_SET_CHARSET_NAME, charset_name_.c_str()))
	{
		return false;
	}

	my_bool my_true = true;
	if (mysql_options(temp, MYSQL_OPT_RECONNECT, &my_true))
	{
		return false;
	}

	temp = mysql_real_connect(temp, hostname_.c_str(), username_.c_str(), password_.c_str(), database_.c_str(), port_, NULL, CLIENT_MULTI_STATEMENTS);
	if (temp == NULL)
	{
		PRINTF_ERROR("Could not reconnect to database because of `%s`", mysql_error(temp));
		mysql_close(temp);
		return false;
	}

	if (conn_)
	{
		mysql_close(conn_);
		conn_ = NULL;
	}

	conn_ = temp;
	//PRINTF_INFO("reconnect to database success");

	return true;
}

uint32 Connection::get_conn_idx()
{
	return conn_idx_;
}

Statement* Connection::CreateStatement()
{
	StatementMysql* stmt = new StatementMysql(conn_, conn_idx_);
	return stmt;
}

void Connection::AddQuery(Statement* stmt, HandleInfo handler, DBTransactionFlag flag)
{
	// 如果连接的服务已经停止, 就不处理了
	if (!is_running_)
	{
		return;
	}

	if (flag == DB_TRANSACTION_ROLLBACK)
	{
		//打印错误
		PRINTF_ERROR("use DB_TRANSACTION_ROLLBACK is error");
		return;
	}

	AsyncQuery* query = new AsyncQuery();
	query->flag = flag;
	query->handler = handler;
	query->stmt = stmt;
	query_list_.push_back(query);

	WakeUp();
}

void Connection::InsertFristQuery(Statement* stmt, HandleInfo handler)
{
	// 如果连接的服务已经停止, 就不处理了
	if (!is_running_)
	{
		return;
	}

	AsyncQuery* query = new AsyncQuery();
	query->flag = DB_TRANSACTION_NONE;
	query->handler = handler;
	query->stmt = stmt;
	query_list_.push_front(query);

	WakeUp();
}

bool Connection::Ping()
{
	AsyncQuery* query = new AsyncQuery();
	query->is_ping = true;
	query_list_.push_back(query);

	WakeUp();

	return true;
}

void Connection::WakeUp()
{
	event_.Notify();
}

void Connection::BeginTransaction()
{
	mysql_autocommit(conn_, false);
}

void Connection::RollbackTransaction()
{
	mysql_rollback(conn_);
	mysql_autocommit(conn_, true);
}

void Connection::CommitTransaction()
{
	mysql_commit(conn_);
	mysql_autocommit(conn_, true);
}

bool Connection::Run()
{
	// 线程开始运行
	Scheduler::get_instance()->add_thread_ref(thread_name_);

	AsyncQuery* query = NULL;

	while (is_running_)
	{
		while (query_list_.pop(query))
		{
			if (query->is_ping)
			{
				if (mysql_ping(conn_) != 0) //说明ping失败
				{
					PRINTF_ERROR("mysql_ping is error");
				}
				
				delete query;
				query = NULL;
				continue;
			}

			if (flag_ == DB_TRANSACTION_ROLLBACK)
			{
				if (query->flag == DB_TRANSACTION_COMMIT)
				{
					flag_ = DB_TRANSACTION_NONE;
				}

				delete query;
				query = NULL;
				continue;
			}

			//PRINTF_DEBUG("query->handler begin query_list_.size() = %d", query_list_.size());
			//------------------------------------------------------------
			if (query->flag == DB_TRANSACTION_BEGIN)
			{
				BeginTransaction();
				flag_ = DB_TRANSACTION_BEGIN;
			}

			//------------------------------------------------------------
			ResultSet* rs = query->stmt->Execute();
			if (rs->is_success)
			{
				// 必须提交完事务再返回
				if (query->flag == DB_TRANSACTION_COMMIT)
				{
					CommitTransaction();
					flag_ = DB_TRANSACTION_NONE;
				}

				DBTask* task = new DBTask();
				task->Init(query->handler, rs);
				Scheduler::get_instance()->PushTask(task);
			}
			else
			{
				// 必须回滚完事务再返回
				if (flag_ == DB_TRANSACTION_BEGIN)
				{
					RollbackTransaction();

					if (query->flag == DB_TRANSACTION_COMMIT) //这一个语句是Commit语句, 也就是说最后一个事务语句
					{
						flag_ = DB_TRANSACTION_NONE;
					}
					else //说明后续的事务语句还没有执行完成
					{
						flag_ = DB_TRANSACTION_ROLLBACK;
					}
				}

				DBTask* task = new DBTask();
				task->Init(query->handler, rs);
				Scheduler::get_instance()->PushTask(task);
			}

			delete query;
			query = NULL;
			
			//PRINTF_DEBUG("query->handler end query_list_.size() = %d", query_list_.size());
		}

		event_.Wait();
	}

	if (conn_)
	{
		mysql_close(conn_);
		conn_ = NULL;
	}

	// 线程结束运行
	Scheduler::get_instance()->remove_thread_ref(thread_name_);

	return true;
}

void Connection::Shutdown()
{
	is_running_ = false;

	WakeUp();
}
