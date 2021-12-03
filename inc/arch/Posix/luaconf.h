
#ifndef lconfig_h

#define BA_HAS_ANSI_IO

/* Redirect sprintf functions to the more stack friendly BA versions */
#ifndef NO_BA_SERVER
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
#endif

#ifdef NO_LOCALDECPOINT
#define lua_getlocaledecpoint() '.'
#endif

#include "../gluaconf.h"

#endif
