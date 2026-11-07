Barracuda Web Server and App Server on Eclipse ThreadX / NetX Duo

Use this port to add a web interface, HTTP APIs and WebSockets to a ThreadX
device. Barracuda App Server (BAS) also runs Lua and Lua Server Pages (LSP).
Xedge builds on BAS with a browser-based development environment for these
applications. C code keeps control of the board and its drivers.

The port uses threads from the ThreadX real-time operating system (RTOS)
and native NetX Duo sockets, with NetX Duo 6 or newer. FileX storage is
optional for a general BAS integration.

This guide also describes the full SDK's MIMXRT1020-EVK board example,
which uses FileX and LevelX. Its XedgeStartup.c and board project are not
included in the GitHub source package. If you are integrating this
package into your own board project, start with "Build the Barracuda
library" and use the board sections as an explanation of the startup
sequence.

Start here
----------

  1. Read "How startup works" to see where ThreadX hands control to BAS.
  2. For the RT1020 example, follow "Build the Xedge board example" and
     its memory-layout requirements. For another board, use "Build the
     Barracuda library" and adapt the board services to your hardware.
  3. Read "Debugger restarts and hook lifetime" before adding Lua bindings
     or native workers. Those two kinds of code have different lifetimes.

The final sections are a reference for network options and FileX storage.
BAS source paths below are relative to the GitHub package root. Board-specific
files and settings refer to the full SDK example.
The general Xedge build guide explains resource generation and integration:
  https://realtimelogic.com/ba/examples/xedge/readme.html

Which code owns each service
----------------------------

ThreadX schedules threads and supplies the kernel tick. NetX Duo owns the
IP interfaces, packets, DHCP address acquisition and DNS hostname lookup.
NXP drivers operate the board hardware. FileX provides the file system;
LevelX spreads flash writes across blocks to reduce wear.

BAS uses these services through its ThreadX socket/thread port and FileX
DiskIo adapter. BAS owns the web server, Lua virtual machine (VM), timers
and Lua worker pool. Xedge's common startup is in
examples/xedge/src/xedge.c; the board-specific startup is XedgeStartup.c
in the full SDK's ThreadX example directory.

The section headings in XedgeStartup.c mark board support, BAS/Xedge hooks,
native workers, and the startup code that connects them.

How startup works
-----------------

The RT1020 example starts in this order:

  1. The board's C runtime calls main(). It configures the board and
     Ethernet hardware, then calls tx_kernel_enter() to start ThreadX.

  2. ThreadX calls _tx_initialize_low_level() to set up its Cortex-M7 tick,
     then tx_application_define() to create NetX services and the Xedge
     application thread. SoDisp_nxInit registers the network objects with
     BAS. Scheduled threads have not started yet.

  3. ThreadX runs application(). It prepares the BAS heap, initial clock,
     trace output and cryptographic entropy, mounts FileX, waits for DHCP,
     and configures DNS. It starts separate time and storage workers.

  4. application() calls barracuda(). The common Xedge code creates BAS
     server objects and Lua, calls xedgeInitDiskIo() to attach the opened
     FileX volume, and loads Xedge's embedded .config script.

  5. Xedge calls xedgeOpenAUX() to install the board's Lua-side additions,
     then starts the configured applications and runs the socket dispatcher
     on the application thread. The native workers continue independently.

The public hooks are called by their owning framework; your code supplies
their implementations. Do not call xedgeOpenAUX() from a ThreadX worker.
Its caller supplies XedgeOpenAUX* aux and holds the BAS server mutex, which
protects Lua and server state. A zero return means success; this example
returns -1 on setup failure and Xedge treats that as a fatal startup error.

xedgeInitDiskIo(DiskIo* io) receives an already constructed DiskIo object.
It attaches the board-owned FX_MEDIA object and sets the root to "/".
Return 0 on success or the adapter error code. The media must already be
open and remain valid while DiskIo uses it.

Build the Xedge board example
-----------------------------

XedgeStartup.c contains the application startup for the MIMXRT1020-EVK:
Cortex-M7 ThreadX tick setup, Ethernet and DHCP, IPv6, DNS, entropy, BAS heap,
FileX/LevelX mounting, Xedge startup and initial time acquisition using
Simple Network Time Protocol (SNTP).
Compile this file once, together with examples/xedge/src/xedge.c. It supplies
main, tx_application_define, _tx_initialize_low_level, SysTick_Handler,
xedgeOpenAUX and xedgeInitDiskIo; omit duplicate definitions from your
project. This file supplies the board integration; your project must
also provide the libraries, drivers, resources and linker layout below.

