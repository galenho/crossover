#include "common.h"
#include "task.h"
#include "lua_fix.h"
#include "lua_bson.h"
#include "bson.h"
#include "scheduler.h"

void LuaCall(lua_State* L, int nargs)
{
	//stack_dump(L);
	
	int nresults = 0;
	int ret = lua_pcall(L, nargs, nresults, g_erro_func);
	if (ret != 0) //有返回值才需要出栈
	{
		lua_pop(L, 1);
	}

	//stack_dump(L);
}

Task::Task()
{
	
}

Task::~Task()
{
	
}

TimerTask::TimerTask()
{
	index_ = 0;
}

TimerTask::~TimerTask()
{
	
}

void TimerTask::Init(HandleInfo handle, uint32 index)
{
	handle_ = handle;
	index_ = index;
}

void TimerTask::process()
{
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);
		lua_pushinteger(g_lua_state, index_);

		if (handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handle_.param_id);
			LuaCall(g_lua_state, 2);
		}
		else
		{
			LuaCall(g_lua_state, 1);
		}
	}
}

InputTask::InputTask()
{

}

InputTask::~InputTask()
{

}

void InputTask::Init(HandleInfo handle, string& cmd)
{
	handle_ = handle;
	cmd_ = cmd;
}

void InputTask::process()
{
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);
		lua_pushstring(g_lua_state, cmd_.c_str());
		LuaCall(g_lua_state, 1);
	}
}

DBTask::DBTask()
{
	rs_ = NULL;
}

DBTask::~DBTask()
{
	if (handle_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handle_.fun_id);
	}

	if (handle_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handle_.param_id);

	}

	if (rs_)
	{
		delete rs_;
		rs_ = NULL;
	}
}

void DBTask::Init(HandleInfo handle, ResultSet* rs)
{
	handle_ = handle;
	rs_ = rs;
}

void DBTask::process()
{
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);

		lua_pushboolean(g_lua_state, rs_->is_success);
		bool is_success = rs_->BuildLuaResult(g_lua_state);
		ASSERT(is_success);

		if (handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handle_.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

MongoDBTask::MongoDBTask()
{
	rs_ = NULL;
	is_one_table_ = false;
}

MongoDBTask::~MongoDBTask()
{
	if (handle_.fun_id > 0)
	{
		toluafix_remove_function_by_refid(g_lua_state, handle_.fun_id);
	}

	if (handle_.param_id > 0)
	{
		toluafix_remove_param_by_refid(g_lua_state, handle_.param_id);
	}

	if (rs_)
	{
		delete rs_;
		rs_ = NULL;
	}
}

void MongoDBTask::Init(HandleInfo handle, MongoResultSet* rs, bool is_one_table /*= false*/)
{
	handle_ = handle;
	rs_ = rs;
	is_one_table_ = is_one_table;
}

void MongoDBTask::process()
{
	//---------------------------------------------
	// galen: mongos异步查询 ---- 第3步
	//---------------------------------------------
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);

		lua_pushboolean(g_lua_state, rs_->is_success);
		
		if (is_one_table_) //单个table
		{
			if (rs_->doc_list.size() == 1)
			{
				const bson_t* doc = rs_->doc_list[0];
				struct bson_reader br = { bson_get_data(doc), (int)doc->len };
				unpack_dict(g_lua_state, &br, false);
			}
			else if (rs_->doc_list.size() == 0)
			{
				lua_newtable(g_lua_state); //空的表格
			}
			else
			{
				luaL_error(g_lua_state, "one_table error! ------ the result count is > 1");
			}
		}
		else
		{
			lua_newtable(g_lua_state);//创建一个表格，放在栈顶

			for (uint32 n = 0; n < rs_->doc_list.size(); n++)
			{
				//stack_dump(g_lua_state);

				lua_pushnumber(g_lua_state, n + 1); // key为自增int

				const bson_t* doc = rs_->doc_list[n];
				struct bson_reader br = { bson_get_data(doc), (int)doc->len };
				unpack_dict(g_lua_state, &br, false);

				lua_settable(g_lua_state, -3);
			}
		}

		if (handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handle_.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

TcpConnectTask::TcpConnectTask()
{
	conn_idx_ = 0;
	is_success_ = false;
	is_tcp_client_ = false;
}

TcpConnectTask::~TcpConnectTask()
{
	if (!is_success_)
	{
		if (is_tcp_client_)
		{
			if (connect_handle_.fun_id > 0)
			{
				toluafix_remove_function_by_refid(g_lua_state, connect_handle_.fun_id);
			}

			if (connect_handle_.param_id > 0)
			{
				toluafix_remove_param_by_refid(g_lua_state, connect_handle_.param_id);
			}

			if (recv_handle_.fun_id > 0)
			{
				toluafix_remove_function_by_refid(g_lua_state, recv_handle_.fun_id);
			}

			if (recv_handle_.param_id > 0)
			{
				toluafix_remove_param_by_refid(g_lua_state, recv_handle_.param_id);
			}

			if (close_handle_.fun_id > 0)
			{
				toluafix_remove_function_by_refid(g_lua_state, close_handle_.fun_id);
			}

			if (close_handle_.param_id > 0)
			{
				toluafix_remove_param_by_refid(g_lua_state, close_handle_.param_id);
			}
		}
	}
}

void TcpConnectTask::Init(HandleInfo connect_handle, HandleInfo recv_handle, HandleInfo close_handle, uint32 conn_idx, bool is_success, bool is_tcp_client)
{
	connect_handle_ = connect_handle;
	recv_handle_ = recv_handle;
	close_handle_ = close_handle;
	conn_idx_ = conn_idx;
	is_tcp_client_ = is_tcp_client;

	is_success_ = is_success;
}

void TcpConnectTask::process()
{
	if (connect_handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, connect_handle_.fun_id);
		lua_pushinteger(g_lua_state, conn_idx_);
		lua_pushboolean(g_lua_state, is_success_);
		
		if (connect_handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, connect_handle_.param_id);
			LuaCall(g_lua_state, 3);
		}
		else
		{
			LuaCall(g_lua_state, 2);
		}
	}
}

