
Building Barracuda Embedded Web/Application Server for ThreadX/NetX

  * Memory allocation
  * Zlib compression
  * Optimizing NetX for Barracuda
  * Using FileX with Barracuda
  * Using the NetX Duo stack and IPv6
  * Enable DNS lookup
  * Multiple NetX Network Interfaces
  * Example startup code


-----------------------------  Prerequisite
View the Barracuda Web Server and Barracuda Application Server Setup
Tutorial video: http://youtu.be/1NqHB79cfpY


-----------------------------  Recommended Barracuda Example Build
* Read and edit demobuild.bat
* Read and edit setpath.bat
* Run demobuild.bat
* Add Barracuda startup code, BA libs, and demo libs in your IDE.
  See below for instructions.
* Load and run the binary on your target platform.
* Naviage, using a browser to http://target-ip-addr
* View the lspappmgr video tutorial:
  http://realtimelogic.com/downloads/video/#lspappmgr


-----------------------------  Memory allocation

The Barracuda Embedded Web Server (BA) requires a memory allocator.
When using the LSP plugin, it is important to have a malloc
implementation with an efficient realloc function. The allocation
functions used by Barracuda, which are defined in TargConfig.h, are
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


----------------------------- Using FileX with Barracuda

One can optionally add FileX support in Barracuda by compiling and
linking the following file with your firmware:
xrc\DiskIo\FileX\BaFile.c

Native file system support in Barracuda is called DiskIo, and all
examples can optionally be compiled with DiskIo integration. In
addition to the DiskIo startup code in the examples, the FileX DiskIo
port needs the following additional initialization:

    /* Cast DiskIo to the IoIntf super class. */
   IoIntfPtr io = (IoIntfPtr)myFileXDiskIo;
   io->propertyFp(io, "setmedia", (void*)&mptr, 0);

The above code uses the I/O property method to set the FileX media
pointer (mptr).

We suggest that you use the following code when initializing the FileX
version of the DiskIo:

void setFxMedia(DiskIo* dio, FX_MEDIA* mptr)
{ 
   IoIntfPtr io = (IoIntfPtr)dio;
   io->propertyFp(io, "setmedia", (void*)&mptr, 0);
}

File system support is not needed unless you plan on remotely managing
files in the device by, for example, using the WebDAV server or the Web
File Manager. We do not recommend storing web applications directly as
files using FileX. Instead use the NetIo for development and the ZipIo
for deployment. See the main documentation for more information.


----------------------------- Using the NetX Duo stack and IPv6

The Barracuda Embedded Web Server is NetX Duo ready and is
automatically listening on both IPv4 and IPv6 messages when used with
this new stack. Unlike traditional TCP/IP stacks, which require that
you create a HttpServCon object for IPv4 and one for IPv6, the NetX
duo stack automatically listens for both protocols by using one
HttpServCon object.

We suggest using the Firefox browser for testing since this browser is
IPv6 ready. As an example, connect to the loopback interface by
enter the following URL in the browser: http://[::1]/. You
can also edit the windows\system32\drivers\etc\hosts file and add your
own name mapping to an IPv6 address.

Please note that restarting the target, when using IPv6, also requires
that you restart the browser.

The NetIo, designed for development only, is not IPv6 ready.


-----------------------------  Enable DNS lookup

DNS lookup is not required by the server, but may be useful if using
any of the client stacks such as the HTTP(S) client library, the SMTP
library, and the Lua socket library. The client libraries cannot
resolve domain names unless the DNS is enabled. You do not need the
DNS plugin if you only use IP addresses and not domain names.

Enable DNS as follows:

* Compile the Barracuda source with macro BA_NX_DNS. 
* Call function SoDisp_setDNS(nxdns)

Variable nxdns is of type NX_DNS and must be properly initialized
prior to calling SoDisp_setDNS().

We recommend verifying that the NetX DNS plugin is properly configured
by calling function nxd_dns_host_by_name_get(...) after setting up the
DNS. The function should not return an error code.



-----------------------------  Multiple NetX Network Interfaces

If using multiple networks, each network must have its own NX_IP
instance. These networks must be listed in an array as follows:

       static NX_IP* ipList[] = {ip1, ip2,0};