Required libraries, drivers and BAS settings:

  - RTOS middleware: ThreadX Cortex-M7 GNU port, NetX Duo with DNS and DHCP,
    FileX and LevelX.
    The example does not require the NetX SNTP add-on.

  - Board support: MCUXpresso SDK 26.09 RT1020 board, clock, pin and boot
    Device Configuration Data (DCD) files; Arm CMSIS support;
    GPIO, UART/debug console, ENET/KSZ8081 PHY, TRNG, FlexSPI and mflash.
    The pin configuration is from the RT1020 ENET transfer example.

  - Driver adapters: the platform's nx_link_driver and NetX interrupt
    adapter, plus the legacy NXP FileX/LevelX mflash example's
    fx_levelx_driver.c/.h and LevelX flash adapter. The NetX and FileX/LevelX
    adapters remain from NXP's 2.16 integration; SDK 26.09 supplies the
    peripheral drivers.
    Ethernet buffers must be DMA-accessible and cache-coherent;
    flash-programming code and constants must execute from RAM.

  - BAS/Xedge: BAS with the ThreadX port, xrc/DiskIo/FileX/BaFile.c,
    the FileX DiskIo headers, Xedge's embedded resources and key material.
    Enable XEDGE_ENABLE_AUX=1, XEDGE_ENABLE_DISK_IO=1 and
    XEDGE_USE_DEFAULT_DISK_IO_INIT=0. Standalone Xedge also requires
    BA_ENABLE_EMBEDDED_CA=1 in both BAS and Xedge so TLS clients have the
    embedded certificate-authority bundle. This example uses software-based
    key derivation (softTPM, BA_ENABLE_SOFTTPM_KEY_MATERIAL=1) and seeds
    SharkSSL from NXP's hardware true random number generator (TRNG).

  - Network options: use the asynchronous socket and DNS settings in the
    reference sections below. Define NX_ENABLE_INTERFACE_CAPABILITY for
    NXP ENET checksum offload. Define
    NX_DHCP_CLIENT_USER_CREATE_PACKET_POOL and
    NX_DNS_CLIENT_USER_CREATE_PACKET_POOL. Both clients use the noncached
    network pool. Compile fsl_phyksz8081.c with
    FSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE for the EVK reference clock.

  - Timing and stacks: configure matching ThreadX/NetX tick rates and
    compile all consumers with the same tx_user.h, nx_user.h, fx_user.h
    and lx_user.h settings. The offset-addressed NXP flash adapter requires
    LX_DIRECT_READ_DISABLED.
    The example profile uses TX_TIMER_TICKS_PER_SECOND=100,
    TX_TIMER_THREAD_STACK_SIZE=4096, TX_ENABLE_STACK_CHECKING and
    BA_THREAD_STACK_SIZE=65536.

  - Initial clock: define RT1020_BUILD_EPOCH as an unsigned integer giving
    UTC Unix seconds, supplied by the build system. This fallback lets
    Xedge create its initial certificate; it does not signal successful
    synchronization.

Compiler and boot settings
~~~~~~~~~~~~~~~~~~~~~~~~~~

The firmware executes from external flash (execute in place, or XIP).
Use the SDK's RT1020 GNU XIP build settings, including CPU_MIMXRT1021DAG5A,
XIP_EXTERNAL_FLASH=1, XIP_BOOT_HEADER_ENABLE=1,
XIP_BOOT_HEADER_DCD_ENABLE=1, SKIP_SYSCLK_INIT,
__STARTUP_INITIALIZE_NONCACHEDATA and __STARTUP_CLEAR_BSS. Enable the SDK's
UART debug console and LX_USE_MFLASH. Use Cortex-M7/Thumb with the FPv5-D16
hard-float ABI consistently across the application and libraries.

Memory and flash layout
~~~~~~~~~~~~~~~~~~~~~~~

The linker and memory protection unit (MPU) setup must match these regions:

  Start address   Size     Use
  0x60000000       4 MiB   Firmware in external flash
  0x60400000       4 MiB   FileX data in external flash
  0x80000000      30 MiB   Cacheable SDRAM, including the 24 MiB BAS heap
  0x81e00000       2 MiB   Noncacheable SDRAM for Ethernet DMA buffers

