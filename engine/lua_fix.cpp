#include "lua_fix.h"
#include <iostream>

static int s_function_ref_id = 1;
static map<int, int> s_function_id_map;

static int s_param_ref_id = 1;
static map<int, int> s_param_id_map;

void stack_dump(lua_State* L)
{
	int i = 0;
	int top = lua_gettop(L); //获取栈顶下标
	for (i = top; i > 0; --i)
	{
		int t = lua_type(L, i); //获取栈上数据类型
		switch (t)
		{
		case LUA_TSTRING:
			{
				printf("|INDEX='%d','LUA_STRING=%s'|\n", i, lua_tostring(L, i));
			}
			break;
		case LUA_TBOOLEAN:
			{
				printf("|INDEX='%d','LUA_BOOLEAN=%s'|\n", i, lua_toboolean(L, i) ? "true" : "false");
			}
			break;
		case LUA_TNUMBER:
			{
				printf("|INDEX='%d','LUA_NUMBER=%g'|\n", i, lua_tonumber(L, i));
			}
			break;
		default:
			{
				printf("|INDEX='%d','DEFAULT=%s'|\n", i, lua_typename(L, t));
			}
			break;
		}
	}

	printf("\n\n");
}

void check_param(lua_State* L, int count, const char* s)
{
	int len = strlen(s);
	ASSERT(count == len);

	int param_count = lua_gettop(L);
	if (param_count != count)
	{
		luaL_error(L, "param is error!");
	}

	for (int i=0; i < param_count; i++)
	{
		//----------------------------------------------------
		//注: C是从0开始, lua从1开始, 提示参数错误从0开始
		//----------------------------------------------------
		int t = lua_type(L, i+1); //获取栈上数据类型
		switch (t)
		{
		case LUA_TSTRING:
			{
				if (s[i] != 's')
				{
					luaL_error(L, "param %d is not string!", i);
					return;
				}
			}
			break;
		case LUA_TBOOLEAN:
			{
				if (s[i] != 'b')
				{
					luaL_error(L, "param %d is not bool!", i);
					return;
				}
			}
			break;
		case LUA_TNUMBER:
			{
				if (s[i] != 'n')
				{
					luaL_error(L, "param %d is not member!", i);
					return;
				}
			}
			break;
		case LUA_TUSERDATA:
			{
				if (s[i] != 'u')
				{
					luaL_error(L, "param %d is not userdata!", i);
					return;
				}
			}
			break;
		case LUA_TTABLE:
			{
				if (s[i] != 't')
				{
					luaL_error(L, "param %d is not table!", i);
					return;
				}
			}
			break;
		case LUA_TFUNCTION:
			{
				if (s[i] != 'f')
				{
					luaL_error(L, "param %d is not fuction!", i);
					return;
				}
			}
			break;
		default:
			{
				luaL_error(L, "param %d is not define!", i);
				return;
			}
			break;
		}
	}
}

void toluafix_open(lua_State* L)
{
	lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);

	lua_pushstring(L, TOLUA_REFID_PARAM_MAPPING);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);
}

int generate_function_id()
{
	int function_id = s_function_ref_id;
	s_function_id_map[function_id] = function_id;

	while (true) // 寻找下一个可用的序号
	{	
		if (s_function_ref_id < 0x7fffffff) // 最大的正整数
		{
			s_function_ref_id++;
		}
		else
		{
			s_function_ref_id = 1;
		}

		if (s_function_id_map.find(s_function_ref_id) == s_function_id_map.end())
		{
			break;
		}
	}
	
	return function_id;
}


int toluafix_ref_function(lua_State* L, int lo)
{
	if (!lua_isfunction(L, lo))
		return 0;

	int fun_id = generate_function_id();

	lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: fun ... refid_fun */

	lua_pushinteger(L, fun_id);									/* stack: fun ... refid_fun refid */
	lua_pushvalue(L, lo);                                       /* stack: fun ... refid_fun refid fun */
	lua_rawset(L, -3);					/* refid_fun[refid] = fun, stack: fun ... refid_ptr */

	lua_pop(L, 1);                                              /* stack: fun ... */

	return fun_id;
}

