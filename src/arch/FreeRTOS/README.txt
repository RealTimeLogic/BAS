
Building Barracuda Embedded Web/Application Server for FreeRTOS+TCP

  * Compiling
  * Examples
  * Memory management
  * Using FreeRTOS+FAT with Barracuda
  * Example startup code


-----------------------------  Compiling the source code library

View the Barracuda Web Server (BWS) and Barracuda Application Server (BAS)
Setup Tutorial video: http://youtu.be/1NqHB79cfpY

Include one of the following in your makefile/IDE:

amalgamation/BWS.c 
amalgamation/BAS.c

Include paths:
  Generic: inc
  FreeRTOS+TCP: inc/arch/FreeRTOS
  FreeRTOS lwIP: inc/arch/bsd/LwIP and then inc/arch/FreeRTOS
                 See the end of this readme for lwIP instructions.

Required Macros:
FreeRTOS: BA_FREERTOS
If using lwIP: BA_LWIP


See amalgamation/README.txt for details.

In addition, include one of the two following examples when using BAS:

-----------------------------  Recommended BAS Examples

https://realtimelogic.com/ba/doc/?url=LspNetIo/readme.html
https://realtimelogic.com/ba/doc/?url=lspappmgr/readme.html


-----------------------------  Memory management

The BWS and BAS requires a memory allocator.
When using the LSP plugin, it is important to have a malloc
implementation with an efficient realloc function. The allocation
functions used by BWS/BAS, which are defined in TargConfig.h, are
therefore configured to use dlmalloc. Consequently, you must include
dlmalloc in the build. It is however possible to redefine the allocation
functions in TargConfig.h if you prefer to use a different allocator.

From your startup code, initiate the allocator as follows:

extern void init_dlmalloc(char* heapstart, char* heapend);
static char poolBuf[1024*1024]; /* 1M, minimum 600K when using LSP */
init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));

In your IDE, define the following: USE_DLMALLOC
BAS uses the default allocator (malloc/free) if this macro is not set.


----------------------------- Using FreeRTOS+FAT

One can optionally add FreeRTOS+FAT file support in Barracuda by
compiling and linking the following file with your firmware:
xrc\DiskIo\FreeRTOS-FAT\BaFile.c

Native file system support in Barracuda is called DiskIo, and all
examples can optionally be compiled with DiskIo integration.

File system support is not needed unless you plan on remotely managing
files in the device by, for example, using the WebDAV server or the Web
File Manager. We do not recommend storing web applications directly as
files on the file system. Instead use the NetIo for development and the ZipIo
for deployment. See the main documentation for more information.



----------------------------- Example startup code

#include <HttpServer.h>
#include <HttpTrace.h>

extern void init_dlmalloc(char* heapstart, char* heapend);

static void
myErrHandler(BaFatalErrorCodes ecode1,
             unsigned int ecode2,
             const char* file,
             int line)
{
   // Do error handling / reboot
}


static void
writeHttpTrace(char* buf, int bufLen)
{
   buf[bufLen]=0; /* Zero terminate. Bufsize is at least bufLen+1. */
   //Emit buffer e.g. printf("%s", buf);
}
 

/* The Barracuda examples entry function. Link with one of
   the examples.
*/
extern void barracuda(void);

/* dlmalloc and it's allocator buffer */
extern void init_dlmalloc(char* heapstart, char* heapend);
static char poolBuf[1024 * 1024];

/* The Barracuda thread
*/
static void
barracudaTask(void* notused)
{
   (void)notused;
   init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));
   HttpTrace_setFLushCallback(writeHttpTrace);
   HttpServer_setErrHnd(myErrHandler);
   barracuda(); /* Run the Barracuda example code. Function never returns. */
}

.
.
.

xTaskCreate(barracudaTask, /* The function that implements the demo task. */
      "BAS", /* The name to assign to the task being created. */
      2000,  /* The stack size in WORDS. */
      NULL,  /* The task parameter is not being used. */
      4,     /* The priority at which the task being created will run. */
      NULL);
    

----------------------------- FreeRTOS and lwIP

Required settings in lwIP opt.h that is not enabled by default:
#define LWIP_DNS                        1
#define LWIP_SO_RCVTIMEO                1

The lwIP BAS port uses the netconn callback API. This lwIP API
provides a mechanism for the lwIP TCP stack to call into the BAS lwIP
porting layer for each socket event, however, although the API
includes a function pointer, the API does not include a callback data
pointer. The BAS lwIP porting API also requires a data pointer. The
data pointer is by default cast to an int and back again to a
pointer. This requires sizeof(int) == sizeof(void*). If the int size
differs from void* size on your platform, add the following to the end
of lwIP's netconn structure (in api.h): void* callbackData;

The end of the structure should then look like:
  netconn_callback callback;
   void* callbackData; /* PATCH needed by BAS */
};

You must also set the define USE_CALLBACKDATA
See src/arch/bsdSocket/LwIP/SoDisp.c for details.
