
Building Barracuda Embedded Web/Application Server for Quadros and Treck TCP/IP

  * Memory allocation
  * Zlib compression
  * Time function
  * Example code


-----------------------------  Prerequisite
View the Barracuda Web Server and Barracuda Application Server Setup
Tutorial video: http://youtu.be/1NqHB79cfpY


-----------------------------  Memory allocation

The Barracuda Server (BA) requires a memory allocator.
When using the LSP plugin, it is important to have a malloc
implementation with an efficient realloc function. The allocation
functions used by BA, which are defined in TargConfig.h, are
therefore configured to use dlmalloc. Consequently, you must include
dlmalloc in the build. It is however possible to redefine the allocation
functions in TargConfig.h if you prefer to use a different allocator.

-- To use dmalloc

Include xrc\malloc\dlmalloc.c in your build.

From your startup code, initiate the allocator as follows:

extern void init_dlmalloc(char* heapstart, char* heapend);
static char poolBuf[500*1024]; /* Minimum 500K when using LSP */
init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));


----------------------------- The Zlib compression library

Barracuda requires the ZLIB inflate/deflate library. You can use the
standard zlib library or build your application with our modified
version: xrc\zip\BaZlib.c


----------------------------- Time function

You must create the following function, which is used by BA.

unsigned long baGetUnixTime(void);

The function must return the number of seconds since January 1970.
UNIX time: https://en.wikipedia.org/wiki/Unix_time

The time logic required for the function can be setup by creating a
periodic one second tick using the Quadros kernel call
KS_SetCounterAcc.

The base value for the timer can be setup at system start by using a
hardware clock or by designing a Simple Network Time Protocol (SNTP)
client.

We provide a SNTP C code example in the Barracuda Server HTML
documentation under section: Manual -> C & C++ -> Examples -> Network
Client.


----------------------------- Example startup code

#include <HttpServer.h>
#include <HttpTrace.h>

/* xrc\malloc\dlmalloc.c.  */
extern void init_dlmalloc(char* heapstart, char* heapend);

/* The Barracuda examples entry function. Link with one of
   the examples.
*/
extern void barracuda(void);


static void
displayFatalError(const char* eMsg,
                  BaFatalErrorCodes ecode1,
                  unsigned int ecode2,
                  const char* file,
                  int line)
{
   // Do error handling
}

static void
myErrHandler(BaFatalErrorCodes ecode1,
             unsigned int ecode2,
             const char* file,
             int line)
{
   displayFatalError("Fatal error in Barracuda", ecode1, ecode2, file, line);
}


static void
writeHttpTrace(char* buf, int bufLen)
{
   buf[bufLen]=0; /* Zero terminate. Bufsize is at least bufLen+1. */
   //Emit buffer
}
 

/* The Barracuda Quadros socket dispatcher thread.
   Create and start this thread at startup.
*/
void barracudaThread(void)
{
   static char baPoolBuf[500*1024]; /* Minimum 500K when using LSP */
   /* IMPORTANT: Do this before any Barracuda calls */
   init_dlmalloc(baPoolBuf, baPoolBuf + sizeof(baPoolBuf));

   HttpTrace_setFLushCallback(writeHttpTrace);
   HttpServer_setErrHnd(myErrHandler);
   barracuda(); /* Run the Barracuda example code. Function never returns. */
}


/* Implement a function that returns seconds since Jan 1970
*/
unsigned long baGetUnixTime(void)
{
    return seconds since jan 1970;
}
