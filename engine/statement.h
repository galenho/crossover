/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _STATEMENT_MYSQL_H_
#define _STATEMENT_MYSQL_H_

#include "common.h"
#include "database_defines.h"

class StatementMysql : public Statement
{
public:
	StatementMysql(MYSQL* conn, uint32 conn_idx);
	~StatementMysql();

	int32 GetConnIdx();

	bool Prepare(const char* sql);

	ResultSet* Execute();

	void SetFloat(int32 index, float value);

	void SetUInt8(int32 index, uint8 value);
	void SetUInt16(int32 index, uint16 value);
	void SetUInt32(int32 index, uint32 value);
	void SetUInt64(int32 index, uint64 value);

	void SetInt8(int32 index, int8 value);
	void SetInt16(int32 index, int16 value);
	void SetInt32(int32 index, int32 value);
	void SetInt64(int32 index, int64 value);

	void SetString(int32 index, const char* value);
	void SetJson(int32 index, const char* value);

	void SetBytes(int32 index, const char* value, int len);

private:
	void SetBindInfo(uint32 len, BindMemInfo& info);

private:
	uint32 conn_idx_;
	MYSQL* conn_;
	MYSQL_STMT* stmt_;

	string str_sql_;
	int32 param_bind_count_;
	MYSQL_BIND* param_bind_;
	BindMemInfo* param_bind_mem_infos_;
};

#endif //_STATEMENT_MYSQL_H_