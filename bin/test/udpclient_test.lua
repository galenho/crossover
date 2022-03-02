-- lua 表数据
local data = {
   name = "ilse",
   age  = 18,
   contacts = {
      { name = "alice", phonenumber = 12312341234 },
      { name = "bob",   phonenumber = 45645674567 }
   }
}

client = udpclient.new()

fun_connect = function(conn_idx, is_success)
    print(111111)
    --print(is_success)
    if is_success then    
        -- (1)编码变成二进制
        byte, len = seri.pack(data)
        client:send_msg(conn_idx, byte, len)
    end
end

fun_close = function(conn_idx)
	print(conn_idx)
end

fun_recv = function(conn_idx, data, len)
    -- (2)解码变成Lua表
	local t = assert(seri.unpack(data, len))
    --client:send_msg(conn_idx, data, len);
end

for i=1, 1, 1 do
    client:connect("127.0.0.1", 10000, fun_connect, fun_close, fun_recv, 8192, 8192) 
end