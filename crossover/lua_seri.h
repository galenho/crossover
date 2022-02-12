#ifndef LUA_SERIALIZE_H
#define LUA_SERIALIZE_H

#include <lua.h>

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

int _luaseri_pack(lua_State* L);
int _luaseri_unpack(lua_State* L);

int luaopen_lua_seri(lua_State* L);

C_END

#endif
