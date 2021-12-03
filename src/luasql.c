/*
** $Id: luasql.c 4914 2021-12-01 18:24:30Z wini $
** See Copyright Notice in license.html
*/

#include <string.h>

#ifndef BA_LIB
#define BA_LIB
#endif
#ifndef LUA_LIB
#define LUA_LIB
#endif

#include "balua.h"

#include "luasql.h"

/* this header cross depoendency is needed for the one
   basnprintf() call */
#include "BufPrint.h"

/*
** Typical database error situation
*/
LUASQL_API int luasql_faildirect(lua_State *L, const char *err) {
   lua_pushnil(L);
   lua_pushstring(L, err);
   return 2;
}


/*
** Create a metatable and leave it on top of the stack.
*/
LUASQL_API int luasql_createmeta (
   lua_State *L, const char *name, const luaL_Reg *methods) {
   if (!luaL_newmetatable (L, name))
      return 0;

   lua_pushvalue(L,BA_ENV_IX);
   lua_pushvalue(L,SQL_ENV_IX);
   luaL_setfuncs(L, methods, 2);

   lua_pushliteral (L, "__index");
   lua_pushvalue (L, -2);
   lua_settable (L, -3);

   lua_pushliteral (L, "__metatable");
   lua_pushliteral (L,LUASQL_PREFIX" private");
   lua_settable (L, -3);

   return 1;
}


/*
** Define the metatable for the object on top of the stack
*/
LUASQL_API void luasql_setmeta (lua_State *L, const char *name) {
   luaL_getmetatable (L, name);
   baAssert( ! lua_isnil(L, -1) );
   lua_setmetatable (L, -2);
}
