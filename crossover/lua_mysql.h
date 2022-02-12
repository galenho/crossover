/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef LUA_MYSQL_H
#define LUA_MYSQL_H

#include "database.h"

//-------------------------------------------------------------------------------------
//					mysql接口
//-------------------------------------------------------------------------------------
int lua_mysql_new(lua_State* L);
int lua_mysql_initialize(lua_State* L);
int lua_mysql_shutdown(lua_State* L);
int lua_mysql_get_next_connect(lua_State* L);
int lua_mysql_get_free_connect(lua_State* L);
int lua_mysql_add_connect_ref(lua_State* L);
int lua_mysql_remove_connect_ref(lua_State* L);
int lua_mysql_destroy(lua_State* L);

//---------------
// 异步接口
//---------------
int lua_mysql_add_query(lua_State* L);
int lua_mysql_insert_first_query(lua_State* L);
int lua_mysql_ping(lua_State* L);

static const struct luaL_Reg lua_mysql_methods[] = {
	{ "initialize", lua_mysql_initialize },
	{ "shutdown", lua_mysql_shutdown },
	{ "get_next_connect", lua_mysql_get_next_connect },
	{ "get_free_connect", lua_mysql_get_free_connect },
	{ "add_connect_ref", lua_mysql_add_connect_ref },
	{ "remove_connect_ref", lua_mysql_remove_connect_ref },
	{ "__gc",  lua_mysql_destroy },

	{ "add_query", lua_mysql_add_query },
	{ "insert_frist_query", lua_mysql_insert_first_query },
	{ "ping", lua_mysql_ping },

	{ NULL,NULL },
};

static const struct luaL_Reg lua_mysql_functions[] = {
	{ "new", lua_mysql_new },
	{ NULL, NULL }
};

typedef struct
{
	Database* db;
} database_t;


#endif //LUA_MYSQL_H