TcpReadTask::TcpReadTask()
{
	conn_idx_ = 0;
	data_ = NULL;
	data_len_ = 0;
}

TcpReadTask::~TcpReadTask()
{
	if (data_)
	{
		delete[] data_;
		data_ = NULL;
	}
}

void TcpReadTask::Init(HandleInfo handle, uint32 conn_idx, char* data, uint32 data_len)
{
	handle_ = handle;
	conn_idx_ = conn_idx;
	data_len_ = data_len;
	data_ = new char[data_len];
	memcpy(data_, data, data_len);
}

void TcpReadTask::process()
{
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);
		lua_pushinteger(g_lua_state, conn_idx_);
		lua_pushlstring(g_lua_state, data_, data_len_);
		lua_pushinteger(g_lua_state, data_len_);

		if (handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, handle_.param_id);
			LuaCall(g_lua_state, 4);
		}
		else
		{
			LuaCall(g_lua_state, 3);
		}
	}
}

TcpCloseTask::TcpCloseTask()
{
	conn_idx_ = 0;
	is_tcp_client_ = false;
}

TcpCloseTask::~TcpCloseTask()
{
	if (is_tcp_client_)
	{
		if (connect_handle_.fun_id > 0)
		{
			toluafix_remove_function_by_refid(g_lua_state, connect_handle_.fun_id);
		}

		if (connect_handle_.param_id > 0)
		{
			toluafix_remove_param_by_refid(g_lua_state, connect_handle_.param_id);
		}

		if (recv_handle_.fun_id > 0)
		{
			toluafix_remove_function_by_refid(g_lua_state, recv_handle_.fun_id);
		}

		if (recv_handle_.param_id > 0)
		{
			toluafix_remove_param_by_refid(g_lua_state, recv_handle_.param_id);
		}

		if (close_handle_.fun_id > 0)
		{
			toluafix_remove_function_by_refid(g_lua_state, close_handle_.fun_id);
		}

		if (close_handle_.param_id > 0)
		{
			toluafix_remove_param_by_refid(g_lua_state, close_handle_.param_id);
		}
	}
}

void TcpCloseTask::Init(HandleInfo connect_handle, HandleInfo recv_handle, HandleInfo close_handle, uint32 conn_idx, bool is_tcp_client)
{
	connect_handle_ = connect_handle;
	recv_handle_ = recv_handle;
	close_handle_ = close_handle;
	conn_idx_ = conn_idx;
	is_tcp_client_ = is_tcp_client;
}

void TcpCloseTask::process()
{
	if (close_handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, close_handle_.fun_id);
		lua_pushinteger(g_lua_state, conn_idx_);

		if (close_handle_.param_id > 0)
		{
			toluafix_get_param_by_refid(g_lua_state, close_handle_.param_id);
			LuaCall(g_lua_state, 2);
		}
		else
		{
			LuaCall(g_lua_state, 1);
		}
	}
}

HttpTask::HttpTask()
{
	is_success_ = false;
	use_time_ = 0;
}

HttpTask::~HttpTask()
{
	
}

void HttpTask::Init(HandleInfo handle, bool is_success, string& recv_buff, uint32 use_time)
{
	handle_ = handle;
	is_success_ = is_success;
	recv_buff_ = recv_buff;
	use_time_ = use_time;
}

void HttpTask::process()
{
	if (handle_.fun_id > 0)
	{
		toluafix_get_function_by_refid(g_lua_state, handle_.fun_id);
		lua_pushboolean(g_lua_state, is_success_);
		lua_pushstring(g_lua_state, recv_buff_.c_str());
		lua_pushinteger(g_lua_state, use_time_);
		
		LuaCall(g_lua_state, 3);
	}
}
