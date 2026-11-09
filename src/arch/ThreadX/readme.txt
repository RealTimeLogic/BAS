Barracuda Web Server and App Server on Eclipse ThreadX / NetX Duo
===============================================================

Build connected ThreadX products with a web interface, network services,
and application logic you can update independently of the firmware. The
Barracuda App Server (BAS) runs Lua and Lua Server Pages (LSP); Xedge adds
the browser-based workspace for developing and managing those applications.
Keep hardware drivers and time-sensitive work in C.

For a native C/C++ web application, use the Barracuda Web Server (BWS).
BAS adds the Lua runtime and application services needed by Xedge. Both
use the same ThreadX and NetX Duo port.

The port uses threads from the ThreadX real-time operating system (RTOS)
and native NetX Duo sockets, with NetX Duo 6 or newer. FileX storage is
optional for a general BAS integration.

This guide covers the ThreadX/NetX Duo port and the integration requirements
for your platform. For a ready-to-run Xedge example, see:
  https://github.com/RealTimeLogic/Xedge4ThreadX

The example project provides build instructions, hardware setup, and a
browser-to-Lua getting-started tutorial. See its documentation for supported
boards, development tools, and programming instructions.

Getting started
---------------

  1. Start with the Xedge4ThreadX example linked above, or use "Build the
     Barracuda library" to integrate with your own ThreadX project. Supply
     your platform's startup, network configuration, and storage services.
  2. Read "How startup works" to connect those services to Xedge.
  3. Add application behavior in Lua. Read "Debugger restarts and hook
     lifetime" when adding native bindings or workers.

All source paths in the instructions are relative to the full BAS SDK
root. "SDK and GitHub source layouts" lists the amalgamated package
equivalents. Keep the SDK paths when working in this source tree.

The network and storage sections provide the porting reference. For shared
Xedge resource-generation and integration steps, see:
  https://realtimelogic.com/ba/examples/xedge/readme.html
For the browser environment and Lua application model, see:
  https://realtimelogic.com/ba/doc/en/Xedge.html

Understanding the Xedge architecture
-----------------------------------

Xedge follows the same model across ThreadX, Xedge32, and other ports:
BAS supplies the application server and Lua runtime, Xedge supplies the
application environment and browser tools, and the board port connects
them to the operating system, network, and storage.

ThreadX schedules threads and supplies the kernel tick. NetX Duo owns the
IP interfaces, packets, DHCP address acquisition and DNS hostname lookup.
Your platform's drivers operate the hardware. FileX can provide writable
storage; LevelX can manage flash wear when required by the storage driver.

BAS uses these services through its ThreadX socket/thread port and optional
FileX DiskIo adapter. BAS owns the web server, Lua virtual machine (VM),
timers and Lua worker pool. Xedge's common startup is in
examples/xedge/src/xedge.c. Your platform startup connects the hardware and
RTOS services to that common code.

How startup works
-----------------

A typical Xedge integration follows this sequence:

  1. Initialize the hardware and start ThreadX using your platform's
     startup code and ThreadX port.

  2. Create the NetX interfaces, packet pools, required network services,
     and the thread that will run Xedge. Register the initialized network
     objects with BAS through SoDisp_nxInit before creating BAS sockets.

  3. In the application thread, prepare the BAS heap, clock, trace output,
     and cryptographic entropy. Open any required filesystem, establish
     network connectivity, and configure DNS.

  4. Call barracuda(). The common Xedge code creates BAS server objects and
     Lua, calls xedgeInitDiskIo() when using application-provided DiskIo
     initialization, and loads Xedge's embedded .config script.

  5. Xedge calls xedgeOpenAUX() when auxiliary initialization is enabled.
     This hook installs your native Lua bindings and other per-VM setup.
     Xedge then starts the configured applications and runs the socket
     dispatcher on the application thread.

The public hooks are called by their owning framework; your code supplies
their implementations. Do not call xedgeOpenAUX() from a ThreadX worker.
Its caller supplies XedgeOpenAUX* aux and holds the BAS server mutex, which
protects Lua and server state. Return 0 on success or -1 on setup failure;
Xedge treats a nonzero return as a fatal startup error.

For FileX, xedgeInitDiskIo(DiskIo* io) receives an already constructed
DiskIo object. Attach your opened FX_MEDIA object and set the filesystem
root. Return 0 on success or the adapter error code. Keep the media open
and valid while DiskIo uses it.

Integrating Xedge with your firmware
------------------------------------

Build the BAS library as described below, then compile
examples/xedge/src/xedge.c together with your platform startup code.
Provide one implementation of each application entry point, interrupt
handler, and Xedge hook; avoid duplicate startup definitions.

Your project needs:

  - ThreadX and NetX Duo libraries built for your processor and toolchain,
    plus the DNS, DHCP, and other add-ons required by your network setup.

  - Hardware startup, linker configuration, network drivers, a clock
    source, and a cryptographic entropy source. Make network buffers
    accessible to DMA and handle cache coherency as required by the driver.
    Initialize external memory before using it for heaps or static data.

  - Xedge's embedded Lua and web resources. Follow the standalone Xedge
    build guide linked above to generate them. Enable
    BA_ENABLE_EMBEDDED_CA=1 in both BAS and Xedge for the embedded
    certificate-authority bundle used by TLS clients.

  - XEDGE_ENABLE_AUX=1 when supplying xedgeOpenAUX(). For writable FileX
    storage, add xrc/DiskIo/FileX/BaFile.c and the matching DiskIo headers,
    FileX, and your storage driver. Use XEDGE_ENABLE_DISK_IO=1 and
    XEDGE_USE_DEFAULT_DISK_IO_INIT=0 when supplying xedgeInitDiskIo().
    Add LevelX if your flash storage driver requires it.

