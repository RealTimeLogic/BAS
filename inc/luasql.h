/*
** $Id: luasql.h 4914 2021-12-01 18:24:30Z wini $
** See Copyright Notice in license.html
*/

#ifndef _LUASQL_
#define _LUASQL_

#ifndef LUASQL_API
#define LUASQL_API
#endif

#define LUASQL_PREFIX "LuaSQL: "
#define LUASQL_TABLENAME "luasql"
#define LUASQL_ENVIRONMENT "Each driver must have an environment metatable"
#define LUASQL_CONNECTION "Each driver must have a connection metatable"
#define LUASQL_CURSOR "Each driver must have a cursor metatable"

/* Barracuda BA_ENV_IX is on upvalue position 1 */
#define SQL_ENV_IX lua_upvalueindex(2)



typedef struct {
	short  closed;
} pseudo_data;

LUASQL_API int luasql_faildirect (lua_State *L, const char *err);
LUASQL_API int luasql_createmeta (lua_State *L, const char *name, const luaL_Reg *methods);
LUASQL_API void luasql_setmeta (lua_State *L, const char *name);
LUASQL_API void luasql_set_info (lua_State *L);

#endif
