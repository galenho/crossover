#include "lua_wrapper.h"

int luaopen_lua_crossover(lua_State* L)
{
	luaL_newlib(L, lua_common_functions);

	return 1;
}

int luaopen_lua_tcpserver(lua_State* L)
{
	luaL_newmetatable(L, "tcpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_tcpserver_methods, 0);
	luaL_newlib(L, lua_tcpserver_functions);

	return 1;
}

int luaopen_lua_tcpclient(lua_State* L)
{
	luaL_newmetatable(L, "tcpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_tcpclient_methods, 0);
	luaL_newlib(L, lua_tcpclient_functions);

	return 1;
}

int luaopen_lua_udpserver(lua_State* L)
{
	luaL_newmetatable(L, "udpserver");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_udpserver_methods, 0);
	luaL_newlib(L, lua_udpserver_functions);

	return 1;
}

int luaopen_lua_udpclient(lua_State* L)
{
	luaL_newmetatable(L, "udpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_udpclient_methods, 0);
	luaL_newlib(L, lua_udpclient_functions);

	return 1;
}

int luaopen_lua_httpclient(lua_State* L)
{
	luaL_newmetatable(L, "httpclient");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_httpclient_methods, 0);
	luaL_newlib(L, lua_httpclient_functions);

	return 1;
}

int luaopen_lua_mysql(lua_State* L)
{
	luaL_newmetatable(L, "mysql");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_mysql_methods, 0);
	luaL_newlib(L, lua_mysql_functions);

	return 1;
}

int luaopen_lua_mongo(lua_State* L)
{
	luaL_newmetatable(L, "mongo");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, lua_mongo_methods, 0);
	luaL_newlib(L, lua_mongo_functions);

	return 1;
}