Initialize SDRAM using the board's boot DCD before clearing the BSS data
section, and preserve the SDRAM clock during startup. Flash-writing code
must run from the processor's data tightly coupled memory (DTCM).
An unmountable data partition is erased and formatted by this evaluation
startup. Choose your application's recovery policy before reusing it.

Debugger restarts and hook lifetime
-----------------------------------

When built with BA_ENABLE_LUA_DEBUGGER=1, Xedge can restart its Lua
environment at the debugger's request. barracuda() shuts down the old BAS
objects and Lua VM, then creates replacements within the same call. It
calls xedgeInitDiskIo(), reloads .config and calls xedgeOpenAUX() again.

The board keeps running during this restart. main(), tx_application_define()
and application() do not run again. NetX interfaces, the mounted FileX
volume, the storage worker and the time-synchronization flag remain alive.

Put Lua bindings and other per-VM setup in xedgeOpenAUX(), so every new VM
gets its own registrations. Put one-time hardware setup and native worker
creation in the board startup. Never keep lua_State pointers, Lua registry
references or old server/manager pointers in a native worker across a
debugger restart.

The time notification illustrates this separation: xedgeOpenAUX() creates
a new Lua-owned timer for every VM. If SNTP has already succeeded, the new
timer sees the existing flag and queues an event for the new applications.
The native time worker does not need to run again.

A debugger termination without a restart request lets barracuda() return.
This board example treats that return as a stop condition and halts.

The standalone build guide documents the restart behavior under
BA_ENABLE_LUA_DEBUGGER:
  https://realtimelogic.com/ba/examples/xedge/readme.html#features
The debugger API describes connections and the restartonmod option:
  https://realtimelogic.com/ba/doc/en/lua/auxlua.html#dbgmon

Internet time: from a ThreadX worker to Lua
-------------------------------------------

The board uses Simple Network Time Protocol (SNTP) to obtain its initial
UTC time without making the web server wait for the time server. Until a
reply arrives, RT1020_BUILD_EPOCH supplies an approximate build-time clock
so Xedge can create its initial certificate. That fallback never announces
successful synchronization.

There are three stages:

  1. synchronizeTime(), a native ThreadX worker, calls fetchTime(). On
     success it updates the BAS and FileX clocks and sets timeSynchronized.
     It then exits; BAS and FileX advance their clocks from that sample.

  2. A BAS Lua timer checks the flag once per second. xedgeOpenAUX() passes
     pollTimeEvent to Lua using lua_pushcfunction(), so ba.timer can call
     the C function just as it would call a Lua function. The timer belongs
     to the current VM and runs with the BAS mutex held.

  3. When the flag is set, pollTimeEvent queues a ThreadJob with LThreadMgr.
     A BAS worker calls timeEvent(), which invokes _XedgeEvent("sntp").
     Application event handlers therefore run on a worker, where blocking
     bindings do not occupy the timer thread. The native time worker never
     calls Lua or keeps a pointer to the current VM.

In timer:set(1000, true), 1000 is the interval in milliseconds and true
keeps the timer referenced after xedgeOpenAUX() returns. The callback's
return value controls repetition: Lua true retries; no return value stops
the timer. This also retries a failed ThreadJob allocation. Each VM queues
at most one time event, and shutdown can discard a pending job.

Xedge retains the delivered "sntp" event for later subscribers. An
application can register a function with xedge.event("sntp", callback) to
start time-dependent TLS clients; this event passes no extra arguments.
See the C-to-Lua callback patterns in:
  https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html

Time-server settings and retry behavior
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The native time worker resolves pool.ntp.org and sends the 48-byte SNTP
request over IPv4 UDP port 123. XEDGE_NTP_SERVER may override the hostname
with a C string; XEDGE_NTP_PORT may override the integer port (1 through
65535, default 123). IPv6 remains enabled for BAS network connections.

Each failed attempt closes its socket and sleeps for one second. Receiving
a reply has a five-second timeout, so a timeout followed by the retry delay
takes about six seconds. DNS has its own NetX retry behavior. These waits
run outside the BAS mutex and independently of FileX flushing and the UI.

The client checks the source address/port, packet length, server mode,
version, leap/stratum status and transmit timestamp. It accepts NTP v3/v4
replies and handles the 2036 era rollover within BAS's 32-bit Unix clock
range. Dates before 2020 and an all-zero timestamp are rejected. This small
client obtains an initial time sample; it does not implement periodic
resynchronization or authenticate the time server.

