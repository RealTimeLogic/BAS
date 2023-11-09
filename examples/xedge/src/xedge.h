
/*
  Include all I/O related header files
*/
#include <barracuda.h>

typedef int (*InitXedge)(lua_State* L, int initXedgeFuncRef);

typedef struct {
   lua_State* L;
   IoIntfPtr dio;
#ifndef NO_ENCRYPTIONKEY
   InitXedge initXedge;
   int initXedgeFuncRef;
#endif
} XedgeOpenAUX;


int xedgeInitDiskIo(DiskIo* dio);
int xedgeOpenAUX(XedgeOpenAUX* aux);




