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
 *   $Id: lxrc.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2023 - 2026
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
/** @file lxrc.h */

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
    Optional bindings implemented in xrc/lua and declared in inc/lxrc.h.
    Installation functions require an initialized BAS Lua state and ownership
    of its server mutex. Install during startup, before application scripts.
    They return no C value; Lua allocation errors follow the Lua error mechanism. 
    @{
*/


/** Install the [forkpty Lua bindings](../../../lua/auxlua.html#forkptylib).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_forkpty(lua_State* L);

/** Install the [httpc Lua bindings](../../../lua/auxlua.html#httpc).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_http(lua_State* L);

/** Install the [LuaIo Lua bindings](../../../lua/auxlua.html#luaio).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_luaio(lua_State *L);

/** Install the [SharkSSL Lua bindings](../../../lua/auxlua.html#sharkssl).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_sharkssl(lua_State *L);

/** Install the [socket Lua bindings](../../../lua/auxlua.html#socket).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_socket(lua_State* L);

/** Notify registered coroutine sockets that the server is shutting down.
    @param L Required initialized BAS Lua state with the server mutex held.
    Pending socket operations resume with E_SYS_SHUTDOWN. Lua callbacks may run
    before this function returns. Call before closing the VM; this function
    neither closes the VM nor waits for all application work to finish.
 */
void balua_relsocket(lua_State* L);

/** Install the [crypto Lua bindings](../../../lua/auxlua.html#crypto).
    @param L Required initialized BAS Lua state; borrowed for the call.
 */
void balua_crypto(lua_State *L);

struct ThreadJob;
struct LThreadMgr;

/** Install the [TraceLogger Lua bindings](../../../lua/auxlua.html#tracelogger).
    \param L Required initialized BAS Lua state with its mutex held.
    \param tmgr Borrowed manager, required for trlogger:onclient(cb); otherwise
    NULL is allowed. Keep it alive while the logger can schedule jobs.
 */
void balua_tracelogger(lua_State *L, struct LThreadMgr* tmgr);

/** @defgroup ThreadMgr The Lua Thread Library
    See Advanced Lua Bindings, section \ref fullsolution "Calling Lua Code Asynchronously From C Code" for how to use this class.

    @ingroup AuxLSP
    @{
*/


/** Generic ThreadJob callback.
    The server mutex is held. The manager frees the job after return and clears
    its Lua stack. Do not free or requeue the job or retain its thread state.
    Release the mutex for blocking external work and reacquire it before return.
    \param tj the job passed into function LThreadMgr_run
    \param mgr the manager associated with the global or dynamically
    created instance via
    [ba.thread.create](../../../lua/auxlua.html#thread_create)
 */
typedef void (*ThreadJob_Run)(struct ThreadJob* tj, struct LThreadMgr* mgr);

/** ThreadJob callback designed for calling Lua code using lua_pcall.
    The server mutex is held. Use tj->Lt and pass msgh to lua_pcall. The manager
    frees the job and clears the thread stack after return. Do not free/requeue
    the job, retain tj->Lt, or allow an unprotected Lua error to escape.
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
   lua_State* Lt; /**< Borrowed Lua thread, valid only during the job callback. */
} ThreadJob;

/** This function returns true if at least one thread is currently
 * idle and there is no need to queue the job when calling
 * #LThreadMgr_run.
 * \param o Required initialized manager; its server mutex must be held.
 * @return Nonzero if a worker is idle, zero otherwise. This is not a job result.
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
 * to stop before returning. Pending jobs may be discarded without callbacks.
 * @param o Required initialized manager. Stop submitting jobs first. Its server
 * mutex must be held; it is released and reacquired while waiting. Keep the
 * server and Lua VM alive until this returns, and do not call from its worker.
 */
BA_API void LThreadMgr_destructor(LThreadMgr* o);

/** Initialize the thread pool and install ba.thread.
 * Call with the server mutex held. The server and Lua state are borrowed and
 * must outlive the manager. Lua allocation errors and native thread-creation
 * failures use the underlying Lua/platform error handling, not a return code.
 * \param o The LThreadMgr instance to initialize.
 * \param server The HttpServer instance that owns the pool.
 * \param  priority typically set to ThreadPrioNormal
 * \param stackSize typically set to BA_STACKSZ
 * \param threads Positive initial worker count; zero provides no workers.
 * \param L the Lua state returned by balua_create
 * \param allowCreate set to TRUE to enable the two APIs
 * ba.thread.create and ba.thread.configure
 */
BA_API void LThreadMgr_constructor(LThreadMgr* o, HttpServer* server,
   ThreadPriority priority, int stackSize,
   int threads, lua_State* L, int allowCreate);

/** Create a generic thread job.
 * \param size at least sizeof(ThreadJob)
 * \param run Required callback. Your callback function will execute in the context of a
 * thread within the Thread Manager
 * \return The allocated job, or NULL if memory allocation fails.
 * Do not queue a NULL job. Additional payload storage is uninitialized.
 * Before queuing, the caller owns the allocation and may release it with baFree.
 * Queuing transfers ownership to the manager, which frees it after the callback.
 */
BA_API ThreadJob* ThreadJob_create(size_t size, ThreadJob_Run run);

/** Create a thread job designed to execute Lua code.
 * \param size at least sizeof(ThreadJob)
 * \param lrun Required callback. Your callback function will execute in the context of a
 * thread within the Thread Manager
 * \return The allocated job, or NULL if memory allocation fails.
 * Do not queue a NULL job. Additional payload storage is uninitialized.
 * Before queuing, the caller owns the allocation and may release it with baFree.
 * Queuing transfers ownership to the manager, which frees it after the callback.
 */
BA_API ThreadJob* ThreadJob_lcreate(size_t size, ThreadJob_LRun lrun);

/** This function sends a thread job to an available idle thread, or
 *  queues the job if no threads are currently available. Before
 *  calling this method from outside of the server environment, you
 *  must have ownership of the #SoDisp #ThreadMutex.
 * @param o Required running manager with at least one worker.
 * @param tj Required allocated job, not already queued. Ownership transfers.
 * @return TRUE if an idle worker was signalled, FALSE if only queued. Both
 * results mean the job was accepted; neither reports callback completion.
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
int calcTabSize(lua_State* L, int ix, BaBool query); /* lhttp.c */
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
