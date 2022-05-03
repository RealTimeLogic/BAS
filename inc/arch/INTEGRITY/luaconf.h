
#ifndef lconfig_h

/* Use integers, not 'double' for Lua numbers */
#ifndef LUA_NUMBER_INTEGER
#define LUA_NUMBER_INTEGER 1
#endif

/* Only a subset of the os lib is available for embedded systems */
#ifdef USE_SYSTEM_POSIX
#define BA_HAS_ANSI_IO
#else
#define BA_MINIOSLIB
#endif

#define LUA_PATH_DEFAULT ""
#define LUA_CPATH_DEFAULT ""
#ifndef lua_getlocaledecpoint
#define lua_getlocaledecpoint() '.'
#endif

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
