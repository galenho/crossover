server = tcpserver.new()

fun_connect = function(conn_idx, is_success)
	--print(conn_idx)
end

fun_close = function(conn_idx)
	print(conn_idx)
end

fun_recv = function(conn_idx, data, len)
	server:send_msg(conn_idx, data, len)
end

server:start("127.0.0.1", 30061, fun_connect, fun_close, fun_recv, 8192, 8192, true)