Use the serial log to distinguish startup from successful synchronization:

  CLOCK_BUILD_FALLBACK=...   Approximate clock; SNTP is still pending.
  SNTP_RETRY status=...     An attempt failed; the worker will retry.
  SNTP UTC=...              The native worker has set both clocks.
  SNTP_EVENT UTC=...        The BAS worker has called the Xedge event handler.

A ready web interface alone does not prove that SNTP succeeded.

Build the Barracuda library
---------------------------

The source lists below use the full BAS SDK layout. See "Full SDK source
layout" at the end of this guide for the GitHub package equivalents.

Compile amalgamation/BWSCore.c for a native C/C++ web application, or amalgamation/BASCore.c for
Lua or Xedge. Add these ThreadX port files:

  src/arch/ThreadX/ThreadLib.c
  src/arch/ThreadX/SoDisp.c

Add inc/arch/ThreadX and inc to the include path, together with the headers
for your ThreadX and NetX Duo ports. Compile the application, Barracuda and
NetX with the same network settings. Rebuild all objects after changing them:
NetX notification settings affect native socket layouts.

Put Barracuda options in your project's BaConfigOpts.h or shared compiler
definitions. Put NetX options in nx_user.h and define
NX_INCLUDE_USER_DEFINE_FILE consistently for NetX and its consumers.
Likewise, use TX_INCLUDE_USER_DEFINE_FILE when configuring ThreadX through
tx_user.h.

Network stack and driver requirements
-------------------------------------

Choose a NetX Duo stack and network driver that preserve the complete BAS
socket behavior required by your build. For full Xedge support, this covers
TCP clients and servers in blocking and asynchronous modes, Lua callbacks
and cosockets, TLS and WebSockets, and IPv4/IPv6 UDP, DNS and mDNS. It also
includes partial writes, timeouts, close/cancellation and recovery when
transmit queues or packet pools run out of space. A working web page or
small echo exchange does not establish this compatibility. Features and
known option limits are described in the reference sections below.

BAS blocking TCP sends and large UDP datagrams can span several NX_PACKET
buffers linked as a packet chain. Keep NX_DISABLE_PACKET_CHAIN undefined
for this full configuration, with the required IP fragmentation/reassembly
support. TCP segmentation and IP fragmentation are different from buffer
chaining: one network frame can itself occupy several buffers.

The supplied stack/driver combination must handle every packet chain that
can reach the driver. A driver that accepts only single-buffer frames is
compatible only if the stack guarantees that representation on all required
send paths, including packet-pool pressure. Size each buffer for the frame,
headers, alignment and DMA requirements. Increasing buffer counts alone
does not prove that a chained packet can never reach the driver.

Require compatibility with the unchanged BAS socket port and supplied
drivers. Disabling chaining, omitting blocking sockets or reducing datagram
coverage to accommodate a driver does not satisfy full socket support.

Memory and BAS initialization
-----------------------------

The ThreadX port defaults to dlmalloc. Compile xrc/malloc/dlmalloc.c once,
alongside the amalgamated library, and initialize its heap before any
Barracuda allocations. Size the heap, thread stacks, TCP windows and
packet pools for your application, including concurrent TLS connections.

Initialize ThreadX, NetX Duo, the Ethernet driver, packet pools and NX_IP
instances before entering the Barracuda application. Configure their network
addresses and enable the required IP services. Provide a valid UTC clock
and the target's cryptographic entropy source when using TLS.

Call SoDisp_nxInit once before constructing Barracuda sockets:

  void SoDisp_nxInit(NX_IP** ipList, NX_PACKET_POOL* sendPool,
                    ULONG tcpWindowBytes);

  ipList (NX_IP**, required): NULL-terminated array of pointers to
    initialized NX_IP instances. Keep the array and instances alive.
  sendPool (NX_PACKET_POOL*, optional): Dedicated send packet pool,
    recommended to preserve receive capacity. NULL uses each IP instance's
    default pool. Keep the selected pool alive while BAS uses it.
  tcpWindowBytes (ULONG, required): TCP receive-window size in bytes.
    The RT1020 example uses 65535.

Keep the IP list, IP instances, selected pools and Barracuda heap alive for
the server's lifetime. Allow heap space for listeners, client sockets and
sockets awaiting deferred cleanup. No socket-specific allocator is required.

