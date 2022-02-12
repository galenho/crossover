#include "statement.h"
#include "resultset.h"

int32 CountChar(const char* sql, char c)
{
	int count = 0;

	const char* p = sql;
	while (*p != '\0')
	{
		if (*p == c)
		{
			count += 1;
		}

		p++;
	}

	return count;
}

StatementMysql::StatementMysql(MYSQL* conn, uint32 conn_idx)
{
	param_bind_ = NULL;
	param_bind_count_ = 0;
	param_bind_mem_infos_ = NULL;

	conn_ = conn;
	conn_idx_ = conn_idx;

	stmt_ = NULL;
}

StatementMysql::~StatementMysql()
{
	if (stmt_)
	{
		mysql_stmt_close(stmt_);
	}

	if (param_bind_)
	{
		delete[] param_bind_;
		param_bind_ = NULL;
	}

	if (param_bind_mem_infos_)
	{
		delete[] param_bind_mem_infos_;
		param_bind_mem_infos_ = NULL;
	}
}

int32 StatementMysql::GetConnIdx()
{
	return conn_idx_;
}

bool StatementMysql::Prepare(const char* sql)
{
	param_bind_count_ = CountChar(sql, '?');

	param_bind_ = new MYSQL_BIND[param_bind_count_];
	memset(param_bind_, 0, sizeof(MYSQL_BIND) * param_bind_count_);

	param_bind_mem_infos_ = new BindMemInfo[param_bind_count_];
	memset(param_bind_mem_infos_, 0, sizeof(BindMemInfo) * param_bind_count_);

	str_sql_ = sql;
	return true;
}

void StatementMysql::SetBindInfo(uint32 len, BindMemInfo& info)
{
	info.data = new char[len];
	info.len = len;
}

void StatementMysql::SetFloat(int32 index, float value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_FLOAT;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
}

void StatementMysql::SetUInt8(int32 index, uint8 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_TINY;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = true;
}

void StatementMysql::SetUInt16(int32 index, uint16 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_SHORT;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = true;
}

void StatementMysql::SetUInt32(int32 index, uint32 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_LONG;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = true;
}

void StatementMysql::SetUInt64(int32 index, uint64 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_LONGLONG;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = true;
}

void StatementMysql::SetInt8(int32 index, int8 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_TINY;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = false;
}

void StatementMysql::SetInt16(int32 index, int16 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_SHORT;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = false;
}

void StatementMysql::SetInt32(int32 index, int32 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_LONG;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = false;
}

void StatementMysql::SetInt64(int32 index, int64 value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = sizeof(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_LONGLONG;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
	param_bind_[index].is_unsigned = false;
}

void StatementMysql::SetString(int32 index, const char* value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = strlen(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_STRING;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
}

void StatementMysql::SetJson(int32 index, const char* value)
{
	ASSERT(index < param_bind_count_);

	uint32 len = strlen(value);
	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_JSON;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
}

void StatementMysql::SetBytes(int32 index, const char* value, int len)
{
	ASSERT(index < param_bind_count_);

	BindMemInfo& stBindInfo = param_bind_mem_infos_[index];

	SetBindInfo(len, stBindInfo);
	memcpy(stBindInfo.data, &value, len);

	param_bind_[index].buffer_type = MYSQL_TYPE_VARCHAR;
	param_bind_[index].buffer = (void*)stBindInfo.data;
	param_bind_[index].buffer_length = len;
}

ResultSet* StatementMysql::Execute()
{
	ResultSetMysql* rs = new ResultSetMysql();

	stmt_ = mysql_stmt_init(conn_);
	if (!stmt_)
	{
		printf("mysql_stmt_init() failed\n");
		rs->is_success = false;
		return rs;
	}

	int ret = mysql_stmt_prepare(stmt_, str_sql_.c_str(), str_sql_.length());
	if (ret != 0)
	{
		printf("mysql_stmt_prepare() failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt_));
		rs->is_success = false;
		return rs;
	}

	if (mysql_stmt_bind_param(stmt_, param_bind_))
	{
		printf("mysql_stmt_bind_param() failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt_));
		rs->is_success = false;
		return rs;
	}

	if (mysql_stmt_execute(stmt_))
	{
		printf("mysql_stmt_execute() failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt_));
		rs->is_success = false;
		return rs;
	}

	rs->is_success = rs->Init(conn_, stmt_);
	rs->is_success = rs->BuildBinResult();

	return rs;
}

