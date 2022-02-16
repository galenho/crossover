package.path = package.path..";../common/?.lua"
package.cpath = package.cpath..";../?.dll"

require("common")
local util = require("util")
db = mysql.new()
bind_conn_idx = 0

fun_drop_callback = function(is_success, rs)
    if is_success then
		print("drop is success")
	end
end

function test_drop()
	sql = "DROP TABLE IF EXISTS `config_scene`;"
	db:add_query(fun_drop_callback, sql, {}, bind_conn_idx)
end

fun_create_callback = function(is_success, rs)
    if is_success then
		print("create is success")
	end
end

function test_create()
	sql = "CREATE TABLE `config_scene` (\
			`type_idx` int(10) NOT NULL,\
			`scene_name` char(36) NOT NULL,\
			`area_idx` int(10) NOT NULL,\
			`instance_rule_type` tinyint(3) unsigned NOT NULL,\
			`map_id` int(11) unsigned NOT NULL,\
			`map_instance_id` int(11) unsigned NOT NULL,\
			PRIMARY KEY (`type_idx`)\
			) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;"

	db:add_query(fun_create_callback, sql, {}, bind_conn_idx)
end

fun_query_callback = function(is_success, rs)
    if is_success then
		dump(rs)
	end
end

fun_query_callback_ex = function(is_success, rs, param)
    if is_success then
		dump(rs)
	end
end

function test_query()
	
	sql = "select scene_name, type_idx, map_id from config_scene where type_idx = ?"
	sql_param = {101}
	
	params = {a=1, b=2}
	tran_flag = 0

	db:add_query(sql, fun_query_callback, sql_param)
	db:add_query(sql, fun_query_callback_ex, sql_param, bind_conn_idx, params, tran_flag)
	
end

fun_insert_callback = function(is_success, rs, params)
    if is_success then
		--print("insert is success")
	end
end

function test_insert()
	for i=1,10000,1 do
		sql = "insert into config_scene values(?, ?, ?, ?, ?, ?)"
		sql_param = {i, "kof", 1, 1, 1, 1}
		if i == 1 then
			db:add_query(fun_insert_callback, sql, sql_param, bind_conn_idx, {}, 1)
		elseif i == 10000 then
			db:add_query(fun_insert_callback, sql, sql_param, bind_conn_idx, {}, 3)
		else
			db:add_query(fun_insert_callback, sql, sql_param, bind_conn_idx, {})
		end
	end
end

fun_query_callback = function(is_success, rs)
    if is_success then
		dump(rs)
	end
end

fun_query_callback_ex = function(is_success, rs, param)
    if is_success then
		dump(rs)
	end
end

function test_query()
	
	sql = "select scene_name, type_idx, map_id from config_scene where type_idx = ?"
	sql_param = {101}
	bind_conn_idx = 0
	params = {a=1, b=2}
	tran_flag = 0

	db:add_query(fun_query_callback, sql, sql_param, bind_conn_idx)
	db:add_query(fun_query_callback_ex, sql, sql_param, bind_conn_idx, params, tran_flag)
	
end

function test_mysql()
	db:initialize("127.0.0.1", 4769, "root", "root", "testdb", "utf8", 2)

	db:ping()
	
	test_drop()
	test_create()
	test_insert()
	test_query()
end

test_mysql()