The following helper runs from your Barracuda ThreadX thread. Its arguments
refer to application-owned memory and already initialized network objects.
heap is a required writable char* buffer of heapBytes (size_t) bytes;
utcSeconds is a required U32 UTC value in seconds since 1970-01-01.
The network arguments have the types and lifetime rules listed above.
barracuda() is the entry point supplied by your Barracuda application/example.
This helper returns only if barracuda() returns; it has no return value.

  #include <SoDisp.h>
  #include <dlmalloc.h>

  extern void barracuda(void);

  void startBarracuda(char* heap, size_t heapBytes, NX_IP** ipList,
                     NX_PACKET_POOL* sendPool, ULONG tcpWindowBytes,
                     U32 utcSeconds)
  {
     /* The heap and network objects must outlive the server. */
     init_dlmalloc(heap, heap + heapBytes);
     SoDisp_nxInit(ipList, sendPool, tcpWindowBytes);
     baSetUnixTime(utcSeconds);
     barracuda();
  }

Use baSetUnixTime with seconds since the Unix epoch to set or update the
clock. Register your platform's fatal-error handler and trace output before
entering the server.

For native SharkSSL, seed the random generator before entering barracuda()
or starting any other random-number consumer. The RT1020 example obtains
48 bytes from the NXP hardware TRNG and passes the complete buffer to
sharkssl_entropyEx(const U8* entropy, U16 len) in one call. Check its int
return value: zero means ready; nonzero is a startup failure. Then erase
the temporary buffer with sharkssl_clear. Do not split the initial seed
into word-sized calls. BAS's compatibility bootstrap does not replace a
hardware entropy source or discard the platform's earlier contribution.

Lua obtains current time from baGetUnixTime(), which returns BaTime. With
BA_USE_C_RUNTIME_TIME=1, Lua retains C-runtime calendar conversion and
os.date, but does not require a C time() implementation. The ThreadX stored
clock and baSetUnixTime argument remain U32 seconds, with a 2106 limit.

IPv4 and IPv6
-------------

BA_ENABLE_IPV6 follows the NetX Duo configuration unless explicitly set:

  NX_DISABLE_IPV6 absent:   BA_ENABLE_IPV6 defaults to 1.
  NX_DISABLE_IPV6 defined:  BA_ENABLE_IPV6 defaults to 0.

You can explicitly set BA_ENABLE_IPV6=0 to omit BAS IPv6 support. Setting it
to 1 requires IPv6 in NetX Duo; a conflicting configuration fails to compile.
A native NetX Duo listener accepts both families on one port, so use one
Barracuda listener per NX_IP instance and port.

NetX feature switches are presence-tested. Defining NX_DISABLE_IPV6=0 still
disables IPv6. Remove that definition to enable it.

Asynchronous sockets and thread priorities
------------------------------------------

Asynchronous sockets let BAS wait for network readiness without tying up a
worker thread. Lua cosockets use this mode. BA_HAVE_SOCKET_WRITE_EVENTS is
an integer switch that defaults to 1 and is required for Lua cosockets.
Enable both of these NetX options:

  #define NX_ENABLE_EXTENDED_NOTIFY_SUPPORT
  #define NX_ENABLE_TCP_QUEUE_DEPTH_UPDATE_NOTIFY

The port reports a compile-time error naming either missing option. These
options enable native connection and transmit-queue notifications. They must
be present when building NetX itself as well as Barracuda and the application.
Their presence enables them, even if a value of zero is supplied.

This supports asynchronous connect and partial writes, including Lua
cosockets. Waiting writers retry once per ThreadX tick to detect window and
packet-pool recovery; the dispatcher does not poll when no writers are waiting.
DNS lookup remains a synchronous operation with a finite wait per attempt.

Set BA_HAVE_SOCKET_WRITE_EVENTS=0 to select the blocking send/connect path.
The notification options are then optional. Receive events remain available.

Normal TCP close returns without waiting for the peer. A native cleanup thread
retains the socket until queued data and FIN are acknowledged, or a five-second
deadline expires. Explicit hard close and cancellation of a blocked operation
remain immediate. Allow heap space for sockets awaiting cleanup;
a client's explicitly selected local port remains reserved until cleanup.

Each dispatcher includes a 4 KiB cleanup-thread stack. The cleanup thread runs
at ThreadX priority 3 and never calls Lua. Keep the dispatcher and its mutex
alive for the server's lifetime.

Run the application thread that enters SoDisp_run at ThreadX priority 17,
below the normal BAS timer and Lua worker priority of 16. Lower ThreadX
priority numbers run first. A higher-priority dispatcher can starve timers
during continuous traffic and cause premature read timeouts when the timer
thread catches up. Keep unrelated high-priority tasks bounded so they cannot
starve BAS timers either.

