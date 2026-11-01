/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *
 *   $Id: xedge.c 6657 2026-10-28 18:19:39Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2008 - 2026
 *               http://www.realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************

 The Xedge C code demonstrates the typical startup procedures required
 for the Barracuda App Server when deployed in an RTOS/firmware
 environment. 

 Xedge Documentation:
   How to use the default IDE:
      https://realtimelogic.com/ba/doc/en/Xedge.html
   How to compile Xedge, including custom build info:
      https://realtimelogic.com/ba/examples/xedge/readme.html

 In most production cases, this C code can be used "as is". Your changes
 if any would be in xedge.zip
 (see https://github.com/RealTimeLogic/BAS-Resources/tree/main/build).

 Your own Lua bindings should go into a file similar to the led.c example file.

 Note: When using the full Barracuda App Server (BAS) SDK, The Xedge
 platform integrates files and resources from the auxiliary API (from
 the xrc directory). The Makefile handles the inclusion of these files
 during the build process. Additionally, the Makefile assembles
 various resource files, such as Lua scripts, HTML files, and others,
 into a ZIP file. The optional BAS aux APIs included are documented in
 the startup code below.

 This C code includes support for a graceful shutdown, which external
 code can initiate by calling setDispExit(). Graceful shutdown is also
 used when Xedge is restarted by the debugger; however, when
 restarted, the program does not exit.

 The C code below also installs a Lua debugger hook, indirectly when
 compiled with BA_ENABLE_LUA_DEBUGGER=1, or directly when compiled with the macro
 XEDGE_ENABLE_LUA_LINE_TRACE. When XEDGE_ENABLE_LUA_LINE_TRACE is defined, the code
 demonstrates how to print each Lua line executed. Refer to the lHook
 function below for details. For instructions on using the debugger,
 see the following link:
 https://makoserver.net/articles/Lua-and-LSP-Debugging

 Note: Some platforms automatically define BA_ENABLE_LUA_DEBUGGER=1 if the macro
 is not explicitly set. Check inc/arch/<PLAT>/TargConfig.h for your
 specific platform.
*/

#include "xedge.h"

/* Apply this definition to BAS as well as the standalone Xedge host. */
#if !BA_ENABLE_EMBEDDED_CA
#error Standalone Xedge requires BA_ENABLE_EMBEDDED_CA=1 when compiling Xedge and BAS. Alternatively, add .certificate/cacert.shark to Xedge.zip.
#endif

#if BA_ENABLE_MDNS && BA_ENABLE_MDNS_LUA
#include <BamDNS.h>
#endif

#if SHARKSSL_PSA_CONSUMER
void balua_psa(lua_State *L);
#endif

/* Special case when working on Xedge Lua code on a host computer */
#if !XEDGE_ENABLE_DISK_IO && defined(BAIO_DISK)
#include <BaDiskIo.h>
#endif


/* A secret key used by the Xedge Lua code
   See EncryptionKey.h.
 */
#if BA_ENABLE_SOFTTPM_KEY_MATERIAL
#include XEDGE_SOFTTPM_KEY_HEADER
#endif

#if BA_ENABLE_ZIP_PASSWORD
#include "ZipBinPwd.h"
#endif

#if BA_ENABLE_ZIP_SIGNATURE
#include "ZipPublicKey.h"
#endif

/* Call this function from the program's main() function or from a
 * dedicated thread.
 */
void barracuda(void);


/* The server's socket dispatcher object.
 */
static SoDisp dispatcher;

/* We keep the Thread Manager global so it can be accessed as:
   extern LThreadMgr ltMgr;
*/
LThreadMgr ltMgr;


/* External code can call this function for graceful shutdown
 */
static BaBool shutDown=FALSE;
void setDispExit(void)
{
   shutDown=TRUE;
   SoDisp_setExit(&dispatcher);
}


/* 
   Makefile option: io=ezip
*/
#if !defined(BAIO_DISK) && !defined(BAIO_ZIP)
/* Default is: BAIO_EZIP */
extern ZipReader* getLspZipReader(void);
#endif

#if BA_ENABLE_LUA_LPEG
/* Parsing Expression Grammars For Lua
   https://www.inf.puc-rio.br/~roberto/lpeg/
*/
extern int luaopen_lpeg(lua_State *L);
#endif
#if BA_ENABLE_LUA_PROTOBUF
/* Google's protobuf for Lua
   https://github.com/starwing/lua-protobuf
*/
extern int luaopen_pb(lua_State* L);
#endif
#if BA_ENABLE_LUA_CBOR
/*  The Concise Binary Object Represenation Lua Modules
    https://github.com/spc476/CBOR
    CBOR requires lpeg
*/
int luaopen_org_conman_cbor_c(lua_State *L);
#if BA_ENABLE_LUA_LPEG != 1
#error CBOR requires LPeg
#endif
#endif
#if BA_ENABLE_LUA_OPCUA
#include <opcua_module.h>
#endif


#ifdef NDEBUG
#define xpcall(L,nargs) lua_pcall(L,nargs,0,0)
#else
static void
xpcall(lua_State *L, int nargs)
{
   if(lua_pcall(L, nargs, 1, 0))
   {
      HttpTrace_printf(0,"Error in 'startServer': %s\n",
                       lua_isstring(L,-1) ? lua_tostring(L, -1) : "?");
   }
   lua_pop(L,1);
}
#endif

/* This function runs the Lua 'onunload' handlers for all loaded apps
   when the server exits.  onunload is an optional function Lua
   applications loaded by the server can use if the applications
   require graceful termination such as sending a socket close message
   to peers.
 */
static void
onunload(lua_State* L, int onunloadRef)
{
   /* Run the 'onunload' function in the .config Lua script, which in
    * turn runs the optional onunload for all loaded apps.
    */
   lua_rawgeti(L, LUA_REGISTRYINDEX, onunloadRef);
   baAssert(lua_isfunction(L, -1));
   xpcall(L,0);
   luaL_unref(L, LUA_REGISTRYINDEX, onunloadRef);
   balua_relsocket(L); /* Gracefully close all cosockets, if any */
}


/* RTOS devices typically require special file system initialization.
 * This variable can be set to a function by platform specific startup
 * code. (REF-1).
 */
#if XEDGE_ENABLE_DISK_IO

/* File system init code for HLOS */
#if XEDGE_USE_DEFAULT_DISK_IO_INIT
#include <stdlib.h>
int xedgeInitDiskIo(DiskIo* dio)
{
   static const char appmgr[] =
#if SHARKSSL_PSA_CONSUMER
      {"xedge-psa"};
#else
      {"xedge"};
#endif

   int retVal;
   char* buf;
   IoStat st;
   IoIntf* io = (IoIntf*)dio;
   const char* home = getenv("HOME");
   if( ! home )
      home = getenv("USERPROFILE");
   if( ! home )
   {
      HttpTrace_printf(0, "'HOME' environment variable not set\n");
      return -1;
   }
   DiskIo_setRootDir(dio,home);
   if(io->statFp(io,appmgr,&st) && io->mkDirFp(io,appmgr,0))
   {
      HttpTrace_printf(0, "Cannot create %s/%s\n",home,appmgr);
      return -1;
   }
   buf=baMalloc(strlen(home)+sizeof(appmgr)+1);
   if(!buf) return -1;
   basprintf(buf,"%s/%s",home,appmgr);
   HttpTrace_printf(0, "Xedge root directory: %s\n", buf);
   retVal=DiskIo_setRootDir(dio,buf);
   baFree(buf);
   return retVal;
}
#endif
/* End file system init code */

#endif /* XEDGE_ENABLE_DISK_IO */


/* SharkTrustX

   Create your own tokengen.h header file as follows:

   1: Sign up for Real Time Logic's DNS testing service or preferably,
      set up your own DNS service (one of:)
        A: Testing service: https://acme.realtimelogic.com/
        B: Your own service: https://github.com/RealTimeLogic/SharkTrustX
   2: Wait for the email received from the DNS service
   3: Navigate to your DNS zone https://your-domain/login
   4: Login using your credentials
   5: Navigate to https://your-domain/cgen
   6: Download the generated C code, rename to tokengen.h,
      and place generated file in this directory
*/
#if !BA_ENABLE_SHARKTRUST
#define balua_tokengen(L)
#else
#define EMBEDDED_ZONE_KEY
#include "tokengen.c"
#endif


/* Initialize the HttpServer object by calling HttpServer_constructor.
   The HttpServerConfig object is only needed during initialization.
*/
static void
createServer(HttpServer* server)
{
   HttpServerConfig scfg;
   /* With XEDGE_THREAD_POOL_MODE=XEDGE_THREAD_POOL_SHARED_FIXED, the HttpCmdThreadPool (HTTP thread
    * pool) is not utilized. Instead, the LThreadMgr is used as the
    * thread pool. Additionally, the LThreadMgr is configured to prohibit
    * the creation of additional threads.
    * Details:
    *  https://realtimelogic.com/ba/doc/en/C/reference/html/group__ThreadMgr.html
    */
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SHARED_FIXED
   U16 cmdInstances=XEDGE_THREAD_COUNT;
   if(cmdInstances > 1)
      cmdInstances-=1;
#else
   /* (A) Configure for 3 threads. See HttpCmdThreadPool. */
   U16 cmdInstances=3;
#endif
   HttpServerConfig_constructor(&scfg);

   HttpServerConfig_setNoOfHttpCommands(&scfg,cmdInstances);

   HttpServerConfig_setNoOfHttpConnections(
      &scfg, cmdInstances+3 < 8 ? 8 : cmdInstances+3);

   /* For huge url encoded data, if any. */
   HttpServerConfig_setRequest(&scfg,2*1024, 8*1024);
   /* Large response buffers makes the NetIO much faster when used by
    * the web server.
    */
   HttpServerConfig_setResponseData(&scfg,8*1024);

   scfg.maxResponseHeader = 32 * 1024;

   /* Create and init the server, by using the above HttpServerConfig.
    */
   HttpServer_constructor(server, &dispatcher, &scfg);
}


/* Lua debugger hook. Use the debug monitor or the basic trace lib.
 */
#if BA_ENABLE_LUA_DEBUGGER
static void
dbgExitRequest(void* na, BaBool rstart)
{
   (void)na;
   if(rstart)
   {
      SoDisp_setExit(&dispatcher);
   }
   else
   {
      setDispExit(); /* Terminate without entering the restart loop. */
   }
}
#elif XEDGE_ENABLE_LUA_LINE_TRACE
/* Prints every line executed by Lua if enabled */
static void lHook(lua_State *L, lua_Debug *ar)
{
   lua_getinfo(L, "S", ar);
   printf("%4d\t%s\n",ar->currentline,ar->short_src);
}
#endif


/* Create and return the IO used by the Lua virtual machine.
   The IO can be configured with compile time options as follows:
   disk   |    net   |    zip   |   ezip
   BAIO_DISK | BAIO_NET | BAIO_ZIP | BAIO_EZIP
*/
static IoIntf*
createVmIo()
{
#if defined(BAIO_DISK)
   static DiskIo vmIo;
   int ecode;
   DiskIo_constructor(&vmIo);
   if( (ecode=DiskIo_setRootDir(&vmIo, XEDGE_RESOURCE_DIRECTORY)) != 0 )
   {
      HttpTrace_printf(0, "Cannot set DiskIo " XEDGE_RESOURCE_DIRECTORY " directory: % s\n",
                       baErr2Str(ecode));
      baFatalE(FE_USER_ERROR_1, __LINE__);
   }
#elif defined(BAIO_NET)
#error Cannot use the NetIo: Not implemented.
/* Disabled Makefile option: make io=net
   Think about the Xedge as a tool and the tool's
   web and Lua app as firmware. The lua code is required for the
   operation of the tool and should be embedded in the executable. The
   code is designed to be used "as is" as a development tool and you
   do not need to study or modify it (unless you want to know how the
   Lua code works).
*/
#else
   static ZipIo vmIo;
   ZipReader* zipReader;
#if defined(BAIO_ZIP)
   static FileZipReader fileZipReader;
   FileZipReader_constructor(&fileZipReader, "lsp.zip");
   zipReader = (ZipReader*)&fileZipReader;  /* cast to base class */
#else
   /* BAIO_EZIP: for Xedge release build. */
   zipReader = getLspZipReader();
#if BA_ENABLE_ZIP_SIGNATURE
   if(baCheckZipSignature(zipPubKey, zipReader->size, (CspReader*)zipReader))
      return 0; /* ZIP signature failed */
#endif
#endif
   if( ! CspReader_isValid((CspReader*)zipReader) )
      baFatalE(FE_USER_ERROR_1, __LINE__);
   ZipIo_constructor(&vmIo, zipReader, 2048, 0);
   if(ZipIo_getECode(&vmIo) !=  ZipErr_NoError)
      baFatalE(FE_USER_ERROR_1, __LINE__);
#endif
   return (IoIntf*)(&vmIo);
}

/*
  Push the embedded primary secret key material as a Lua string
*/
#if BA_ENABLE_SOFTTPM_KEY_MATERIAL
static void pushPrimarySecret(lua_State* L)
{
#include "XedgeWBC.h"
}


/* Add additional TPM secrets.
   This function simplifies calling the Lua function returned after
   .config has been executed (see balua_loadconfigExt() below). One Lua
   value must be pushed on the Lua stack when this function is
   called. This value is sent as an argument to the Lua function. The
   Lua function accepts the following arguments:
   nil, (string,bollean), boolean (true).
   When called with nil, the Lua code immediately starts the
   server. If called with string values (binary), the Lua function
   collects the passed-in string values as long as the function is
   called. When the function is finally called with the argument
   'true', it calculates a device unique pre-master key based on the
   passed-in string values and starts the server.
   Note that the calculated non device specific (global) AES key
   (based on main secret in EncryptionKey.h and values set by you is
   used for encrypting/decrypting components of xedge.conf. this makes
   it possible to copy xedge.conf between devices.
 */
static void
addSecret(XedgeOpenAUX* aux, BaBool unique, const U8* secret, size_t slen)
{
   lua_State* L = aux->L;
   lua_rawgeti(L, LUA_REGISTRYINDEX, aux->initXedgeFuncRef);
   if(secret)
   {
      lua_pushlstring(L,(char*)secret,slen);
   }
   else
   {
      lua_pushvalue(L, -2);
      baAssert(LUA_TSTRING == lua_type(L, -1));
   }
   lua_pushboolean(L,unique);
   xpcall(L, 2);
   if(!secret)
   {
      lua_pop(L,1);
   }
}
#endif /* BA_ENABLE_SOFTTPM_KEY_MATERIAL */


/* Wrapper for calling function xedgeOpenAUX.
 * Two xedgeOpenAUX examples: led.c and AsynchLua.c.
 */
#if !XEDGE_ENABLE_AUX
#define callXedgeOpenAUX(L,ref,io) 0
#else
static void
callXedgeOpenAUX(lua_State* L, int initXedgeFuncRef, IoIntfPtr dio)
{
   XedgeOpenAUX aux = {
      L,
      dio,
#if !BA_ENABLE_SOFTTPM_KEY_MATERIAL
      0,0,
#else
      addSecret,initXedgeFuncRef,
#endif
      0
   };
#if BA_ENABLE_SOFTTPM_KEY_MATERIAL
   /* ENCRYPTIONKEY from (New)EncryptionKey.h
      Push the primary secret key material as a Lua string
   */
   pushPrimarySecret(L);
   addSecret(&aux, FALSE, 0, 0);
#endif
   if(xedgeOpenAUX(&aux))
      baFatalE(FE_USER_ERROR_1, __LINE__);
   if(aux.xedgeCfgFile)
   {
      lua_rawgeti(L, LUA_REGISTRYINDEX, initXedgeFuncRef);
      lua_pushcfunction(L, aux.xedgeCfgFile);
      /* Register the open/save cfg file */
      xpcall(L,1);
   }
}
#endif


/*
  This is the "main" barracuda function. This function initializes the
  xedge, configures a Lua Virtual Machine and enters a forever
  loop.

  This function is typically called from a thread/task dedicated to
  running the Barracuda App Server socket dispatcher SoDisp.

  Notice that we do not create a Virtual File System in this startup
  code. The Virtual File System is created by the Lua .config script
  in the "lsp" subdirectory. The .config script is executed when we
  call balua_loadconfig from the C code below.
*/
void
barracuda(void)
{
   /* static: less stack; we only have one server instance */
   static ThreadMutex mutex;
   static HttpServer server;
   static BaTimer timer;
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SEPARATE
   static HttpCmdThreadPool pool;
#endif
   static int ecode;
   static NetIo netIo;
   static lua_State* L; /* pointer to a Lua virtual machine */
   static BaLua_param blp; /* configuration parameters */
   static int onunloadRef;
   static int initXedgeFuncRef;
#if XEDGE_ENABLE_DISK_IO
   static DiskIo diskIo;
#endif

   HttpTrace_setPrio(9); /* block > 9 */

#if SHARKSSL_PSA_CONSUMER
   /* Provider storage and hardware must be ready before ZIP verification or
    * Lua creation uses cryptography. Keep them alive across Lua restarts.
    */
   {
      int status=xedgeInitPsa();
      if(status)
      {
         HttpTrace_printf(0,"PSA provider initialization failed: %d\n",status);
         baFatalE(FE_USER_ERROR_1, __LINE__);
         return;
      }
   }
#endif

/* If restarted by Lua debugger */
  L_restart:

   /* Create the Socket dispatcher (SoDisp), the SoDisp mutex, and the server.
    */
   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&dispatcher, &mutex);
   createServer(&server);

   /* For embedded systems when developing */
   NetIo_constructor(&netIo, &dispatcher);

   /* The optional timer is useful in LSP code */
   BaTimer_constructor(&timer, &mutex, BA_THREAD_STACK_SIZE, 25, ThreadPrioNormal, 0);
   
   /* Create a Lua (LSP) virtual machine.
    */
   memset(&blp, 0, sizeof(blp));
   blp.vmio = createVmIo();  /* The required IO */
   if (!blp.vmio)
      return; /* ZIP signature failed */
   blp.server = &server;     /* pointer to a HttpServer */
   blp.timer = &timer;       /* Pointer to a BaTimer */
#if BA_ENABLE_ZIP_PASSWORD
   /* Must be set in zipbinpwd.h */
   blp.zipBinPwd = zipBinPwd;
   blp.zipBinPwdLen = sizeof(zipBinPwd);
   blp.pwdRequired = BA_REQUIRE_ZIP_PASSWORD;
#endif
#if BA_ENABLE_ZIP_SIGNATURE
   /* Must be set in ZipPublicKey.h */
   blp.zipPubKey = zipPubKey;
#endif
   L = balua_create(&blp);   /* create the Lua state */

   /* Lua debugger hook.
      Use the debugger monitor or the basic trace lib (lHook).
      Must be first.
   */
#if BA_ENABLE_LUA_DEBUGGER
   ba_ldbgmon(L, dbgExitRequest, 0);
#elif XEDGE_ENABLE_LUA_LINE_TRACE
   lua_sethook(L, lHook, LUA_MASKLINE, 0);
#endif

   /* Install optional IO interfaces */
   balua_iointf(L, "net",  (IoIntf*)&netIo);
   balua_http(L); /* Install optional HTTP client library */
   LThreadMgr_constructor(&ltMgr,
                          &server,
                          ThreadPrioNormal,
                          BA_THREAD_STACK_SIZE,
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SHARED_FIXED
                          XEDGE_THREAD_COUNT,
#else
                          XEDGE_THREAD_COUNT, /* Initial number of Lua threads */
#endif
                          L,
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SHARED_FIXED
                          FALSE /* do not allow creating more threads */
#else
                          TRUE /* allow creating more threads */
#endif
      );
                          
   balua_socket(L);  /* Install optional Lua socket library */
   balua_sharkssl(L);  /* Install optional Lua SharkSSL library */
   balua_crypto(L);  /* Install optional crypto library */
   balua_tracelogger(L,&ltMgr); /* Install optional trace logger library */
   balua_luaio(L); /* xrc/lua/lio.c */

/* 
   Some embedded devices may not have a DiskIo port or a file system.
   The following macro makes it possible to disable the DiskIo.
   Makefile option: make ..... nodisk=1
*/
#if !XEDGE_ENABLE_DISK_IO
   HttpTrace_printf(0, "DiskIo not included. Use NetIo!\n");
#else
   DiskIo_constructor(&diskIo);
   /* REF-1 */
   if( xedgeInitDiskIo(&diskIo) == 0)
   {
      /* Add optional IO interfaces */
      balua_iointf(L, "disk",  (IoIntf*)&diskIo);
   }
   else
   {
      HttpTrace_printf(0, "Installing DiskIo failed!\n");
   }
#endif

   /* Create user/login tracker */
   balua_usertracker_create(
      L,
      20, /* Tracker node size. */
      4, /* Max number of login attempts. */
      10*60); /* 10 minutes ban time if more than 4 login attempts in a row. */

   balua_tokengen(L); /* See  the "SharkTrustX" comment above */
#if BA_ALLOCATOR == BA_ALLOCATOR_DLMALLOC
   balua_mallinfo(L); /* Mem info. See dlmalloc.c */
#endif
#if BA_ENABLE_UBJSON
   balua_ubjson(L);
#endif
#if BA_ENABLE_LUA_LPEG
   luaL_requiref(L, "lpeg", luaopen_lpeg, FALSE);
   lua_pop(L,1); /* Pop lpeg obj: statically loaded, not dynamically. */
#endif
#if BA_ENABLE_LUA_CBOR
   luaL_requiref(L, "org.conman.cbor_c", luaopen_org_conman_cbor_c, FALSE);
   lua_pop(L,1);
#endif
#if BA_ENABLE_LUA_PROTOBUF
   luaL_requiref(L, "pb", luaopen_pb, FALSE);
   lua_pop(L,1); /* Pop pb obj: statically loaded, not dynamically. */
#endif
#if BA_ENABLE_REVERSE_CONNECTION
   /* Add reverse server connection. This requires SharkTrustX.
    */
   balua_revcon(L);
#endif
#if BA_ENABLE_LUA_OPCUA
   luaopen_opcua_ns0_static(L);
#endif
#if BA_ENABLE_MDNS && BA_ENABLE_MDNS_LUA
   /* Install ba.createmdns for SharkCA and local hostname resolution. */
   BamDNS_luaopen(L, NULL);
#endif
#if SHARKSSL_PSA_CONSUMER
   /* Register the binding in each new Lua state, before .config runs. */
   balua_psa(L);
#endif
   /* Dispatcher mutex must be locked until the dispatcher starts
    */
   ThreadMutex_set(&mutex);
   ecode=balua_loadconfigExt(L, blp.vmio, 0, 2); /* Load and run .config  */
   if(ecode)
   {
      HttpTrace_printf(0,".config error: %s.\n", lua_tostring(L,-1)); 
      baFatalE(FE_USER_ERROR_1, __LINE__);
   }
   /* .config must return two functions, one for starting server, and
    * one onunload handler
    */
   if(!lua_isfunction(L, -1) || !lua_isfunction(L, -2))
   {
      HttpTrace_printf(0,".config error: no start server or onunload\n");
      baFatalE(FE_USER_ERROR_1, __LINE__);
   }
   /* Keep a reference to the two functions returned by .config */
   onunloadRef=luaL_ref(L,LUA_REGISTRYINDEX);
   initXedgeFuncRef=luaL_ref(L,LUA_REGISTRYINDEX);

   /* See (A) above */
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SHARED_FIXED
   LThreadMgr_enableHttpPool(&ltMgr, &server);
#else
   HttpCmdThreadPool_constructor(&pool, &server, ThreadPrioNormal, BA_THREAD_STACK_SIZE);
#endif

   /* Example Lua bindings, compile with AsynchLua.c or led.c.
      This code opens ESP32 bindings when Xedge32 is compiled.
    */
#if !XEDGE_ENABLE_DISK_IO
   callXedgeOpenAUX(L, initXedgeFuncRef, 0);
#else
   callXedgeOpenAUX(L, initXedgeFuncRef, (IoIntfPtr)&diskIo);
#endif

   /* Signal done, now start server */
   lua_rawgeti(L, LUA_REGISTRYINDEX, initXedgeFuncRef);
#if !XEDGE_ENABLE_AUX || !BA_ENABLE_SOFTTPM_KEY_MATERIAL
   lua_pushnil(L);
#else
   lua_pushboolean(L, TRUE);
#endif
   xpcall(L, 1);
   luaL_unref(L, LUA_REGISTRYINDEX, initXedgeFuncRef);

   /*
     The dispatcher object waits for incoming HTTP requests. These
     requests are sent to the HttpServer object, where they are delegated to
     a Barracuda resource such as the WebDAV instance.

     Note: the server socket connections are opened by the Lua script
     .config and not by C code.

     Arg -1: Never returns, unless SoDisp_setExit() is called
   */
   ThreadMutex_release(&mutex);
#ifdef xedgeWait4Network
   xedgeWait4Network();
#endif
   SoDisp_run(&dispatcher, -1);

   /* Gracefull shutdown
    */

   /*Dispatcher mutex must be locked when terminating the following objects.*/
   ThreadMutex_set(&mutex);
   /* Graceful termination of Lua apps. See function above. */
   onunload(L, onunloadRef);
   LThreadMgr_destructor(&ltMgr); /* Wait for threads to exit */
#if XEDGE_THREAD_POOL_MODE == XEDGE_THREAD_POOL_SEPARATE
   HttpCmdThreadPool_destructor(&pool);  /* Wait for threads to exit */
#endif

   /* Must cleanup all sessions before destroying the Lua VM */
   HttpServer_termAllSessions(&server);
   BaTimer_destructor(&timer);
   /* Destroy all objects, including server listening objects. */
   balua_close(L);

   IoIntf_destructor(blp.vmio); /* Virtual destr */
#if XEDGE_ENABLE_DISK_IO
   DiskIo_destructor(&diskIo);
#endif
   NetIo_destructor(&netIo);
   HttpServer_destructor(&server);
   SoDisp_destructor(&dispatcher);
   ThreadMutex_release(&mutex);   
   ThreadMutex_destructor(&mutex);

   if(shutDown) /* Explicit shutdown or debugger termination. */
   {
      shutDown=FALSE;
   }
   else /* via debugger */
   {
      HttpTrace_printf(0,"\n\nRestarting xedge.\n\n");
      HttpTrace_flush();
      goto L_restart;
   }
}
