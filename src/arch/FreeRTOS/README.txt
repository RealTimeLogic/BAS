Barracuda Web Server and App Server on FreeRTOS
==============================================

Use this port to run Barracuda Embedded Web Server (BWS) or Barracuda
App Server (BAS) with FreeRTOS+TCP or lwIP. BWS provides native C/C++ web
services; BAS adds Lua and Lua Server Pages (LSP).

For a first BAS application, start with Xedge in examples/xedge. Its build
guide covers resources, startup, and hardware bindings:
  https://github.com/RealTimeLogic/BAS/blob/main/doc/compiling.md#xedge-rtos

Build files
-----------

The source lists below use the full BAS SDK layout. See "Full SDK source
layout" at the end of this guide for the GitHub package equivalents.

Compile exactly one library:

  amalgamation/BWSCore.c    Native C/C++ applications
  amalgamation/BASCore.c    Applications using Lua or Xedge

Add src/arch/FreeRTOS/ThreadLib.c and select one network port:

  FreeRTOS+TCP
    Source:  src/arch/FreeRTOS/SoDisp.c
    Headers: inc and inc/arch/FreeRTOS

  lwIP
    Source:  src/arch/bsdSocket/LwIP/SoDisp.c
    Headers: inc, inc/arch/bsd/LwIP, and inc/arch/FreeRTOS

For lwIP, put its network headers before the FreeRTOS headers. Define
BA_FREERTOS in the build and BA_LWIP when using lwIP. Add your application
sources and initialize the board and network before starting the server.

Memory allocation
-----------------

The port defaults to the C runtime's malloc, realloc, and free. Lua makes
frequent use of realloc, so choose an allocator suited to your workload.

To use the included dlmalloc allocator, compile xrc/malloc/dlmalloc.c and set
BA_ALLOCATOR=BA_ALLOCATOR_DLMALLOC throughout the build. Initialize its
heap before any Barracuda allocation:

  #include <dlmalloc.h>

  /* Example heap size; measure your application's peak use. */
  static char poolBuf[1024 * 1024];
  init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));

Keep the buffer alive for the server's lifetime. Account for application
data, concurrent connections, TLS, and thread stacks when sizing memory.

Optional FreeRTOS+FAT storage
----------------------------

For writable files, add xrc/DiskIo/FreeRTOS-FAT/BaFile.c and initialize
the filesystem before using it. Barracuda calls its native filesystem
adapter DiskIo. It supports uses such as file uploads, WebDAV, and the
Web File Manager.

You can also keep the web application in an embedded ZIP and serve it
through ZipIo, without a native filesystem. NetIo provides another option
for loading files over the network during development.

Example startup code
--------------------

This skeleton shows where to initialize dlmalloc and install trace and
fatal-error callbacks. Supply those callbacks for your board, initialize
networking and cryptographic entropy, and choose a task stack and priority
for your application before using it.


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

/* dlmalloc and its allocator buffer */
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

Enable these settings in lwIP's lwipopts.h:
#define LWIP_DNS                        1
#define LWIP_SO_RCVTIMEO                1
#define LWIP_NETCONN_FULLDUPLEX         1

Optional IPv6 support requires lwIP 2.1 or later with both LWIP_IPV4=1 and
LWIP_IPV6=1. Compile BAS with BA_ENABLE_IPV6=1; leaving it disabled retains
the IPv4-only port. IPv6 sockets are IPv6-only, so create separate IPv4 and
IPv6 listeners when serving both families. Enable LWIP_IPV6_SCOPES for
link-local addresses. Scoped literals accept an interface index or lwIP
interface name, for example fe80::1%2 or fe80::1%ap1. Returned scoped
addresses include the numeric index. The application must configure IPv6
addresses on each interface; BAS does not configure the network stack.

The UDP port is shared across lwIP targets. The recorded ESP32-S3 tests
used the following limits and buffer settings: lwIP's 16-bit
pbuf total length limits ordinary IPv6 UDP payloads to 65487 bytes
(65535 minus 40 IP and 8 UDP header bytes); IPv4 permits 65507
bytes. Larger outgoing datagrams fail before allocation. Receiving
fragmented packets requires IPv6 reassembly and enough native
reassembly/driver buffers. The S3 tests use 64 reassembly pbufs and 64
dynamic Wi-Fi RX buffers; these are on-demand driver buffers, not a
BAS temporary receive allocation. Other targets must size their own
pools.

Warning: with IPv6 reassembly enabled, the tested ESP-IDF 6.1 stack on ESP32-S3
asserted in ip6_reass and rebooted on an incoming fragmented IPv6 UDP payload
of 65488 bytes; 65487 bytes passed. The failure occurs in lwIP before BAS
receives the packet, so BAS receive-buffer handling cannot prevent it.
Xedge32 uses the SDK's unmodified IPv6 reassembler and permits builds with
IPv6 reassembly enabled. This known native-stack issue remains until fixed
in the SDK; Xedge32 does not apply an automatic lwIP patch.

The port requires LWIP_NETCONN_FULLDUPLEX=1 at compile time. Mutex release
remains conditional on the caller, blocking mode and termination marker;
full-duplex support does not mean the mutex is always released. IPv6 does not
change this behavior. Retest blocked read/write cancellation with both
address families.

The lwIP netconn callback reports socket events but has no user-data
pointer. By default, the BAS port stores its pointer in an integer field,
which requires sizeof(int) == sizeof(void*). If those sizes differ on your
target, add a void* callbackData member at the end of lwIP's struct netconn
in api.h:

  netconn_callback callback;
  void* callbackData; /* User data for the BAS socket-event callback. */
};

Set the Boolean option BA_LWIP_USE_NETCONN_CALLBACK_DATA=1 consistently
across the build. Its default is 0. See src/arch/bsdSocket/LwIP/SoDisp.c for the
callback implementation.

Full SDK source layout
----------------------

The full SDK and the GitHub package use different source layouts. For an
SDK build with explicitly selected ports, use these equivalents:

  GitHub package                 Full SDK
  src/BAS.c                      amalgamation/BASCore.c
  src/BWS.c                      amalgamation/BWSCore.c
  src/arch/NET/                   src/arch/bsdSocket/
  inc/arch/NET/                   inc/arch/bsd/
  src/DiskIo/                    xrc/DiskIo/
  src/dlmalloc.c                 xrc/malloc/dlmalloc.c

ThreadLib.c and the OS-specific header directories keep the same paths.
Both core files include zlib.

The SDK also provides amalgamation/BAS.c and amalgamation/BWS.c, which
include the default target ports. If you use one of those wrappers, do
not add a second copy of its port sources. See amalgamation/README.txt
and examples/xedge/readme.html in the SDK for its build instructions.