TCP keepalive
-------------

Define NX_ENABLE_TCP_KEEPALIVE consistently in NetX and its consumers to enable
the Lua socket keepalive option. The default idle and probe interval values
come from NX_TCP_KEEPALIVE_INITIAL and NX_TCP_KEEPALIVE_RETRY, in seconds.
Explicit per-socket values must be positive integers no greater than 2147483
seconds. NX_TCP_KEEPALIVE_RETRIES controls the retry count.

The dispatcher checks enabled keepalive timers at intervals of at most 100 ms;
ThreadX scheduling can add delay. Disabling keepalive removes that socket from
timer polling. Without NX_ENABLE_TCP_KEEPALIVE the option is unavailable.

UDP and DNS
-----------

Set BA_ENABLE_UDP=1 to enable Barracuda's native datagram adapter and the Lua
UDP API. Its default is 0. TCP and UDP socket state use the Barracuda heap
through baMalloc and baFree. NetX packets use the configured packet pools.
General Lua UDP multicast membership and dontroute are not implemented. IPv4 broadcast is
available through the Lua socket broadcast option.

For hostname resolution, define BA_NX_DNS, compile the NetX Duo DNS add-on
and include nxd_dns.h in your initialization code. Enable NetX UDP, create
an NX_DNS instance, configure its servers, and call SoDisp_setDNS(&dns)
before starting Barracuda clients. Perform DNS initialization from a ThreadX
thread. The NX_DNS object must remain valid while Barracuda uses it.

BA_NX_DNS is also presence-tested: omit it to disable the BAS DNS
adapter; however, several services in Xedge require DNS.

DNS does not require BA_ENABLE_UDP=1; it uses NetX's own UDP service. Numeric
IP addresses do not require DNS. NetX retry settings determine the total
lookup wait.

mDNS and SharkCA local hostnames
-------------------------------

Set BA_ENABLE_MDNS=1 and BA_ENABLE_UDP=1 for the native NetX mDNS responder.
Set BA_ENABLE_MDNS_LUA=1 to expose ba.createmdns to Lua and enable Xedge's
SharkCA option. With IPv6 enabled, also define NX_ENABLE_IPV6_MULTICAST in
nx_user.h and rebuild NetX and the application with the same configuration.
Missing required settings fail at compile time. General port defaults remain
off; the RT1020 Xedge example enables both mDNS macros.

SoDisp.c supplies the native transport; do not compile BamDNS-Sock.c from
the BSD socket port. SoDisp_nxInit enables IGMP. Automatic discovery selects
one active physical interface from the registered NX_IP instances, prefers
an IPv6 link-local address, and refreshes the address snapshot every five
seconds. IPv4 and IPv6 channels share UDP port 5353 on each NX_IP instance.
The driver must deliver receive processing in NetX's IP helper thread, as
the RT1020 deferred driver does. Each channel has a bounded receive queue.
Closing a responder releases its
memberships and packets; the last channel releases the shared socket.

The responder announces the selected host label under .local, with A and
AAAA records when available. Choose a unique name; this minimal responder
does not probe for name conflicts. SharkCA issues private certificates, so
connecting computers must trust the intended portal's private root CA.
Enabling the dialog is not certificate enrollment: configure your SharkCA
portal and credentials before requesting a certificate.

Selecting a network interface
-----------------------------

An interface name passed to a Barracuda listener or socket selects an NX_IP
instance by its configured name. Register the instances in ipList; omission
selects the first instance. Create a listener for each NX_IP instance you
want to serve. Native binding is to the instance rather than a particular
local address.

Web resources and optional FileX storage
----------------------------------------

Use ZipIo for embedded web resources. The BAS/BWS amalgamation includes
zlib; do not compile another copy.

For writable FileX storage, include xrc/DiskIo/FileX/BaFile.c and the matching
DiskIo headers. After constructing DiskIo, attach an opened FX_MEDIA* with
its setmedia property. For DiskIo* disk and FX_MEDIA* media:

  /* Keep the opened FileX media alive while DiskIo uses it. */
  IoIntf* io = (IoIntf*)disk;
  int status = io->propertyFp(io, "setmedia", media, NULL);

Check that status is zero before using DiskIo. Initialize and manage the
FileX media in your platform code before serving files.

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
