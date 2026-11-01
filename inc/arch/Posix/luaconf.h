
#ifndef lconfig_h

#include <TargConfig.h>


/* Redirect sprintf functions to the more stack friendly BA versions */

#if BA_ENABLE_SERVER
#ifdef sprintf
#undef snprintf
#undef sprintf
#define snprintf nonosnprintf
#define sprintf nonosprintf
#endif
#include <stdio.h>
#undef snprintf
#undef sprintf
#define snprintf basnprintf
#define sprintf basprintf
extern int basnprintf(char* buf, int len, const char* fmt, ...);
extern int basprintf(char* buf, const char* fmt, ...);
#endif

/* Set lua_getlocaledecpoint() in the port for a dot-only runtime. */

#include <gluaconf.h>

#endif
