#ifndef LUA_CJSON_H
#define LUA_CJSON_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifndef C_BEGIN
# ifdef __cplusplus
#   define C_BEGIN extern "C" {
#   define C_END   }
# else
#   define C_BEGIN
#   define C_END
# endif
#endif /* C_BEGIN */

C_BEGIN

int luaopen_lua_cjson(lua_State* L);

C_END

#endif
