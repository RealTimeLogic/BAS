
/*
  Include all I/O related header files
*/
#include <barracuda.h>

typedef int (*InitXedge)(lua_State* L, int initXedgeFuncRef);
typedef int (*XedgeCfgFile)(lua_State* L);

typedef struct {
   lua_State* L;
   IoIntfPtr dio;
   InitXedge initXedge;
   int initXedgeFuncRef;
   XedgeCfgFile xedgeCfgFile;
} XedgeOpenAUX;

#ifndef NO_BAIO_DISK
int xedgeInitDiskIo(DiskIo* dio);
#endif
int xedgeOpenAUX(XedgeOpenAUX* aux);




