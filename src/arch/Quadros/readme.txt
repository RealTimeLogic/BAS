Barracuda Web Server and App Server on Quadros
=============================================

This port uses the Quadros RTXC kernel and Treck TCP/IP. Choose Barracuda
Embedded Web Server (BWS) for native C/C++ web services, or Barracuda
App Server (BAS) for Lua and Lua Server Pages (LSP) applications.

Build files
-----------

The source lists below use the GitHub package layout. If you have the full
BAS SDK, see "Full SDK source layout" at the end of this guide before
selecting files.

Compile src/BWS.c or src/BAS.c, plus these port files:

  src/arch/Quadros/ThreadLib.c
  src/arch/NET/generic/SoDisp.c

Add inc and inc/arch/Quadros to the include path, together with the
headers for your kernel and TCP/IP stack. Add your application or Xedge
host sources separately. The Xedge build guide covers resource generation:
  https://github.com/RealTimeLogic/BAS/blob/main/doc/compiling.md#xedge-rtos

The amalgamated library already includes zlib. Do not compile a second
copy of the compression library.

Memory allocation
-----------------

The port defaults to malloc, realloc, and free from the C runtime. Lua
uses realloc frequently, so check that your allocator handles this
workload well.

To use the supplied dlmalloc allocator, compile src/dlmalloc.c and set
BA_ALLOCATOR=BA_ALLOCATOR_DLMALLOC throughout the build. Initialize it
before any Barracuda allocation:

  #include <dlmalloc.h>

  /* Example heap size; adjust it to the application's measured needs. */
  static char poolBuf[1024 * 1024];
  init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));

The heap buffer must remain valid while the server runs.

Provide the clock
-----------------

Implement the function declared by inc/arch/Quadros/TargConfig.h:

  BaTime baGetUnixTime(void);

It takes no arguments and returns UTC seconds since 1970-01-01 as a
BaTime value. Set an initial value from a hardware clock or network time
service, then advance it using your platform timer. The port uses the
Quadros TIMEBASE counter and CLKTICK for its separate millisecond clock.

Start the server
----------------

After initializing the kernel and network, start a dedicated Barracuda
task. Before calling the chosen example's barracuda(void) entry point:

  1. Initialize the allocator, if using dlmalloc.
  2. Set the UTC clock and provide cryptographic entropy for TLS.
  3. Register board-specific callbacks with HttpTrace_setFLushCallback()
     and HttpServer_setErrHnd().

Keep this task and its resources alive while the server runs. Replace the
host startup files with your Quadros startup code; desktop startup does
not initialize the board or Treck stack.

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
