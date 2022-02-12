/*
** CrossOver
** Copyright (C) 2009-2021 RedLight Team
** author: galen
**     qq: 88104725
*/

#ifndef LUA_FIX_H
#define LUA_FIX_H

#include "common.h"

#define TOLUA_REFID_FUNCTION_MAPPING "toluafix_refid_function_mapping"
#define TOLUA_REFID_PARAM_MAPPING "toluafix_refid_param_mapping"

void stack_dump(lua_State* L);
void check_param(lua_State* L, int count, const char* s);

void toluafix_open(lua_State* L);

int  generate_function_id();
int  toluafix_ref_function(lua_State* L, int lo);
void toluafix_get_function_by_refid(lua_State* L, int refid);
void toluafix_remove_function_by_refid(lua_State* L, int refid);

int  generate_param_id();
int  toluafix_ref_param(lua_State* L, int lo);
void toluafix_get_param_by_refid(lua_State* L, int refid);
void toluafix_remove_param_by_refid(lua_State* L, int refid);

bool lua_table_is_array(lua_State* L, int index);
int  lua_array_length(lua_State* L, int index);

#endif //LUA_FIX_H