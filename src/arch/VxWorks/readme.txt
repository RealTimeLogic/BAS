
-----------------------------  Getting Started

Barracuda App Server (BAS) for VxWorks

  * Building the BAS library
  * Examples
  * Memory allocation
  * Zlib compression
  * IPv6
  * File System
  * Using client libraries (such as HTTP) with DNS resolve


--------------------------------------------------------------------
------------------------  Building the BAS library:
--------------------------------------------------------------------

The following directories must be in your compiler's include path:
    inc
    inc/arch/VxWorks
    inc/arch/bsd/Posix
No other directories should be included.

Macros:
 Required:
   BA_VXWORKS
 Optional:
   LUA_NUMBER_INTEGER=0 - e.g. -DLUA_NUMBER_INTEGER=0
     Standard Lua supports both double precision floating point and 64
     bit integer; however, the default for the VxWorks BAS build is to
     use 32 bit integer only. To enable floating point and 64 bit
     integer, set LUA_NUMBER_INTEGER=0. Note: floating point requires
     that you create a dedicated task for the socket dispatcher SoDisp
     with the flag VX_FP_TASK -- i.e.
     taskCreate("SoDisp", prio, VX_FP_TASK,....
   USE_ADDRINFO -
     Use addrinfo for name translation and enable multi name
     DNS lookups that work with Round Robin DNS clusters.
   USE_DGRAM -
     Enable datagram (UDP sockets) in the auxiliary socket API.
   USE_IPV6 -
     Enable IPv6
   USE_DLMALLOC -
    Use the dlmalloc library and not the VxWorks heap functions (recommended).
    See below for details.

BAS library (one file): amalgamation/BAS.c 

We provide generic makefiles for compiling BAS. However, the makefiles
are not needed if you use the C file BAS.c, which includes
all other BAS C files using the "#include" directive. You can simply
include this C file in your build instead of using the makefiles (recommended).

Note: all code, when compiled with DIAB, must be compiled with:
       -Xchar-unsigned

-- Makefile information (if you are not using BAS.c)

The SIMNT directory contains the BA library Makefile and the target
definition includes file Target.mk. Copy the directory SIMNT and its
content to a new directory such as mytarget.

Edit mytarget/Target.mk and modify the settings such that it suits
your target environment.

Optionally edit the SetEnv.bat batch file if you would like to use the
build.bat batch file to build the libraries.

Build the BAS libraries for your target platform.

--------------------------------------------------------------------
-------------------  Examples (VxWorks integration test):
--------------------------------------------------------------------

Navigate to the examples directory. We suggest that you start with
examples/lspappmgr/ or examples/LspNetIo 

Include the example and BAS.c in your build. Read the
example readme file for details.

Call function barracuda from a dedicated thread, or from your main function.
Example:

extern void barracuda(void); /* Example program entry point */
int main()
{
   barracuda();
   return -1; // Should not return
}


-- Makefile information (if you are not using BAS.c)

The example makefiles require that you have two environment variables
set, BAROOT and tinc.

BAROOT must point to the root of the BA installation directory.
tinc must be: $BAROOT/ builds/VxWorks/mytarget /Target.mk, where
mytarget is your copied SIMNT directory.

The example makefiles are designed to support host like operating
systems and deep embedded systems. Most deep embedded systems must
support their own startup code. Typically an embedded system compiles
the example library files as follows: make lib. This produces one or
two libraries that must be included in your device together with the
BA libraries and startup code. All examples define the entry point
"barracuda" for the entry to the server example. This entry point is
called by platform specific startup code.

VxWorks is a cross between a deep embedded system and a host operating
system. You can make the examples work out of the box if you rename
the "main" startup entry to another name. You can then use the
complete make operation and perform the following make command: make
exe.


--------------------------------------------------------------------
------------------------  Memory allocation
--------------------------------------------------------------------

BA requires a memory allocator.

When using the LSP plugin, it is important to have a malloc
implementation with an efficient realloc function. The allocation
functions used by BAS, which are defined in TargConfig.h, can be
reconfigured to use dlmalloc. The default is to use the VxWorks
allocation routines.

To enable dlmalloc:

compile the code with the macro USE_DLMALLOC or modify
inc/arch/VxWorks/TargConfig.h

Include xrc/malloc/dlmalloc.c in your build.
Note: BAS.c includes dlmalloc.c if macro USE_DLMALLOC is defined.

From your startup code, initiate the allocator as follows:

extern void init_dlmalloc(char* heapstart, char* heapend);
static char poolBuf[600*1024]; /* Minimum 600K when using LSP */
init_dlmalloc(poolBuf, poolBuf + sizeof(poolBuf));



--------------------------------------------------------------------
----------------------- The Zlib compression library
--------------------------------------------------------------------

Barracuda requires the ZLIB inflate/deflate library. You can use the
standard zlib library or build your application with our modified
version: xrc/zip/BaZlib.c.

Note: BAS.c includes BaZlib.c
 

--------------------------------------------------------------------
------------------------ IPv6
--------------------------------------------------------------------

IPv6 is not enabled by default for VxWorks. To enable IPv6 support,
compile the code with the macro USE_IPV6. This macro is defined in
Target.mk for SIMNT


--------------------------------------------------------------------
----------------------- File System
--------------------------------------------------------------------

The DiskIo enables BAS to access files on any of your VxWorks file systems.

In additon, you may optionally enable the original Lua IO by compiling with:
-DBA_HAS_ANSI_IO
See: https://realtimelogic.com/ba/doc/en/lua/man/manual.html#6.8

The VxWorks DiskIo port xrc/DiskIo/posix/BaFile.c is automatically
included when compiling amalgamation/BAS.c

VxWorks provides a virtualized I/O system, where the base component of
a path typically includes the VxWorks device type. You must configure
the DiskIo and set the base path to a valid "file device". A base path
will typically include "/file-device/root-dir", where file-device is
a valid file system device and root-dir is an existing directory on
this file system. You may omit this directory if the root of the file
system is to be used as a base path.

Typical DiskIo configuration:

DiskIo_setRootDir(&vmIo,"/file-device"); /* access root of '/file-device' */
baLua_param.vmio = &vmIo;          /* The required IO */

Lua can now directly access the root of the "/file-device" file
system. You may then create a new I/O in Lua for you web files.

Lua Example:

-- The following code should be in the .config script

local vmio=ba.openio("vm") -- root is "/file-device". See C code.

-- Let's create a new I/O for one web-app, where the base is 'www'
if not vmio:stat"www" then vmio:mkdir"www" end
-- Create a new I/O from the Virtual Machine's I/O
local wwwio = ba.mkio(vmio, "www") -- base will be /file-device/www
assert(wwwio, "Cannot create 'www'")

-- We must have at least one directory in the virtual file system
-- Do not use 'local' in front of 'wwwapp'
-- The variable must be referenced (anchored)
wwwapp = ba.create.resrdr(wwwio) -- The web app uses '/file-device/www'
wwwapp:insert() -- Insert as root directory in the BAS virtual file system
-- http://address/index.html will now map to /file-device/www/index.html

-- Note, you also need to LSP enable this directory:
wwwapp:lspfilter(setmetatable({io=wwwio},{__index=_G}))
-- See the documentation for details on method 'lspfilter'

--------------------------------------------------------------------
--------- Using client libraries (such as HTTP) with DNS resolve
--------------------------------------------------------------------

You must initialize DNS as follows if you plan on using any TCP client
API with names that must be resolved by DNS. The function must be
called by your C startup code.

VxWorks 6.6 and newer: ipcom_sysvar_set("ipdnsc.primaryns", "8.8.8.8", 1)
Older VxWorks versions: resolvInit("8.8.8.8", "com", NULL);

