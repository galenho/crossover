
client = httpclient.new()

fun_get_callback = function(is_success, data, use_time)
	print(data)
end

client:init()
client:set_timeout(5000)
client:get("http://www.baidu.com", fun_get_callback, "")