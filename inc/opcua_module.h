#ifndef OPCUA_MODULE_H
#define OPCUA_MODULE_H

#include <lauxlib.h>
#include <lualib.h>

/*
   an entry point for lua dynamically linked module
*/
LUALIB_API int luaopen_opcua_ns0(lua_State* l);

/*
   an entry point for statically linked module
   calls luaL_requiref to register opcuac module
*/
void luaopen_opcua_ns0_static(lua_State* l);

#endif // OPCUA_MODULE_H


