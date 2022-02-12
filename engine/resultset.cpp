#include "resultset.h"
#include "lua_fix.h"

ResultSetMysql::ResultSetMysql() : ResultSet()
{
	conn_ = NULL;
	stmt_ = NULL;

	result_bind_count_ = 0;
	result_bind_ = NULL;
	result_bind_mem_infos_ = NULL;

	row_count_ = 0;
	row_size_ = 0;
	data_ = NULL;
}

ResultSetMysql::~ResultSetMysql()
{
	if (result_bind_)
	{
		delete[] result_bind_;
		result_bind_ = NULL;
	}

	if (result_bind_mem_infos_)
	{
		delete[] result_bind_mem_infos_;
		result_bind_mem_infos_ = NULL;
	}

	if (data_)
	{
		delete[] data_;
		data_ = NULL;
	}
}

bool ResultSetMysql::Init(MYSQL* conn, MYSQL_STMT* stmt)
{
	conn_ = conn;
	stmt_ = stmt;

	//--------------------------------------------------------------------------------
	MYSQL_RES* res = mysql_stmt_result_metadata(stmt_); //这个结果集只用来取字段集
	if (!res) //可检测语句是否生成了结果集
	{
		is_have_rs = false;
		return true;
	}

	is_have_rs = true;

	result_bind_count_ = mysql_stmt_field_count(stmt_);

	result_bind_ = new MYSQL_BIND[result_bind_count_];
	memset(result_bind_, 0, sizeof(MYSQL_BIND) * result_bind_count_);

	result_bind_mem_infos_ = new BindMemInfo[result_bind_count_];
	memset(result_bind_mem_infos_, 0, sizeof(BindMemInfo) * result_bind_count_);

	row_size_ = 0; // 计算行的字节长度
	int index = 0;
	MYSQL_FIELD* field = NULL;
	while ((field = mysql_fetch_field(res)))
	{
		BindMemInfo* stBindInfo = &result_bind_mem_infos_[index];
		stBindInfo->column_name = new char[strlen(field->name) + 1];
		strcpy(stBindInfo->column_name, field->name);
		MYSQL_BIND* bind = &result_bind_[index];
		AllocateResultBuffer(bind, stBindInfo, field);

		if (bind->buffer_type == MYSQL_TYPE_STRING || bind->buffer_type == MYSQL_TYPE_VAR_STRING || bind->buffer_type == MYSQL_TYPE_JSON)
		{
			row_size_ += 4; //为了做二进制的序列化，字符串要有4个字节int来表示字符串的长度
		}

		row_size_ += bind->buffer_length;

		index++;
	}

	mysql_free_result(res);
	//--------------------------------------------------------------------------------

	int status = mysql_stmt_bind_result(stmt_, result_bind_);
	if (status == 0)
	{
		status = mysql_stmt_store_result(stmt_);
		if (status == 0)
		{
			row_count_ = (uint32)mysql_stmt_affected_rows(stmt_);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void ResultSetMysql::AllocateResultBuffer(MYSQL_BIND* bind, BindMemInfo* mem_info, MYSQL_FIELD* field)
{
	if (field)
	{
		bind->buffer_type = field->type;
		bind->is_unsigned = field->flags & UNSIGNED_FLAG;
		switch (bind->buffer_type)
		{
		case MYSQL_TYPE_TINY:
			bind->buffer_length = 1;
			break;
		case MYSQL_TYPE_YEAR:
		case MYSQL_TYPE_SHORT:
			bind->buffer_length = 2;
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
			bind->buffer_length = 4;
			break;
		case MYSQL_TYPE_LONGLONG:
			bind->buffer_length = 8;
			break;
		case MYSQL_TYPE_FLOAT:
			bind->buffer_length = 4;
			break;
		case MYSQL_TYPE_DOUBLE:
			bind->buffer_length = 8;
			break;
		case MYSQL_TYPE_NEWDECIMAL:
			bind->buffer_length = 67;
			break;

		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TIMESTAMP:
			bind->buffer_length = sizeof(MYSQL_TIME);
			break;
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			bind->buffer_length = field->length;
			break;
		case MYSQL_TYPE_JSON:
			bind->buffer_length = field->length;
			break;
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
			bind->buffer_length = 1;
			break;
		case MYSQL_TYPE_BIT:
			bind->buffer_length = 1;
			break;
		default:
			ASSERT(false);
			printf("allocateResultBuffer() failed: Unknown type for the result.\n");
		}

		mem_info->data = new char[bind->buffer_length];
		bind->buffer = mem_info->data; 
		bind->length = &mem_info->len;
		bind->is_null = &mem_info->is_null;
	}
	else
	{
		bind->buffer_type = MYSQL_TYPE_NULL;
		ASSERT(false);
	}
}

bool ResultSetMysql::NextRow()
{
	if (!is_have_rs)
	{
		return false;
	}
	
	//--------------------------------------------------
	bool ret = false;
	int status = mysql_stmt_fetch(stmt_);
	switch (status)
	{
	case 0:
		ret = true;
		break;
	case MYSQL_NO_DATA: // 没有数据了
		ret = false;
		break;
	case MYSQL_DATA_TRUNCATED: // 出现数据截短
		fprintf(stderr, "出现数据截短\n");
		ASSERT(false);
		ret = false;
		break;
	case 1:
	default:
		ret = false;
		printf(mysql_stmt_error(stmt_));
		break;
	}

	return ret;
}

int8 ResultSetMysql::GetInt8(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(int8*)(mem_info->data);
}

int16 ResultSetMysql::GetInt16(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);
	
	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}
	return *(int16*)(mem_info->data);
}

int32 ResultSetMysql::GetInt32(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(int32*)(mem_info->data);
}

int64 ResultSetMysql::GetInt64(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(int64*)(mem_info->data);
}

uint8 ResultSetMysql::GetUInt8(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(uint8*)(mem_info->data);
}

uint16 ResultSetMysql::GetUInt16(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(uint16*)(mem_info->data);
}

uint32 ResultSetMysql::GetUInt32(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(uint32*)(mem_info->data);
}

uint64 ResultSetMysql::GetUInt64(int32 index)
{
    ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0;
	}

	return *(uint64*)(mem_info->data);
}