Use matching ThreadX and NetX tick rates and compile all consumers with
consistent tx_user.h, nx_user.h, and applicable storage-library settings.
Size stacks, heaps, packet pools, and storage for your application. Keep
writable files separate from firmware and embedded resources, and choose
an update and recovery policy that preserves saved applications and keys.
If firmware and writable storage share a flash device, follow the driver's
requirements for executing code during erase and write operations.

softTPM and SharkSSL
-------------------

Xedge's softTPM integration provides device identity and TLS private-key
operations without a separate TPM chip. Applications refer to named keys
through ba.tpm; softTPM derives the private keys when needed, keeping them
out of application files.

Before production, replace the example product secret and supply stable
per-device input through xedgeOpenAUX(). A device identifier is public,
not secret key material or a hardware-isolated key. Protect the secret and
firmware according to the softTPM integration guide. The Xedge host accepts
a generated secret header through XEDGE_SOFTTPM_KEY_HEADER; see the
standalone Xedge build guide. Changing the secret changes derived keys,
so plan migration of identities and encrypted data before replacing it on
a deployed device.

Seed native SharkSSL from your platform's cryptographic entropy source
before using TLS or other cryptographic services. See "Memory and BAS
initialization" below. For the softTPM security model, see:
  https://realtimelogic.com/ba/doc/en/SoftTPM.html

Debugger restarts and hook lifetime
-----------------------------------

When built with BA_ENABLE_LUA_DEBUGGER=1, Xedge can restart its Lua
environment at the debugger's request. barracuda() shuts down the old BAS
objects and Lua VM, then creates replacements within the same call. It
reloads .config and calls the enabled initialization hooks again.

The ThreadX kernel and your platform services continue running during this
restart. Put Lua bindings and other per-VM setup in xedgeOpenAUX(), so each
new VM gets its own registrations. Put one-time hardware setup and native
worker creation in platform startup. Keep network objects and any mounted
FileX volume alive across the restart.

Never keep lua_State pointers, Lua registry references, or old server and
manager pointers in native workers across a debugger restart. Dispatch
callbacks through BAS's Lua worker mechanism and associate them with the
current VM. A debugger termination without a restart request lets
barracuda() return; define how your application handles that return.

The standalone build guide documents the restart behavior under
BA_ENABLE_LUA_DEBUGGER:
  https://realtimelogic.com/ba/examples/xedge/readme.html#features
The debugger API describes connections and the restartonmod option:
  https://realtimelogic.com/ba/doc/en/lua/auxlua.html#dbgmon

Clock synchronization
---------------------

Provide a valid UTC clock for certificate generation and validation. Use
baSetUnixTime() to set or update the BAS clock from your platform's clock
source. If you acquire time over the network, run the blocking work in a
native worker so the web server can continue serving requests. Update the
FileX clock as well when using timestamped files.

A build-time fallback is not a synchronized clock. Keep synchronization
state in your platform code and notify Lua through BAS's supported callback
mechanism. Native ThreadX workers must not call Lua directly. For callback
patterns and VM lifetime requirements, see:
  https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_LuaBindings.html

Build the Barracuda library
---------------------------

The source lists below use the full BAS SDK layout. See "SDK and GitHub
source layouts" at the end for the amalgamated package equivalents.

Compile amalgamation/BWSCore.c for a native C/C++ web application, or
amalgamation/BASCore.c for Lua or Xedge. Add these ThreadX port files:

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
    Select it to suit your network throughput and memory budget.

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
or starting any other random-number consumer. Obtain the seed from your
platform's cryptographic entropy source and pass the complete buffer to
sharkssl_entropyEx(const U8* entropy, U16 len) in one call. entropy points to
the seed bytes; len is their count. Check its int return value: zero means
ready; nonzero is a startup failure. Then erase the temporary buffer with
sharkssl_clear. Do not split the initial seed into word-sized calls. BAS's
compatibility bootstrap does not replace your platform's entropy source
or discard its earlier contribution.

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
General Lua UDP multicast membership and dontroute are not implemented.
IPv4 broadcast is available through the Lua socket broadcast option.

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
Missing required settings fail at compile time. The mDNS options default
to off; enable them explicitly when your application needs local names or
SharkCA certificates.

SoDisp.c supplies the native transport; do not compile BamDNS-Sock.c from
the BSD socket port. SoDisp_nxInit enables IGMP. Automatic discovery selects
one active physical interface from the registered NX_IP instances, prefers
an IPv6 link-local address, and refreshes the address snapshot every five
seconds. IPv4 and IPv6 channels share UDP port 5353 on each NX_IP instance.
The driver must deliver receive processing in NetX's IP helper thread.
Each channel has a bounded receive queue.
Closing a responder releases its memberships and packets; the last channel
releases the shared socket.

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

SDK and GitHub source layouts
----------------------------

The full SDK and the GitHub package use different source layouts. The
instructions above use the SDK paths on the left. When working in the
amalgamated GitHub package, use the equivalents on the right:

  Full SDK                       GitHub package
  amalgamation/BASCore.c          src/BAS.c
  amalgamation/BWSCore.c          src/BWS.c
  src/arch/NET/             src/arch/NET/
  inc/arch/NET/                   inc/arch/NET/
  xrc/DiskIo/                     src/DiskIo/
  xrc/malloc/dlmalloc.c           src/dlmalloc.c

ThreadLib.c and the OS-specific header directories keep the same paths.
Both core files include zlib.

The SDK also provides amalgamation/BAS.c and amalgamation/BWS.c, which
include the default target ports. If you use one of those wrappers, do
not add a second copy of its port sources. See amalgamation/README.txt
and examples/xedge/readme.html in the SDK for its build instructions.
