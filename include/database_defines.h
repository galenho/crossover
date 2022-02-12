/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _DATABASE_DEFINES_H_
#define _DATABASE_DEFINES_H_

#include "common.h"
#include <mysql/mysql.h>

enum DBTransactionFlag
{
	DB_TRANSACTION_NONE			= 0,
	DB_TRANSACTION_BEGIN		= 1,
	DB_TRANSACTION_ROLLBACK		= 2,
	DB_TRANSACTION_COMMIT		= 3
};

struct BindMemInfo
{
	char* data;
	unsigned long len;
	char  is_null;
	char* column_name;

	BindMemInfo()
	{
		data = NULL;
		len = 0;
		is_null = 0;
		column_name = NULL;
	}

	~BindMemInfo()
	{
		if (data)
		{
			delete[] data;
			data = NULL;
		}

		if (column_name)
		{
			delete[] column_name;
			column_name = NULL;
		}
	}
};

class ResultSet
{
public:
	ResultSet() { is_success = false; is_have_rs = false; }

	virtual ~ResultSet() {}

	virtual bool NextRow() = 0;

	virtual int8 GetInt8(int32 idx) = 0;
	virtual int16 GetInt16(int32 idx) = 0;
	virtual int32 GetInt32(int32 idx) = 0;
	virtual int64 GetInt64(int32 idx) = 0;

	virtual uint8 GetUInt8(int32 idx) = 0;
	virtual uint16 GetUInt16(int32 idx) = 0;
	virtual uint32 GetUInt32(int32 idx) = 0;
	virtual uint64 GetUInt64(int32 idx) = 0;

	virtual float GetFloat(int32 idx) = 0;
	virtual string GetString(int32 idx) = 0;

	virtual int32  GetColumnCount() = 0;
	virtual string GetColumnName(int32 index) = 0;
	virtual int32  GetColumnType(int32 index) = 0;
	virtual bool   GetColumnIsSign(int32 index) = 0;

	virtual bool BuildBinResult() = 0;
	virtual bool BuildLuaResult(lua_State* L) = 0;

public:
	bool is_success;
	bool is_have_rs;
};

class Statement
{
public:
	Statement() {}
	virtual ~Statement() {}

	virtual int32 GetConnIdx() = 0;

	virtual bool Prepare(const char* sql) = 0;

	virtual ResultSet* Execute() = 0;

	virtual void SetFloat(int paramIndex, float value) = 0;

	virtual void SetUInt8(int paramIndex, uint8 value) = 0;
	virtual void SetUInt16(int paramIndex, uint16 value) = 0;
	virtual void SetUInt32(int paramIndex, uint32 value) = 0;
	virtual void SetUInt64(int paramIndex, uint64 value) = 0;

	virtual void SetInt8(int paramIndex, int8 value) = 0;
	virtual void SetInt16(int paramIndex, int16 value) = 0;
	virtual void SetInt32(int paramIndex, int32 value) = 0;
	virtual void SetInt64(int paramIndex, int64 value) = 0;

	virtual void SetString(int paramIndex, const char* value) = 0;
	virtual void SetBytes(int paramIndex, const char* value, int length) = 0;
};

struct AsyncQuery
{
	DBTransactionFlag flag;
	Statement* stmt;
	HandleInfo handler;
	bool is_ping;

	AsyncQuery()
	{
		flag = DB_TRANSACTION_NONE;
		stmt = NULL;
		is_ping = false;
	}

	~AsyncQuery()
	{
		if (stmt)
		{
			delete stmt;
			stmt = NULL;
		}
	}
};

#endif //_DATABASE_DEFINES_H_