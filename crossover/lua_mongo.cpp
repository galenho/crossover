#include "lua_mongo.h"
#include "lua_fix.h"
#include "lua_bson.h"
#include "bson.h"
#include "mongo_task.h"

int lua_mongo_new(lua_State* L)
{
	mongo_database_t* t = (mongo_database_t*)lua_newuserdata(L, sizeof(*t));
	t->db = new MongoDatabase();

	luaL_getmetatable(L, "mongo");
	lua_setmetatable(L, -2);

	return 1;
}

int lua_mongo_initialize(lua_State* L)
{
	check_param(L, 4, "ussn");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	const char* dri = lua_tostring(L, 2);
	const char* db_name = lua_tostring(L, 3);
	uint32 conn_count = (uint32)lua_tointeger(L, 4);

	bool is_success = t->db->Initialize(dri, db_name, conn_count);
	lua_pushinteger(L, is_success);

	return 1;
}

int lua_mongo_shutdown(lua_State* L)
{
	check_param(L, 1, "u");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	t->db->Shutdown();

	return 0;
}

int lua_mongo_get_next_connect(lua_State* L)
{
	check_param(L, 1, "u");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	uint32 conn_idx = t->db->get_next_connect();
	lua_pushinteger(L, conn_idx);

	return 1;
}

int lua_mongo_get_free_connect(lua_State* L)
{
	check_param(L, 1, "u");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	uint32 conn_idx = t->db->get_free_connect();
	lua_pushinteger(L, conn_idx);

	return 1;
}

int lua_mongo_add_connect_ref(lua_State* L)
{
	check_param(L, 2, "un");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->db->add_connect_ref(conn_idx);

	return 0;
}

int lua_mongo_remove_connect_ref(lua_State* L)
{
	check_param(L, 2, "un");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	uint32 conn_idx = (uint32)lua_tointeger(L, 2);
	t->db->remove_connect_ref(conn_idx);

	return 0;
}

int lua_mongo_command_simple(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 3)
	{
		check_param(L, 3, "utf");
	}
	else if (count == 4)
	{
		check_param(L, 4, "utft");
	}
	else
	{
		check_param(L, 5, "utftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 3);

	if (count >= 4)
	{
		handler.param_id = toluafix_ref_param(L, 4);
	}

	if (count >= 5)
	{
		conn_idx = (uint32)lua_tointeger(L, 5);
	}

	int remove_count = count - 2;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	CommandSimpleMongoTask* query = new CommandSimpleMongoTask();
	query->task_type = MONGO_TASK_TYPE_COMMAND_SIMPLE;
	query->handler = handler;

	//------------------------------------------
	// 第2个参数
	//------------------------------------------
	struct bson b;
	bson_create(&b);
	pack_dict(L, &b, false, 0);
	bson_t* command_bson = bson_new_from_data(b.ptr, b.size);
	bson_destroy(&b);
	query->command_bson = command_bson;

	t->db->PushTask(query, conn_idx);

	return 0;
}

int lua_mongo_create_index(lua_State* L)
{
	// sample ---- db:create_index("player", {obj_idx=1}, fun_callback)
	check_param(L, 4, "ustf");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	lua_remove(L, -1);

	//stack_dump(L);
	CreateIndexMongoTask* query = new CreateIndexMongoTask();
	query->task_type = MONGO_TASK_TYPE_CREATE_INDEX;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b;
	bson_create(&b);
	pack_dict(L, &b, false, 0);
	bson_t* key_bson = bson_new_from_data(b.ptr, b.size);
	bson_destroy(&b);
	query->key_bson = key_bson;

	t->db->PushTask(query, 0);
	return 0;
}

int lua_mongo_drop_index(lua_State* L)
{
	// sample ---- db:drop_index("player", "index", fun_callback)
	check_param(L, 4, "ussf");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	const char* table_name = lua_tostring(L, 2);
	const char* index_name = lua_tostring(L, 3);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	//stack_dump(L);
	DropIndexMongoTask* query = new DropIndexMongoTask();
	query->task_type = MONGO_TASK_TYPE_DROP_INDEX;
	query->table_name = table_name;
	query->index_name = index_name;
	query->handler = handler;

	t->db->PushTask(query, 0);
	return 0;
}

int lua_mongo_destroy(lua_State* L)
{
	check_param(L, 1, "u");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	if (t->db)
	{
		delete t->db;
		t->db = NULL;
	}

	return 0;
}

int lua_mongo_drop(lua_State* L)
{
	// sample ---- db:drop("player", fun_callback)
	check_param(L, 3, "usf");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 3);

	//stack_dump(L);
	DropMongoTask* query = new DropMongoTask();
	query->task_type = MONGO_TASK_TYPE_DROP;
	query->table_name = table_name;
	query->handler = handler;

	t->db->PushTask(query, 0);
	return 0;
}

