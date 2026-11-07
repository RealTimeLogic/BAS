Barracuda App Server on VxWorks
==============================

Use this port to add web services and Lua applications to a VxWorks
system. For a new embedded integration, start with examples/xedge and
the Xedge build guide. It also includes a Mako Server command-line build
for applications that need a standalone process:
  https://github.com/RealTimeLogic/BAS/blob/main/doc/compiling.md

Build the library
-----------------

The source lists below use the full BAS SDK layout. See "Full SDK source
layout" at the end of this guide for the GitHub package equivalents.

Paths here are relative to the BAS SDK root. Compile one library:

  amalgamation/BASCore.c    For Lua, Lua Server Pages (LSP), or Xedge
  amalgamation/BWSCore.c    For native C/C++ web services without Lua

Add the VxWorks thread and socket ports:

  src/arch/VxWorks/ThreadLib.c
  src/arch/bsdSocket/generic/SoDisp.c

Use these include directories, along with your VxWorks SDK headers:

  inc
  inc/arch/VxWorks

The VxWorks port supplies its own HttpCfg.h. Keep the selected port
headers ahead of any alternative socket headers in your include path.
TargConfig.h defines BA_VXWORKS.

Add the chosen host or application sources and generated resources.
The core amalgamation includes zlib, but it does not include the thread,
socket, filesystem, or dlmalloc sources listed in this guide. Compile
each required port source once.

Startup
-------

Initialize the network, UTC clock, heap, and platform entropy source
before starting the application. Register trace output and a fatal-error
handler for your device. Run the example's barracuda(void) entry point
from a dedicated task with enough stack for the selected application.
See the Xedge section of the build guide for source files and startup hooks.

When creating a VxWorks task that uses floating point, include VX_FP_TASK
in its task options. For DIAB builds, compile all sources with
-Xchar-unsigned, as required by this port's build configuration.

Memory allocation
-----------------

The port defaults to the VxWorks C allocation routines. To use dlmalloc,
set BA_ALLOCATOR=BA_ALLOCATOR_DLMALLOC consistently across the build,
compile xrc/malloc/dlmalloc.c, and initialize its heap before calling BAS:

  #include <dlmalloc.h>

  /* Example size; choose a heap for your application's measured needs. */
  static char poolBuf[1024 * 1024];
  init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));

Keep the buffer alive while BAS runs. Lua needs an efficient realloc
implementation; allow for application data and concurrent TLS connections.

Network options
---------------

These Boolean build options accept 0 or 1:

  BA_USE_GETADDRINFO   Default 1 in the VxWorks port. Uses getaddrinfo
                      for hostname resolution.
  BA_ENABLE_IPV6      Default 0. Set to 1 with an IPv6-capable target stack.
  BA_ENABLE_UDP       Default 0. Set to 1 for the BAS datagram socket API.

Configure DNS in the VxWorks network stack before starting clients that
use hostnames. Use your SDK's resolver setup and the DNS servers for your
network. Numeric IP addresses do not require a DNS lookup.

Filesystem access
-----------------

Add xrc/DiskIo/posix/BaFile.c for the DiskIo filesystem adapter. Set its
root to an existing VxWorks file device and directory, such as
"/file-device/www". The exact device name comes from your BSP and mounts.

For Xedge, connect the mounted storage through xedgeInitDiskIo() as
described in the Xedge build guide. To include the original Lua standard
I/O library, set the Boolean option BA_HAVE_STDIO=1. The Lua I/O reference is:
  https://realtimelogic.com/ba/doc/en/lua/man/manual.html#6.8

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
