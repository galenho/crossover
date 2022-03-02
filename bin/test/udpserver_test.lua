server = udpserver.new()

fun_connect = function(conn_idx, is_success)
	print("onconnect"..conn_idx)
end

fun_close = function(conn_idx)
	print("onclose"..conn_idx)
end

fun_recv = function(conn_idx, data, len)
	server:send(conn_idx, data, len)
end

server:start("127.0.0.1", 12345, fun_connect, fun_close, fun_recv, 8192, 8192)
