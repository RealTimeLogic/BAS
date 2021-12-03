
#ifndef lconfig_h

#if 0
/* Use integers, not 'double' for Lua numbers */
#ifndef LUA_NUMBER_INTEGER
#define LUA_NUMBER_INTEGER 1
#endif
#endif

/* when LUA_NUMBER_INTEGER == 0 (using float). No log2 in VxWorks */
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

#include "../gluaconf.h"

#endif