float ResultSetMysql::GetFloat(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return 0.0f;
	}

	return *(float*)(mem_info->data);
}

std::string ResultSetMysql::GetString(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	if (mem_info->is_null)
	{
		return "";
	}

	std::string str;
	str.assign(mem_info->data, mem_info->len);
	return str;
}

int32 ResultSetMysql::GetColumnCount()
{
	return result_bind_count_;
}

std::string ResultSetMysql::GetColumnName(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	BindMemInfo* mem_info = &result_bind_mem_infos_[index];
	std::string str;
	str.assign(mem_info->column_name, strlen(mem_info->column_name));
	return str;
}

int32 ResultSetMysql::GetColumnType(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	MYSQL_BIND* bind = &result_bind_[index];
	return (int32)bind->buffer_type;
}

bool ResultSetMysql::GetColumnIsSign(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	MYSQL_BIND* bind = &result_bind_[index];
	return (bool)bind->is_unsigned;
}

uint32 ResultSetMysql::GetColumnMaxLength(int32 index)
{
	ASSERT(index >= 0 && index < result_bind_count_);

	MYSQL_BIND* bind = &result_bind_[index];
	return (uint32)bind->buffer_length;
}

bool ResultSetMysql::BuildBinResult()
{
	// 因为需要跨线程操作，操作mysql_stmt_fetch会造成多线程问题
	// 所以先把记录集用二进制data_存起来，后续再转为Lua Table
	if (row_count_ == 0)
	{
		return true;
	}

	data_ = new char[row_size_ * row_count_];
	int offset = 0;
	while (NextRow())
	{
		for (int32 n = 0; n < result_bind_count_; n++)
		{
			MYSQL_BIND* bind = &result_bind_[n];
			BindMemInfo* mem_info = &result_bind_mem_infos_[n];
			if (bind->buffer_type == MYSQL_TYPE_STRING || bind->buffer_type == MYSQL_TYPE_VAR_STRING || bind->buffer_type == MYSQL_TYPE_JSON)
			{
				*((uint32*)(data_ + offset)) = mem_info->len;
				offset += 4;
				memcpy(data_ + offset, mem_info->data, mem_info->len);
				offset += mem_info->len;
			}
			else
			{
				memcpy(data_ + offset, mem_info->data, mem_info->len);
				offset += mem_info->len;
			}
		}
	}

	return true;
}

bool ResultSetMysql::BuildLuaResult(lua_State* L)
{
	/*
	{
		[1] = {
				["id"] = 1,
				["name"] = "Bob",
		},
		[2] = {
				["id"] = 2,
				["name"] = "Kof",
		}
	}
	*/

	lua_newtable(L);//创建一个表格，放在栈顶

	int offset = 0;
	for (int32 row = 0; row < (int32)row_count_; row++)
	{
		lua_pushnumber(L, row + 1);

		lua_newtable(L); // table ---- 一行数据 begin

		for (int32 col = 0; col < result_bind_count_; col++)
		{
			MYSQL_BIND* bind = &result_bind_[col];
			BindMemInfo* mem_info = &result_bind_mem_infos_[col];

			lua_pushstring(L, mem_info->column_name); //字段名
			//stack_dump(g_lua_state);

			switch (bind->buffer_type)
			{
			case MYSQL_TYPE_STRING:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_JSON:
				{
					uint32 len = *((uint32*)(data_ + offset));
					offset += 4;

					lua_pushlstring(L, data_ + offset, len);
					offset += len;
				}
				break;
			case MYSQL_TYPE_TINY:
				{
					if (bind->is_unsigned)
					{
						uint8 v = *((uint8*)(data_ + offset));
						lua_pushnumber(L, v);
					}
					else
					{
						int8 v = *((int8*)(data_ + offset));
						lua_pushnumber(L, v);
					}

					offset += 1;
				}
				break;
			case MYSQL_TYPE_SHORT:
				{
					if (bind->is_unsigned)
					{
						uint16 v = *((uint16*)(data_ + offset));
						lua_pushnumber(L, v);
					}
					else
					{
						int16 v = *((int16*)(data_ + offset));
						lua_pushnumber(L, v);
					}

					offset += 2;
				}
				break;
			case MYSQL_TYPE_LONG:
				{
					if (bind->is_unsigned)
					{
						uint32 v = *((uint32*)(data_ + offset));
						lua_pushnumber(L, v);
					}
					else
					{
						int32 v = *((int32*)(data_ + offset));
						lua_pushnumber(L, v);
					}

					offset += 4;
				}
				break;
			case MYSQL_TYPE_FLOAT:
				{
					float v = *((float*)(data_ + offset));
					lua_pushnumber(L, v);

					offset += 4;
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				{
					double v = *((double*)(data_ + offset));
					lua_pushnumber(L, v);

					offset += 8;
				}
				break;
			default:
				{
					ASSERT(false);
				}
				break;
			}
			
			//stack_dump(L);
			lua_settable(L, -3);
			//stack_dump(L);
		}

		lua_settable(L, -3); // table ---- 一行数据 end
		//stack_dump(L);
	}

	//stack_dump(L);

	return true;
}