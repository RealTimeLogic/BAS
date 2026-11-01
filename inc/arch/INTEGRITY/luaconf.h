
#ifndef lconfig_h

#include <TargConfig.h>

/* Only a subset of the os lib is available for embedded systems */
/* For the POSIX runtime profile, set BA_HAVE_STDIO=1 and
 * BA_LUA_OS_LIBRARY_MODE=BA_LUA_OS_LIBRARY_FULL in the target configuration. */

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
