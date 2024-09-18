/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: lxrc.h 5568 2024-09-14 14:51:10Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2023 - 2024
 *               https://realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 *
 *
 */
#ifndef __lxrc_h
#define __lxrc_h 

#ifndef NO_SHARKSSL
#include <SharkSSL.h>
#endif
#include <balua.h>
#include <SoDispCon.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup AuxLSP Auxiliary API
    @ingroup LSP
    The optional Lua code in xrc/lua, header file xrc/lua/lxrc.h 
    @{
*/


/** Install the [forkpty Lua bindings](../../../lua/auxlua.html#forkptylib).
 */
void balua_forkpty(lua_State* L);

/** Install the [httpc Lua bindings](../../../lua/auxlua.html#l#httpc).
 */
void balua_http(lua_State* L);

/** Install the [LuaIo Lua bindings](../../../lua/auxlua.html#luaio).
 */
void balua_luaio(lua_State *L);

/** Install the [SharkSSL Lua bindings](../../../lua/auxlua.html#sharkssl).
 */
void balua_sharkssl(lua_State *L);

/** Install the [socket Lua bindings](../../../lua/auxlua.html#socket).
 */
void balua_socket(lua_State* L);

/** When the socket Lua bindings are installed and the server
 * application implements a gracefully system exit, C code can call
 * this function.
 */
void balua_relsocket(lua_State* L);

/** Install the [crypto Lua bindings](../../../lua/auxlua.html#crypto).
 */
void balua_crypto(lua_State *L);

struct ThreadJob;
struct LThreadMgr;

/** Install the [TraceLogger Lua bindings](../../../lua/auxlua.html#tracelogger).
    \param L the state
    \param tmgr the optional LThreadMgr must be set if you plan on using trlogger:onclient(cb)
 */
void balua_tracelogger(lua_State *L, struct LThreadMgr* tmgr);

/** @defgroup ThreadMgr The Lua Thread Library
    See Advanced Lua Bindings, section \ref fullsolution "Calling Lua Code Asynchronously From C Code" for how to use this class.

    @ingroup AuxLSP
    @{
*/


/** Generic ThreadJob callback
    \param tj the job passed into function LThreadMgr_run
    \param mgr the manager associated with the global or dynamically
    created instance via
    [ba.thread.create](../../../lua/auxlua.html#thread_create)
 */
typedef void (*ThreadJob_Run)(struct ThreadJob* tj, struct LThreadMgr* mgr);

/** ThreadJob callback designed for calling Lua code using pcall
    \param tj the job passed into function LThreadMgr_run
    \param msgh the index position to the BAS error handler function
    \param mgr the manager associated with the global or dynamically
    created instance via
    [ba.thread.create](../../../lua/auxlua.html#thread_create)
 */
typedef void (*ThreadJob_LRun)(
   struct ThreadJob* tj, int msgh, struct LThreadMgr* mgr);

/** The global instance created by C code or a dynamic instance
 * created by
 * [ba.thread.create](../../../lua/auxlua.html#thread_create)
 */
typedef struct LThreadMgr
{
   HttpCmdThreadPoolIntf super; /* Inherits from HttpCmdThreadPoolIntf */
   DoubleList idleThreadList; 
   DoubleList runningThreadList;
   DoubleList pendingJobList; /* ThreadJob */
   HttpServer* server; /**< The server object */
   lua_State* Lg; /**< Global state */
   int runningThreads; /**< Threads currently running */
   int pendingJobs; /**< Jobs queued */
   int isDynamic; /**< if created via ba.thread.create() */
   ThreadPriority priority;
   int stackSize;
   int threads; /**< number of threads */
} LThreadMgr;


/** A thread job created by #ThreadJob_create or #ThreadJob_lcreate
 */
typedef struct ThreadJob
{
   DoubleLink super;  /* Inherits from DoubleLink */
   ThreadJob_Run run;
   ThreadJob_LRun lrun;
   lua_State* Lt; /**< Thread state */
} ThreadJob;

/** This function returns true if at least one thread is currently
 * idle and there is no need to queue the job when calling
 * #LThreadMgr_run.
 * \param o the LThreadMgr instance 
 */
#define LThreadMgr_canRun(o) ! DoubleList_isEmpty(&(o)->idleThreadList)

/** LThreadMgr can be used as a
 * [Thread Pool](../../../lua/lua.html#threadscoroutines)
 * and is often utilized in real-time operating system (RTOS) devices
 * with limited resources as a substitute for HttpCmdThreadPool.
 * \param o the LThreadMgr instance 
 * \param server the HttpServer instance 
 */
#define LThreadMgr_enableHttpPool(o, server)                            \
   HttpServer_setThreadPoolIntf(server, (HttpCmdThreadPoolIntf*)o)

/** This function terminates the Thread Pool and waits for all threads
 * to become idle before returning.
 */
BA_API void LThreadMgr_destructor(LThreadMgr* o);

/** Initialize the Thread Pool.
 * \param o
 * \param server
 * \param  priority typically set to ThreadPrioNormal
 * \param stackSize typically set to BA_STACKSZ
 * \param threads Specify the initial number of threads to create
 * \param L the Lua state returned by balua_create
 * \param allowCreate set to TRUE to enable the two APIs
 * ba.thread.create and ba.thread.configure
 */
BA_API void LThreadMgr_constructor(LThreadMgr* o, HttpServer* server,
   ThreadPriority priority, int stackSize,
   int threads, lua_State* L, int allowCreate);

/** Create a generic thread job.
 * \param size at least sizeof(ThreadJob)
 * \param run Your callback function will execute in the context of a
 * thread within the Thread Manager
 */
BA_API ThreadJob* ThreadJob_create(size_t size, ThreadJob_Run run);

/** Create a thread job designed to execute Lua code.
 * \param size at least sizeof(ThreadJob)
 * \param lrun Your callback function will execute in the context of a
 * thread within the Thread Manager
 */
BA_API ThreadJob* ThreadJob_lcreate(size_t size, ThreadJob_LRun lrun);

/** This function sends a thread job to an available idle thread, or
 *  queues the job if no threads are currently available. Before
 *  calling this method from outside of the server environment, you
 *  must have ownership of the #SoDisp #ThreadMutex.
 */
BA_API int LThreadMgr_run(LThreadMgr* o, ThreadJob* tj);

/** @} */ /* end of ThreadMgr */  

/** @} */ /* end of AuxLSP */  

#ifdef __cplusplus
}
#endif

/************************  Private functions ***************************/

int lGetStdSockOptions(lua_State* L, int tabIx, const char** intf,
                       BaBool* ipv6, BaBool* secure);
#ifndef NO_SHARKSSL
void lsharkssl_unlock(lua_State *L,SharkSsl* super);
SharkSsl* lsharkssl_lock(
   lua_State *L,int tabIx,SharkSsl_Role role,SharkSsl* lockedShark);
#endif
int pushCertificate(lua_State *L, SoDispCon* con);
int pushCiphers(lua_State *L, SoDispCon* con);
int calcTabSize(lua_State* L, int ix); /* lhttp.c */
struct HttpClientKeyVal;
char* extractTab(const char* tab, lua_State* L, int ix,
                 char* ud, struct HttpClientKeyVal** pkv); /* lhttp.c */

#define HTTPCLIENT "HTTPCLIENT"
struct HttpClient;
#define toHttpClient(L,ix) (struct HttpClient*)luaL_checkudata(L,ix,HTTPCLIENT)

#define BACERTSTORE "BACERTSTORE"
/* tocs: To SharkSslCertStore -- Certificate Authority
 */
#define toCertStore(L,ix) (SharkSslCertStore*)luaL_checkudata(L,ix,BACERTSTORE) 



#endif
