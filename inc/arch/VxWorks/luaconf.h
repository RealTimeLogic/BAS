
#ifndef lconfig_h

#define NO_LOG2

/*  Use the standard Lua I/O and loadfile. The C libraries required
 *  for this may be available in your VxWorks version.
 */
/* #define BA_HAS_ANSI_IO */

#ifndef BA_HAS_ANSI_IO
#define BA_MINIOSLIB
#endif

#define LUA_PATH_DEFAULT ""
#define LUA_CPATH_DEFAULT ""

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
