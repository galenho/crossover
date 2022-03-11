package.path = package.path..";../?.lua"
package.path = package.path..";../common/?.lua"
package.path = package.path..";../common/hotfix/?.lua"
package.path = package.path..";../common/lua-protobuf/?.lua"
package.path = package.path..";../common/lua-xml/?.lua"
package.path = package.path..";../config/?.lua"
package.path = package.path..";../proto/?.lua"

local util = require "util"

server = udpserver.new()

fun_connect = function(conn_idx, is_success)
	print("onconnect" .. conn_idx)
end

fun_close = function(conn_idx)
	print("fun_close----" ..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	server:send(conn_idx, data, len)
end

server:start("127.0.0.1", 12345, fun_connect, fun_close, fun_recv, 8192, 8192)
