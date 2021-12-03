
#ifndef lconfig_h

#ifdef _WIN32_WCE

#define NO_TIMEFUNCS
#define NO_LOG2 /* No log2() in Win-CE */

/* Only a subset of the os lib is available for embedded systems */
#define BA_MINIOSLIB
#define abort() baFatalE(FE_EXIT,__LINE__)
#define lua_getlocaledecpoint() '.'
#define getenv(x) 0
#define time(x) baGetUnixTime()
#define strcoll strcmp
#define BA_MINIOSLIB


#else


#define BA_HAS_ANSI_IO

#endif

#include "../gluaconf.h"
#endif
