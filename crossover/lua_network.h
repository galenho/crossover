/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef LUA_NETWORK_H
#define LUA_NETWORK_H

#include "tcp_server.h"
#include "tcp_client.h"
#include "http_accessor.h"

//-------------------------------------------------------------------------------------
//					TCPServer接口
//-------------------------------------------------------------------------------------
int lua_tcpserver_new(lua_State* L);
int lua_tcpserver_start(lua_State* L);
int lua_tcpserver_close(lua_State* L);
int lua_tcpserver_send(lua_State* L);
int lua_tcpserver_sendmsg(lua_State* L);
int lua_tcpserver_disconnect(lua_State* L);
int lua_tcpserver_getipaddress(lua_State* L);
int lua_tcpserver_destroy(lua_State* L);

static const struct luaL_Reg lua_tcpserver_methods[] = {
	{ "start", lua_tcpserver_start },
	{ "close", lua_tcpserver_close },
	{ "send", lua_tcpserver_send },
	{ "send_msg", lua_tcpserver_sendmsg },
	{ "disconnect", lua_tcpserver_disconnect },
	{ "get_ip_address", lua_tcpserver_getipaddress },
	{ "__gc",  lua_tcpserver_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_tcpserver_functions[] = {
	{ "new", lua_tcpserver_new },
	{ NULL, NULL }
};

typedef struct
{
	TCPServer* server;
} tcpserver_t;

//-------------------------------------------------------------------------------------
//					TCPClient接口
//-------------------------------------------------------------------------------------
int lua_tcpclient_new(lua_State* L);
int lua_tcpclient_connectex(lua_State* L);
int lua_tcpclient_send(lua_State* L);
int lua_tcpclient_sendmsg(lua_State* L);
int lua_tcpclient_disconnect(lua_State* L);
int lua_tcpclient_getipaddress(lua_State* L);
int lua_tcpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_tcpclient_methods[] = {
	{ "connect", lua_tcpclient_connectex },
	{ "send", lua_tcpclient_send },
	{ "send_msg", lua_tcpclient_sendmsg },
	{ "disconnect", lua_tcpclient_disconnect },
	{ "get_ip_address", lua_tcpclient_getipaddress },
	{ "__gc",  lua_tcpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_tcpclient_functions[] = {
	{ "new", lua_tcpclient_new },
	{ NULL, NULL }
};

typedef struct {
	TCPClient* client;
} tcpclient_t;

//-------------------------------------------------------------------------------------
//					HttpServer接口
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
//					HttpClient接口 (使用curl)
//-------------------------------------------------------------------------------------
int lua_httpclient_new(lua_State* L);
int lua_httpclient_init(lua_State* L);
int lua_httpclient_close(lua_State* L);
int lua_httpclient_settimeout(lua_State* L);
int lua_httpclient_get(lua_State* L);
int lua_httpclient_post(lua_State* L);
int lua_httpclient_destroy(lua_State* L);

static const struct luaL_Reg lua_httpclient_methods[] = {
	{ "init", lua_httpclient_init },
	{ "close", lua_httpclient_close },
	{ "set_timeout", lua_httpclient_settimeout },
	{ "get", lua_httpclient_get },
	{ "post", lua_httpclient_post },
	{ "__gc",  lua_httpclient_destroy },
	{ NULL,NULL },
};

static const struct luaL_Reg lua_httpclient_functions[] = {
	{ "new", lua_httpclient_new },
	{ NULL, NULL }
};

typedef struct {
	HTTPAccessor* client;
} httpclient_t;

#endif //LUA_NETWORK_H