This list is then registered with the BAS server as follows:

       SoDisp_nxInit(ipList, .......); /* See example below for details */

When creating an HttpServCon instance, the interface name (HttpServCon
argument) must correspond with the name of the NX_IP instance
(provided when you create the NX_IP instance) you want to bind to. You
must create an HttpServCon instance for each NX_IP interface if you
want to bind to all interfaces. You cannot create one HttpServCon and
set the name to NULL as specified in the documentation. This feature
is specific to BSD style network stacks. In a multihome configured
NetX stack, the name (HttpServCon argument) must match the name of the
NX_IP instance that the NetX interfaces are attached to.

----------------------------- Example startup code

#include <HttpServer.h>
#include <HttpTrace.h>

/* From xrc\malloc\dlmalloc.c.  */
extern void init_dlmalloc(char* heapstart, char* heapend);

/* Define thread prototypes.  */
void  barracudaThread(ULONG thread_input);


void tx_application_define(void *first_unused_memory)
{
   static TX_THREAD barracudaThreadContext;
   static char barracudaThreadStack[8*1024];
   static NX_IP* ipList[2]; /* The number of IP instances available to BA +1 */

    /* Initialize the allocator used by Barracuda.  */
   static char baPoolBuf[500*1024]; /* Minimum 500K when using LSP */
   /* IMPORTANT: Do this before any Barracuda calls */
   init_dlmalloc(baPoolBuf, baPoolBuf + sizeof(baPoolBuf));

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    /* Create an IP instance.  */
    /* Enable ARP and supply ARP cache memory for IP Instance.  */
    /* Enable TCP traffic.  */
    .........

    /* Init BA IP list i.e. all interfaces (NX_IP objects) that the
       server can use.

       void SoDisp_nxInit(NX_IP** ip, NX_PACKET_POOL* nxSendPacketPool,
                          AllocatorIntf* nxTcpSockAlloc, ULONG nxWindowSize);

        Use receive IP packet pool if nxSendPacketPool is 0. Not recommended.
        Use default allocator if nxTcpSockAlloc is 0.  This is OK.

       Initialize as:
       static NX_IP* ipList[] = {ip1,0};
       static char buf[sizeof(NX_TCP_SOCKET)*20];
       static FixedSizeAllocator allocator(
          buf, sizeof(buf), sizeof(NX_TCP_SOCKET));
       SoDisp_nxInit(ipList, myNxPacketSendPool, allocator, 10000);

       In the above example, a dedicated send pool and a fixed size
       allocator for allocating NX_TCP_SOCKET objects are used. If
       using a static allocator, the size must be larger than
       sizeof(NX_TCP_SOCKET)*N, where N is set with
       HttpServerConfig::setNoOfHttpConnections(N). The default for
       the number of HTTP connections is 16.

       In the code below, nxPacketPool and nxTcpSockAlloc are set to
       NULL, thus forcing BA to use the default allocator and use the
       receive IP packet pool for sending data. You can use the
       default allocator, but we recommend that you use a dedicated
       packet pool for sending data to minimize the risk of losing
       received packets.  
    */
    ipList[0] = myIpInstance;
    ipList[1] = 0; /* end of list */
    SoDisp_nxInit(ipList, 0, 0, 10000);

    /* Set the base time for the Barracuda clock.
     * See the documentation: 
         Manual -> C & C++ -> Examples -> Network Time
       for an example on how to write your own "getMyNtpTimeFunction"
       Doc: https://realtimelogic.com/ba/doc/?url=NetworkTime.html
     */ 
    baInitTxUnixTime(getMyNtpTimeFunction());

    /* Create the Barracuda thread.  */
    tx_thread_create(&bbarracudaThreadContext,"Barracuda",barracudaThread,0,  
                     barracudaThreadStack, sizeof(barracudaThreadStack),
                     2, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}


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
 

/* The Barracuda examples entry function. Link with one of
   the examples.
*/
extern void barracuda(void);

/* The Barracuda thread
*/
void barracudaThread(ULONG thread_input)
{
   HttpTrace_setFLushCallback(writeHttpTrace);
   HttpServer_setErrHnd(myErrHandler);
   barracuda(); /* Run the Barracuda example code. Function never returns. */
}