int lua_mongo_find_one(lua_State* L)
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第1步
	//---------------------------------------------
	//stack_dump(L);

	int count = lua_gettop(L);
	if (count == 5)
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {}, fun_callback)
		check_param(L, 5, "usttf");
	}
	else if (count == 6)
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {}, fun_callback, {})	
		check_param(L, 6, "usttft");
	}
	else
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {born_server=true, active_info=true}, fun_callback, {a=1, b=2}, 1)
		check_param(L, 7, "usttftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 5);

	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	if (count >= 7)
	{
		conn_idx = (uint32)lua_tointeger(L, 7);
	}

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	FindOneMongoTask* query = new FindOneMongoTask();
	query->task_type = MONGO_TASK_TYPE_FIND_ONE;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第4个参数
	//------------------------------------------
	lua_pushstring(L, "_id"); //去掉_id
	lua_pushboolean(L, false);
	lua_settable(L, -3);

	struct bson b1;
	bson_create(&b1);
	pack_dict(L, &b1, false, 0);
	bson_t* fields_bson = bson_new_from_data(b1.ptr, b1.size);
	bson_destroy(&b1);
	query->fields_bson = fields_bson;

	lua_remove(L, -1);

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b2;
	bson_create(&b2);
	pack_dict(L, &b2, false, 0);
	bson_t* query_bson = bson_new_from_data(b2.ptr, b2.size);
	bson_destroy(&b2);
	query->query_bson = query_bson;

	t->db->PushTask(query, conn_idx);

	return 0;
}

int lua_mongo_find(lua_State* L)
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第1步
	//---------------------------------------------
	//stack_dump(L);

	int count = lua_gettop(L);
	if (count == 5)
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {}, fun_callback)
		check_param(L, 5, "usttf");
	}
	else if(count == 6)
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {}, fun_callback, {a=1, b=2})	
		check_param(L, 6, "usttft");
	}
	else if (count == 7)
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {born_server=true, active_info=true}, fun_callback, {a=1, b=2}, 1)
		check_param(L, 7, "usttftn");
	}
	else
	{
		// sample ---- db:find("player", {born_server="gs10001"}, {born_server=true, active_info=true}, fun_callback, {a=1, b=2}, 1, 100)
		check_param(L, 8, "usttftnn");
	}
	
	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	int limit_count = 0;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 5);

	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	if (count >= 7)
	{
		conn_idx = (uint32)lua_tointeger(L, 7);
	}

	if (count >= 8)
	{
		limit_count = (uint32)lua_tointeger(L, 8);
	}

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	FindMongoTask* query = new FindMongoTask();
	query->task_type = MONGO_TASK_TYPE_FIND;
	query->table_name = table_name;
	query->handler = handler;
	query->limit = limit_count;

	//------------------------------------------
	// 第4个参数
	//------------------------------------------
	lua_pushstring(L, "_id"); //去掉_id
	lua_pushboolean(L, false);
	lua_settable(L, -3);

	struct bson b1;
	bson_create(&b1);
	pack_dict(L, &b1, false, 0);
	bson_t* fields_bson = bson_new_from_data(b1.ptr, b1.size);
	bson_destroy(&b1);
	query->fields_bson = fields_bson;
	
	lua_remove(L, -1);

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b2;
	bson_create(&b2);
	pack_dict(L, &b2, false, 0);
	bson_t* query_bson = bson_new_from_data(b2.ptr, b2.size);
	bson_destroy(&b2);
	query->query_bson = query_bson;

	t->db->PushTask(query, conn_idx);

	return 0;
}

