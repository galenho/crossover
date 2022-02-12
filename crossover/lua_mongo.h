/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef LUA_MONGO_H
#define LUA_MONGO_H

#include "mongo_database.h"

//-------------------------------------------------------------------------------------
//					mongo接口
//-------------------------------------------------------------------------------------
int lua_mongo_new(lua_State* L);
int lua_mongo_initialize(lua_State* L);
int lua_mongo_shutdown(lua_State* L);
int lua_mongo_get_next_connect(lua_State* L);
int lua_mongo_get_free_connect(lua_State* L);
int lua_mongo_add_connect_ref(lua_State* L);
int lua_mongo_remove_connect_ref(lua_State* L);
int lua_mongo_destroy(lua_State* L);

//---------------
// 异步接口
//---------------
int lua_mongo_command_simple(lua_State* L);
int lua_mongo_create_index(lua_State* L);
int lua_mongo_drop_index(lua_State* L);
int lua_mongo_drop(lua_State* L);

int lua_mongo_find_one(lua_State* L);
int lua_mongo_find(lua_State* L);
int lua_mongo_update_one(lua_State* L);
int lua_mongo_update_many(lua_State* L);
int lua_mongo_insert_one(lua_State* L);
int lua_mongo_insert_many(lua_State* L);
int lua_mongo_delete_one(lua_State* L);
int lua_mongo_delete_many(lua_State* L);
int lua_mongo_find_and_modify(lua_State* L);

int lua_mongo_ping(lua_State* L);

static const struct luaL_Reg lua_mongo_methods[] = {
	{ "initialize",			lua_mongo_initialize },
	{ "shutdown",			lua_mongo_shutdown },
	{ "get_next_connect",	lua_mongo_get_next_connect },
	{ "get_free_connect",	lua_mongo_get_free_connect },
	{ "add_connect_ref",	lua_mongo_add_connect_ref },
	{ "remove_connect_ref", lua_mongo_remove_connect_ref },
	{ "__gc",				lua_mongo_destroy },
	//-----------------------------------------------------------------------
	{ "command_simple",		lua_mongo_command_simple },
	{ "ping",				lua_mongo_ping },
	//-----------------------------------------------------------------------
	{ "create_index",		lua_mongo_create_index },
	{ "drop_index",			lua_mongo_drop_index },
	{ "drop",				lua_mongo_drop },

	{ "find_one",			lua_mongo_find_one },
	{ "find",				lua_mongo_find },
	{ "update_one",			lua_mongo_update_one },
	{ "update_many",		lua_mongo_update_many },
	{ "insert_one",			lua_mongo_insert_one },
	{ "insert_many",		lua_mongo_insert_many },
	{ "delete_one",			lua_mongo_delete_one },
	{ "delete_many",		lua_mongo_delete_many },
	{ "find_and_modify",	lua_mongo_find_and_modify },

	

	{ NULL,NULL },
};

static const struct luaL_Reg lua_mongo_functions[] = {
	{ "new", lua_mongo_new },
	{ NULL, NULL }
};

typedef struct
{
	MongoDatabase* db;
} mongo_database_t;


#endif //LUA_MONGO_H
