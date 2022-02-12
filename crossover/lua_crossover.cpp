#include "lua_crossover.h"
#include "lua_fix.h"
#include "console_poller.h"

extern CLog* g_logger;
extern bool running_;

int lua_set_console_handler(lua_State* L)
{
	check_param(L, 1, "f");

	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 1);
	handler.fun_id = fun_id;

	ConsolePoller::get_instance()->set_callback_handler(handler);

	return 0;
}

int lua_add_timer(lua_State* L)
{
	int count = lua_gettop(L);
	if (count == 2)
	{
		check_param(L, 2, "nf");
	}
	else
	{
		check_param(L, 3, "nft");
	}

	uint32 interval = (uint32)lua_tointeger(L, 1);

	HandleInfo handler;
	int fun_id = toluafix_ref_function(L, 2);
	handler.fun_id = fun_id;

	if (count == 3)
	{
		handler.param_id = toluafix_ref_param(L, 3);
	}
	
	uint32 timer_id = Scheduler::get_instance()->add_timer(interval, handler);
	lua_pushinteger(L, timer_id);

	return 1;
}

int lua_remove_timer(lua_State* L)
{
	check_param(L, 1, "n");

	uint32 index = (uint32)lua_tointeger(L, 1);
	Scheduler::get_instance()->remove_timer(index);

	return 0;
}

int lua_stop_all_timer(lua_State* L)
{
	Scheduler::get_instance()->stop_all_timer();

	return 0;
}

int lua_set_log_level(lua_State* L)
{
	check_param(L, 1, "n");

	int32 log_level = (int32)lua_tointeger(L, 1);
	g_logger->set_log_level(log_level);

	return 0;
}

int lua_save_log(lua_State* L)
{
	g_logger->Save();

	return 0;
}

int lua_write_log(lua_State* L)
{
	check_param(L, 2, "sn");

	const char* log_data = lua_tostring(L, 1);
	int32 log_level = (int32)lua_tointeger(L, 2);

	g_logger->WriteLogFile(log_level, log_data);

	return 0;
}

int lua_stop(lua_State* L)
{
	check_param(L, 1, "f");

	HandleInfo ondelay_handler;
	int fun_id = toluafix_ref_function(L, 1);
	ondelay_handler.fun_id = fun_id;

	ConsolePoller::get_instance()->Stop();

	Scheduler::get_instance()->stop_all_timer();
	Scheduler::get_instance()->add_timer(1000, ondelay_handler);

	return 0;
}

int lua_exit(lua_State* L)
{
	running_ = false;

	return 0;
}

void BuildXmlResult(TiXmlElement* node, lua_State* L)
{
	lua_newtable(L);//创建一个表格，放在栈顶

	int row = 1;
	TiXmlElement* child_node = node->FirstChildElement();//获取第一个子结点
	while (child_node)
	{
		lua_pushnumber(L, row);
		
		lua_newtable(L);
		
		//(1)遍历本结点的文本及属性
		const char* node_name = child_node->Value();
		
		lua_pushnumber(L, 0); //node_name
		lua_pushstring(L, node_name);
		lua_settable(L, -3);

		TiXmlAttribute* attribute = child_node->FirstAttribute();
		while (attribute)
		{
			const char* name = attribute->Name();
			const char* value = attribute->Value();

			lua_pushstring(L, name);
			lua_pushstring(L, value);
			lua_settable(L, -3);

			attribute = attribute->Next();
		}

		if (child_node->FirstChildElement()) //有子结点的话
		{
			lua_pushnumber(L, 1);
		
			//遍历本结点下的子结点
			BuildXmlResult(child_node, L);
			//stack_dump(L);

			lua_settable(L, -3);
			//stack_dump(L);
		}
		else //最终文本
		{
			const char* node_text = child_node->GetText();
			if (node_text)
			{
				lua_pushnumber(L, 1); //node_text
				lua_pushstring(L, node_text);
				lua_settable(L, -3);
			}
		}
		
		lua_settable(L, -3);

		//(3)继续遍历本结点的下一个兄弟结点
		child_node = child_node->NextSiblingElement(); 
		row++;
	}
}

int lua_loadxml(lua_State* L)
{
	check_param(L, 1, "s");

	const char* file = lua_tostring(L, 1);
	lua_remove(L, -1);

	TiXmlDocument doc(file);
	if (!doc.LoadFile())
	{
		luaL_error(L, "open file %s is fail!", file);
	}

	TiXmlElement* root = doc.RootElement();

	BuildXmlResult(root, L);
	//stack_dump(L);

	return 1;
}
