
#ifndef lconfig_h

/* Default: Use integers, not 'double' for Lua numbers */
#if !defined(LUA_NUMBER_INTEGER)
#define LUA_NUMBER_INTEGER 1
#endif

/* Only a subset of the os lib is available for embedded systems */
#define BA_MINIOSLIB
#define LUA_NOIOLIB
#define LUA_PATH_DEFAULT ""
#define LUA_CPATH_DEFAULT ""
#define lua_getlocaledecpoint() '.'

/* Redirect sprintf functions to the more stack friendly BA versions */
#undef snprintf
#undef sprintf
#define snprintf nonosnprintf
#define sprintf nonosprintf
#include <stdio.h>
#undef snprintf
#undef sprintf
#define snprintf basnprintf
#define sprintf basprintf
extern int basnprintf(char* buf, int len, const char* fmt, ...);
extern int basprintf(char* buf, const char* fmt, ...);

#include <gluaconf.h>

#endif
