#include "lua_mysql.h"
#include "database.h"
#include "lua_fix.h"

int lua_mysql_new(lua_State* L)
{
	database_t* t = (database_t*)lua_newuserdata(L, sizeof(*t));
	t->db = new Database();

	luaL_getmetatable(L, "mysql");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_mysql_initialize(lua_State* L)
{
	check_param(L, 8, "usnssssn");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	const char* hostname = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);
	const char* username = lua_tostring(L, 4);
	const char* password = lua_tostring(L, 5);
	const char* database = lua_tostring(L, 6);
	const char* charset_name = lua_tostring(L, 7);
	uint32 conn_count = (uint32)lua_tointeger(L, 8);

	bool is_success = t->db->Initialize(hostname, port, username, password, database, charset_name, conn_count);
	lua_pushinteger(L, is_success);

	return 1;
}

int lua_mysql_shutdown(lua_State* L)
{
	check_param(L, 1, "u");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	t->db->Shutdown();

	return 0;
}

int lua_mysql_get_next_connect(lua_State* L)
{
	check_param(L, 1, "u");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	uint32 conn_idx = t->db->get_next_connect();
	lua_pushinteger(L, conn_idx);

	return 1;
}

int lua_mysql_get_free_connect(lua_State* L)
{
	check_param(L, 1, "u");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	uint32 conn_idx = t->db->get_free_connect();
	lua_pushinteger(L, conn_idx);

	return 1;
}

int lua_mysql_add_connect_ref(lua_State* L)
{
	check_param(L, 2, "un");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->db->add_connect_ref(conn_idx);

	return 0;
}

int lua_mysql_remove_connect_ref(lua_State* L)
{
	check_param(L, 2, "un");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->db->remove_connect_ref(conn_idx);

	return 0;
}

int lua_mysql_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	if (t->db)
	{
		delete t->db;
		t->db = NULL;
	}

	return 0;
}

int lua_mysql_add_query(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 3)
	{
		check_param(L, 3, "usf");
	}
	else if (count == 4)
	{
		check_param(L, 4, "usft");
	}
	else if (count == 5)
	{
		check_param(L, 5, "usftn");
	}
	else if (count == 6)
	{
		check_param(L, 6, "usftnt");
	}
	else
	{
		check_param(L, 7, "usftntn");
	}

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	const char* sql = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 3);

	uint32 conn_idx = INVALID_INDEX;
	DBTransactionFlag flag = DB_TRANSACTION_NONE;

	if (count >= 5)
	{
		conn_idx = (uint32)lua_tointeger(L, 5);
	}
	
	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}
	
	if (count == 7)
	{
		flag = (DBTransactionFlag)lua_tointeger(L, 7);
	}
	
	Statement* stmt = t->db->CreateStatement(conn_idx);
	stmt->Prepare(sql);

	int remove_count = count - 4;

	for (int i=0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	if (count >= 4)
	{
		int len = lua_rawlen(L, -1); // Param的长度(连续下标的数组, 从1开始)
		for (int i = 1; i <= len; i++)
		{
			lua_pushinteger(L, i);
			lua_gettable(L, -2);
			//stack_dump(L);

			int type = lua_type(L, -1);
			if (type == LUA_TNUMBER)
			{
				stmt->SetInt64(i - 1, (int64)lua_tointeger(L, -1));
			}
			else if (type == LUA_TSTRING)
			{
				stmt->SetString(i - 1, lua_tostring(L, -1));
			}
			else
			{
				assert(false);
			}

			lua_remove(L, -1);
			//stack_dump(L);
		}
	}
	
	t->db->AddQuery(stmt, handler, flag);

	return 0;
}

int lua_mysql_insert_first_query(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 3)
	{
		check_param(L, 3, "ufs");
	}
	else if (count == 4)
	{
		check_param(L, 4, "ufst");
	}
	else if (count == 5)
	{
		check_param(L, 5, "ufstn");
	}
	else if (count == 6)
	{
		check_param(L, 6, "ufstnt");
	}

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 2);

	const char* sql = lua_tostring(L, 3);
	uint32 conn_idx = INVALID_INDEX;

	if (count >= 5)
	{
		conn_idx = (uint32)lua_tointeger(L, 5);
	}

	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	Statement* stmt = t->db->CreateStatement(conn_idx);
	stmt->Prepare(sql);

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	if (count >= 4)
	{
		int len = lua_rawlen(L, -1); // Param的长度(连续下标的数组, 从1开始)
		for (int i = 1; i <= len; i++)
		{
			lua_pushinteger(L, i);
			lua_gettable(L, -2);
			//stack_dump(L);

			int type = lua_type(L, -1);
			if (type == LUA_TNUMBER)
			{
				stmt->SetInt64(i - 1, (int64)lua_tointeger(L, -1));
			}
			else if (type == LUA_TSTRING)
			{
				stmt->SetString(i - 1, lua_tostring(L, -1));
			}
			else
			{
				assert(false);
			}

			lua_remove(L, -1);
			//stack_dump(L);
		}
	}

	t->db->InsertFristQuery(stmt, handler);

	return 0;
}

int lua_mysql_ping(lua_State* L)
{
	check_param(L, 1, "u");

	database_t* t = (database_t*)luaL_checkudata(L, 1, "mysql");
	t->db->Ping();

	return 0;
}