/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

#include "lua_crossover.h"
#include "lua_network.h"
#include "lua_mysql.h"
#include "lua_mongo.h"


int luaopen_lua_crossover(lua_State* L);
int luaopen_lua_tcpserver(lua_State* L);
int luaopen_lua_tcpclient(lua_State* L);
int luaopen_lua_udpserver(lua_State* L);
int luaopen_lua_udpclient(lua_State* L);
int luaopen_lua_httpclient(lua_State* L);
int luaopen_lua_mysql(lua_State* L);
int luaopen_lua_mongo(lua_State* L);

#endif //LUA_WRAPPER_H