
#ifndef lconfig_h

/* Only a subset of the os lib is available for embedded systems */
#define BA_MINIOSLIB
#define LUA_NOIOLIB
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
