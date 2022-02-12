/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef _RESULTSET_MYSQL_H_
#define _RESULTSET_MYSQL_H_

#include "common.h"
#include "database_defines.h"

class ResultSetMysql : public ResultSet
{
public:
	ResultSetMysql();
	~ResultSetMysql();

	bool Init(MYSQL* conn, MYSQL_STMT* stmt);

	bool NextRow();

	int8   GetInt8(int32 index);
	int16  GetInt16(int32 index);
	int32  GetInt32(int32 index);
	int64  GetInt64(int32 index);

	uint8  GetUInt8(int32 index);
	uint16 GetUInt16(int32 index);
	uint32 GetUInt32(int32 index);
	uint64 GetUInt64(int32 index);

	float  GetFloat(int32 index);
	std::string GetString(int32 index);

	bool BuildBinResult();
	bool BuildLuaResult(lua_State* L);

public:
	int32  GetColumnCount();
	std::string GetColumnName(int32 index);
	int32  GetColumnType(int32 index);
	bool   GetColumnIsSign(int32 index);
	uint32 GetColumnMaxLength(int32 index);

private:
	void AllocateResultBuffer(MYSQL_BIND* bind, BindMemInfo* mem_info, MYSQL_FIELD* field);
	
private:
	MYSQL* conn_;
	MYSQL_STMT* stmt_;

	int32 result_bind_count_;
	MYSQL_BIND* result_bind_;
	BindMemInfo* result_bind_mem_infos_;

	uint32 row_count_;
	uint32 row_size_;
	char*  data_;
};

#endif //_RESULTSET_MYSQL_H_