int lua_mongo_update_one(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 5)
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback)
		check_param(L, 5, "usttf");
	}
	else if (count == 6)
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback, {a=1, b=2})
		check_param(L, 6, "usttft");
	}
	else
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 7, "usttftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 5);

	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	if (count >= 7)
	{
		conn_idx = (uint32)lua_tointeger(L, 7);
	}

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	UpdateOneMongoTask* query = new UpdateOneMongoTask();
	query->task_type = MONGO_TASK_TYPE_UPDATE_ONE;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第4个参数
	//------------------------------------------
	struct bson b1;
	bson_create(&b1);
	pack_dict(L, &b1, false, 0);
	bson_t* update_bson = bson_new_from_data(b1.ptr, b1.size);
	bson_destroy(&b1);
	query->update_bson = update_bson;

	lua_remove(L, -1);

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b2;
	bson_create(&b2);
	pack_dict(L, &b2, false, 0);
	bson_t* filter_bson = bson_new_from_data(b2.ptr, b2.size);
	bson_destroy(&b2);
	query->filter_bson = filter_bson;


	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_update_many(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 5)
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback)
		check_param(L, 5, "usttf");
	}
	else if (count == 6)
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback, {a=1, b=2})
		check_param(L, 6, "usttft");
	}
	else
	{
		// sample ---- db:update("player", {born_server="gs10001"}, {born_server="gs10001"}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 7, "usttftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = 0xFFFFFFFF;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	if (count >= 7)
	{
		conn_idx = (uint32)lua_tointeger(L, 7);
	}

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	UpdateManyMongoTask* query = new UpdateManyMongoTask();
	query->task_type = MONGO_TASK_TYPE_UPDATE_MANY;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第4个参数
	//------------------------------------------
	struct bson b1;
	bson_create(&b1);
	pack_dict(L, &b1, false, 0);
	bson_t* update_bson = bson_new_from_data(b1.ptr, b1.size);
	bson_destroy(&b1);
	query->update_bson = update_bson;

	lua_remove(L, -1);

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b2;
	bson_create(&b2);
	pack_dict(L, &b2, false, 0);
	bson_t* filter_bson = bson_new_from_data(b2.ptr, b2.size);
	bson_destroy(&b2);
	query->filter_bson = filter_bson;

	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_insert_one(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 4)
	{
		// sample ---- db:insert_one("player", {born_server="gs10001"}, fun_callback)
		check_param(L, 4, "ustf");
	}
	else if(count == 5)
	{
		// sample ---- db:insert_one("player", {born_server="gs10001"}, fun_callback, {a=1, b=2})
		check_param(L, 5, "ustft");
	}
	else
	{
		// sample ---- db:insert_one("player", {born_server="gs10001"}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 6, "ustftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	if (count >= 5)
	{
		handler.param_id = toluafix_ref_param(L, 5);
	}

	if (count >= 6)
	{
		conn_idx = (uint32)lua_tointeger(L, 6);
	}

	int remove_count = count - 3;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	InsertOneMongoTask* query = new InsertOneMongoTask();
	query->task_type = MONGO_TASK_TYPE_INSERT_ONE;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b;
	bson_create(&b);
	pack_dict(L, &b, false, 0);
	bson_t* doc_bson = bson_new_from_data(b.ptr, b.size);
	bson_destroy(&b);
	query->doc_bson = doc_bson;

	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_insert_many(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 4)
	{
		// sample ---- db:insert_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback)
		check_param(L, 4, "ustf");
	}
	else if (count == 5)
	{
		// sample ---- db:insert_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback, {a=1, b=2})
		check_param(L, 5, "ustft");
	}
	else
	{
		// sample ---- db:insert_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 6, "ustftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	if (count >= 5)
	{
		handler.param_id = toluafix_ref_param(L, 5);
	}

	if (count >= 6)
	{
		conn_idx = (uint32)lua_tointeger(L, 6);
	}

	int remove_count = count - 3;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	InsertManyMongoTask* query = new InsertManyMongoTask();
	query->task_type = MONGO_TASK_TYPE_INSERT_MANY;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	lua_pushnil(L);
	//stack_dump(L);

	bool throw_error = false;
	int documents_index = -2;

	int num_elements = 0;
	int lua_index = 1;
	for (; lua_next(L, documents_index) != 0; num_elements++, lua_index++)
	{
		//stack_dump(L);
		throw_error = !(lua_isnumber(L, -2)) || lua_tonumber(L, -2) != lua_index;
		if (throw_error)
		{
			luaL_error(L, "lua_mongo_insert_many ---- table param is error!");
		}

		struct bson b;
		bson_create(&b);
		pack_dict(L, &b, false, 0);
		bson_t* doc_bson = bson_new_from_data(b.ptr, b.size);
		bson_destroy(&b);
		query->doc_bson_list.push_back(doc_bson);

		//stack_dump(L);
		lua_pop(L, 1);
		//stack_dump(L);
	}

	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_delete_one(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 4)
	{
		// sample ---- db:delete_one("player", {born_server="gs10001"}, fun_callback)
		check_param(L, 4, "ustf");
	}
	else if (count == 5)
	{
		// sample ---- db:delete_one("player", {born_server="gs10001"}, fun_callback, {a=1, b=2})
		check_param(L, 5, "ustft");
	}
	else
	{
		// sample ---- db:delete_one("player", {born_server="gs10001"}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 6, "ustftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	if (count >= 5)
	{
		handler.param_id = toluafix_ref_param(L, 5);
	}

	if (count >= 6)
	{
		conn_idx = (uint32)lua_tointeger(L, 6);
	}

	int remove_count = count - 3;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	DeleteOneMongoTask* query = new DeleteOneMongoTask();
	query->task_type = MONGO_TASK_TYPE_DELETE_ONE;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b;
	bson_create(&b);
	pack_dict(L, &b, false, 0);
	bson_t* doc_bson = bson_new_from_data(b.ptr, b.size);
	bson_destroy(&b);
	query->doc_bson = doc_bson;

	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_delete_many(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 4)
	{
		// sample ---- db:delete_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback)
		check_param(L, 4, "ustf");
	}
	else if (count == 5)
	{
		// sample ---- db:delete_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback, {a=1, b=2})
		check_param(L, 5, "ustft");
	}
	else
	{
		// sample ---- db:delete_many("player", {{born_server="gs10001"}, {born_server="gs10002"}}, fun_callback, {a=1, b=2}, 0)
		check_param(L, 6, "ustftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 4);

	if (count >= 5)
	{
		handler.param_id = toluafix_ref_param(L, 5);
	}

	if (count >= 6)
	{
		conn_idx = (uint32)lua_tointeger(L, 6);
	}

	int remove_count = count - 3;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	DeleteManyMongoTask* query = new DeleteManyMongoTask();
	query->task_type = MONGO_TASK_TYPE_DELETE_MANY;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	lua_pushnil(L);
	//stack_dump(L);

	bool throw_error = false;
	int documents_index = -2;

	int num_elements = 0;
	int lua_index = 1;
	for (; lua_next(L, documents_index) != 0; num_elements++, lua_index++)
	{
		//stack_dump(L);
		throw_error = !(lua_isnumber(L, -2)) || lua_tonumber(L, -2) != lua_index;
		if (throw_error)
		{
			luaL_error(L, "lua_mongo_insert_many ---- table param is error!");
		}

		struct bson b;
		bson_create(&b);
		pack_dict(L, &b, false, 0);
		bson_t* doc_bson = bson_new_from_data(b.ptr, b.size);
		bson_destroy(&b);
		query->doc_bson_list.push_back(doc_bson);

		//stack_dump(L);
		lua_pop(L, 1);
		//stack_dump(L);
	}

	t->db->PushTask(query, conn_idx);
	return 0;
}

int lua_mongo_find_and_modify(lua_State* L)
{
	//stack_dump(L);

	int count = lua_gettop(L);
	if (count == 5)
	{
		// sample ---- db:find_and_modify("public_data", {name="serial_idx"}, {["$inc"] = {value = 1}}, fun_callback)
		check_param(L, 5, "usttf");
	}
	else if (count == 6)
	{
		// sample ---- db:find_and_modify("public_data", {name="serial_idx"}, {["$inc"] = {value = 1}}, fun_callback, {a=1, b=2})
		check_param(L, 6, "usttft");
	}
	else
	{
		// sample ---- db:find_and_modify("public_data", {name="serial_idx"}, {["$inc"] = {value = 1}}, fun_callback, {a=1, b=2}, 1)	
		check_param(L, 7, "usttftn");
	}

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");

	uint32 conn_idx = INVALID_INDEX;
	const char* table_name = lua_tostring(L, 2);

	HandleInfo handler;
	handler.fun_id = toluafix_ref_function(L, 5);
	
	if (count >= 6)
	{
		handler.param_id = toluafix_ref_param(L, 6);
	}

	if (count >= 7)
	{
		conn_idx = (uint32)lua_tointeger(L, 7);
	}

	int remove_count = count - 4;

	for (int i = 0; i < remove_count; i++)
	{
		lua_remove(L, -1);
	}

	//stack_dump(L);
	FindAndModifyMongoTask* query = new FindAndModifyMongoTask();
	query->task_type = MONGO_TASK_TYPE_FIND_AND_MODIFY;
	query->table_name = table_name;
	query->handler = handler;

	//------------------------------------------
	// 第4个参数
	//------------------------------------------
	struct bson b1;
	bson_create(&b1);
	pack_dict(L, &b1, false, 0);
	bson_t* update_bson = bson_new_from_data(b1.ptr, b1.size);
	bson_destroy(&b1);
	query->update_bson = update_bson;

	lua_remove(L, -1);

	//------------------------------------------
	// 第3个参数
	//------------------------------------------
	struct bson b2;
	bson_create(&b2);
	pack_dict(L, &b2, false, 0);
	bson_t* query_bson = bson_new_from_data(b2.ptr, b2.size);
	bson_destroy(&b2);
	query->query_bson = query_bson;

	t->db->PushTask(query, conn_idx);

	return 0;
}

int lua_mongo_ping(lua_State* L)
{
	check_param(L, 1, "u");

	mongo_database_t* t = (mongo_database_t*)luaL_checkudata(L, 1, "mongo");
	t->db->Ping();

	return 0;
}