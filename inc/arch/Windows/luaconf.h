
#ifndef lconfig_h

#include <TargConfig.h>

#ifdef _WIN32_WCE


/* Only a subset of the os lib is available for embedded systems */
#define abort() baFatalE(FE_EXIT,__LINE__)
#define lua_getlocaledecpoint() '.'
#define getenv(x) 0
#define time(x) baGetUnixTime()
#define strcoll strcmp


#else



#endif

#include <gluaconf.h>
#endif