void toluafix_get_function_by_refid(lua_State* L, int refid)
{
	lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(L, refid);                                  /* stack: ... refid_fun refid */
	lua_rawget(L, -2);                                          /* stack: ... refid_fun fun */
	lua_remove(L, -2);                                          /* stack: ... fun */
}

void toluafix_remove_function_by_refid(lua_State* L, int refid)
{
	lua_pushstring(L, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(L, refid);                                  /* stack: ... refid_fun refid */
	lua_pushnil(L);                                             /* stack: ... refid_fun refid nil */
	lua_rawset(L, -3);                  /* refid_fun[refid] = fun, stack: ... refid_ptr */
	lua_pop(L, 1);                                              /* stack: ... */

	s_function_id_map.erase(refid);
}

int generate_param_id()
{
	int param_id = s_param_ref_id;
	s_param_id_map[param_id] = param_id;

	while (true) // 寻找下一个可用的序号
	{
		if (s_param_ref_id < 0x7fffffff) // 最大的正整数
		{
			s_param_ref_id++;
		}
		else
		{
			s_param_ref_id = 1;
		}

		if (s_param_id_map.find(s_param_ref_id) == s_param_id_map.end())
		{
			break;
		}
	}

	return param_id;
}

int toluafix_ref_param(lua_State* L, int lo)
{
	// table at lo
	if (!lua_istable(L, lo))
		return 0;

	int param_id = generate_param_id();

	lua_pushstring(L, TOLUA_REFID_PARAM_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: param ... refid_param */
	lua_pushinteger(L, param_id);								/* stack: param ... refid_param refid */
	lua_pushvalue(L, lo);                                       /* stack: param ... refid_param refid parm */
	lua_rawset(L, -3);			    /* refid_param[refid] = param, stack: param ... refid_ptr */
	lua_pop(L, 1);                                              /* stack: param ... */

	return param_id;
}

void toluafix_get_param_by_refid(lua_State* L, int refid)
{
	lua_pushstring(L, TOLUA_REFID_PARAM_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_param */
	lua_pushinteger(L, refid);                                  /* stack: ... refid_param refid */
	lua_rawget(L, -2);                                          /* stack: ... refid_param parm */
	lua_remove(L, -2);                                          /* stack: ... fun */
}

void toluafix_remove_param_by_refid(lua_State* L, int refid)
{
	lua_pushstring(L, TOLUA_REFID_PARAM_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_param */
	lua_pushinteger(L, refid);                                  /* stack: ... refid_param refid */
	lua_pushnil(L);                                             /* stack: ... refid_param refid nil */
	lua_rawset(L, -3);                  /* refid_fun[refid] = fun, stack: ... refid_ptr */
	lua_pop(L, 1);                                              /* stack: ... */

	s_param_id_map.erase(refid);
}

int _convert_to_absolute_stack_index(lua_State* L, int index)
{
	return index > 0 ? index : lua_gettop(L) + index + 1;
}

bool lua_table_is_array(lua_State* L, int index)
{
	int absolute_stack_index = _convert_to_absolute_stack_index(L, index);
	return lua_array_length(L, absolute_stack_index) > 0;
}

int lua_array_length(lua_State* L, int index)
{
	int num_keys;
	int arr_index;
	int absolute_stack_index = _convert_to_absolute_stack_index(L, index);

	lua_pushnil(L);

	for (num_keys = 0; lua_next(L, absolute_stack_index) != 0; num_keys++) 
	{
		if ((lua_type(L, -2)) != LUA_TNUMBER) 
		{
			lua_pop(L, 2);
			return false;
		}
		
		arr_index = lua_tonumber(L, -2);
		lua_pop(L, 1);
		if (arr_index != num_keys + 1) 
		{
			return -1;
		}
	}

	return num_keys;
}