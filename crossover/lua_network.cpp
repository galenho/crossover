#include "lua_network.h"
#include "lua_fix.h"

int lua_tcpserver_new(lua_State* L)
{
	tcpserver_t* t = (tcpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new TCPServer();

	luaL_getmetatable(L, "tcpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_tcpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnb");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnbf");
	}
	else if (count == 11)
	{
		check_param(L, 11, "usnfffnnbff");
	}
	else
	{
		check_param(L, 12, "usnfffnnbfff");
	}

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	bool is_parse_package = true;
	if (count > 8)
	{
		is_parse_package = (bool)lua_toboolean(L, 9);
	}

	if (count > 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count > 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 11);
	}

	if (count > 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 12);
	}

	t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize, is_parse_package);
	return 0;
}

int lua_tcpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	t->server->Close();

	return 0;
}

int lua_tcpserver_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->Send(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_tcpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_tcpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	tcpserver_t* t = (tcpserver_t*)luaL_checkudata(L, 1, "tcpserver");
	if (t->server) 
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_tcpclient_new(lua_State* L)
{
	tcpclient_t* t = (tcpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new TCPClient();

	luaL_getmetatable(L, "tcpclient");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_tcpclient_connectex(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnb");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnbt");
	}
	else if (count == 11)
	{
		check_param(L, 11, "usnfffnnbtt");
	}
	else
	{
		check_param(L, 12, "usnfffnnbttt");
	}

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	bool is_parse_package = true;
	if (count > 8)
	{
		is_parse_package = (bool)lua_toboolean(L, 9);
	}

	if (count > 9)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count > 10)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 11);
	}

	if (count > 11)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 12);
	}

	t->client->ConnectEx(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize, is_parse_package);

	return 0;
}

int lua_tcpclient_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->Send(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpclient_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_tcpclient_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->client->Disconnect(conn_idx);

	return 0;
}

int lua_tcpclient_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->client->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);
	return 1;
}

int lua_tcpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	tcpclient_t* t = (tcpclient_t*)luaL_checkudata(L, 1, "tcpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_udpserver_new(lua_State* L)
{
	udpserver_t* t = (udpserver_t*)lua_newuserdata(L, sizeof(*t));
	t->server = new UDPServer();

	luaL_getmetatable(L, "udpserver");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_udpserver_start(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnf");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnnff");
	}
	else
	{
		check_param(L, 11, "usnfffnnfff");
	}

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");

	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count > 8)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count > 9)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count > 10)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	t->server->Start(ip, port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);
	return 0;
}

int lua_udpserver_close(lua_State* L)
{
	check_param(L, 1, "u");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	t->server->Close();

	return 0;
}

int lua_udpserver_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpserver_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->server->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpserver_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->server->Disconnect(conn_idx);

	return 0;
}

int lua_udpserver_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->server->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);

	return 1;
}

int lua_udpserver_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	udpserver_t* t = (udpserver_t*)luaL_checkudata(L, 1, "udpserver");
	if (t->server)
	{
		delete t->server;
		t->server = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_udpclient_new(lua_State* L)
{
	udpclient_t* t = (udpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new UDPClient();

	luaL_getmetatable(L, "udpclient");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_udpclient_connect(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 8)
	{
		check_param(L, 8, "usnfffnn");
	}
	else if (count == 9)
	{
		check_param(L, 9, "usnfffnnt");
	}
	else if (count == 10)
	{
		check_param(L, 10, "usnfffnntt");
	}
	else
	{
		check_param(L, 11, "usnfffnnttt");
	}

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	const char* ip = lua_tostring(L, 2);
	uint16 port = (uint16)lua_tointeger(L, 3);

	HandleInfo onconnected_handler;
	int fun_onconn_id = toluafix_ref_function(L, 4);
	onconnected_handler.fun_id = fun_onconn_id;

	HandleInfo onclose_handler;
	int fun_onclose_id = toluafix_ref_function(L, 5);
	onclose_handler.fun_id = fun_onclose_id;

	HandleInfo onrecv_handler;
	int fun_onrecv_id = toluafix_ref_function(L, 6);
	onrecv_handler.fun_id = fun_onrecv_id;

	uint32 sendbuffersize = (uint32)lua_tointeger(L, 7);
	uint32 recvbuffersize = (uint32)lua_tointeger(L, 8);

	if (count > 8)
	{
		onconnected_handler.param_id = toluafix_ref_param(L, 9);
	}

	if (count > 9)
	{
		onclose_handler.param_id = toluafix_ref_param(L, 10);
	}

	if (count > 10)
	{
		onrecv_handler.param_id = toluafix_ref_param(L, 11);
	}

	uint16 local_port = 0;
	t->client->Connect(ip, port, local_port, onconnected_handler, onclose_handler, onrecv_handler, sendbuffersize, recvbuffersize);

	return 0;
}

int lua_udpclient_send(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpclient_sendmsg(lua_State* L)
{
	check_param(L, 4, "unsn");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	uint32 len = (uint32)lua_tointeger(L, 4);
	t->client->SendMsg(conn_idx, (char*)msg, len);

	return 0;
}

int lua_udpclient_disconnect(lua_State* L)
{
	check_param(L, 2, "un");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->client->Disconnect(conn_idx);

	return 0;
}

int lua_udpclient_getipaddress(lua_State* L)
{
	check_param(L, 2, "un");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	char* ip = t->client->GetIpAddress(conn_idx);
	lua_pushstring(L, ip);
	return 1;
}

int lua_udpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	udpclient_t* t = (udpclient_t*)luaL_checkudata(L, 1, "udpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}

//-------------------------------------------------------------------------------------
int lua_httpclient_new(lua_State* L)
{
	httpclient_t* t = (httpclient_t*)lua_newuserdata(L, sizeof(*t));
	t->client = new HTTPAccessor();

	luaL_getmetatable(L, "httpclient");
	lua_setmetatable(L, -2);
	return 1;
}

int lua_httpclient_init(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	bool ret = t->client->Init();
	lua_pushboolean(L, ret);

	return 1;
}

int lua_httpclient_close(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	t->client->Close();

	return 0;
}

int lua_httpclient_settimeout(lua_State* L)
{
	check_param(L, 2, "un");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	uint32 time = (uint32)lua_tointeger(L, 2);
	t->client->SetTimeout(time);

	return 0;
}

int lua_httpclient_get(lua_State* L)
{
	check_param(L, 4, "usfs");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");

	const char* url = lua_tostring(L, 2);
	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 3);
	handler.fun_id = fun_id;
	const char* custom_http_head = lua_tostring(L, 4);

	t->client->Get(url, handler, custom_http_head);

	return 0;
}

int lua_httpclient_post(lua_State* L)
{
	check_param(L, 5, "ussfs");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");

	const char* url = lua_tostring(L, 2);
	const char* data = lua_tostring(L, 3);
	
	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 4);
	handler.fun_id = fun_id;
	const char* custom_http_head = lua_tostring(L, 5);

	t->client->Post(url, data, handler, custom_http_head);

	return 0;
}

int lua_httpclient_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	httpclient_t* t = (httpclient_t*)luaL_checkudata(L, 1, "httpclient");
	if (t->client)
	{
		delete t->client;
		t->client = NULL;
	}

	return 0;
}
