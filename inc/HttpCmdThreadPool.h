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
 *            HEADER
 *
 *   $Id: HttpCmdThreadPool.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004-2012
 *
 *   This software is copyrighted by and is the sole property of Real
 *   Time Logic LLC.  All rights, title, ownership, or other interests in
 *   the software remain the property of Real Time Logic LLC.  This
 *   software may only be used in accordance with the terms and
 *   conditions stipulated in the corresponding license agreement under
 *   which the software has been supplied.  Any unauthorized use,
 *   duplication, transmission, distribution, or disclosure of this
 *   software is expressly forbidden.
 *                                                                        
 *   This Copyright notice may not be removed or modified without prior
 *   written consent of Real Time Logic LLC.
 *                                                                         
 *   Real Time Logic LLC. reserves the right to modify this software
 *   without notice.
 *
 *               http://www.realtimelogic.com
 ****************************************************************************
 *
 */

/** @file HttpCmdThreadPool.h */

#ifndef _HttpCmdThreadPool_h
#define _HttpCmdThreadPool_h


#include <DoubleList.h>
#include <HttpCmdThreadPoolIntf.h>

struct HttpCmdThreadPool;

typedef enum {
   HttpCmdThreadState_Idle,
   HttpCmdThreadState_RunDir,
   HttpCmdThreadState_Exit
} HttpCmdThreadState;


#ifndef __DOXYGEN__
typedef struct
{
      Thread super;
      DoubleLink node;
      ThreadSemaphore sem;
      struct HttpCmdThreadPool* pool;
      HttpCommand* cmd;
      HttpDir* dir;
      HttpCmdThreadState state;
} HttpCmdThread;
#endif

/** @addtogroup ThreadLib
    @{
 */

/** An instance of this class provides a thread pool to an instance of
    the HttpServer class. An instance of this class creates N threads
    where N is identical to the value set with method
    HttpServerConfig::setNoOfHttpCommands.

    <b>See the <a href="../../misc/HttpCmdThreadPool.html">
    Http Command Thread Pool </a> documentation for more information</b>.

*/
typedef struct HttpCmdThreadPool
{
#ifdef __cplusplus

      /** Create an instance of HttpCmdThreadPool and bind the thread
          pool to an instance of the HttpServer class. The constructor
          creates N threads, where N is set identical to the number of
          HttpCommand instances in the HttpServer object. See
          HttpServerConfig::setNoOfHttpCommands for more
          information.

          The constructor returns no status. Platform thread creation failures
          use the Thread implementation's error handling. The worker array must
          be allocated successfully for the pool to serve requests.

         \param server Required initialized server, retained by the pool. Keep it
         alive until after the pool is destroyed.
         \param priority ThreadPriority used for every worker.
         \param stackSize Stack size in bytes for each worker, subject to the
         target Thread implementation requirements.
      */
      HttpCmdThreadPool(
         HttpServer* server,ThreadPriority priority,int stackSize);

      /** Terminates all threads in the thread pool and deregisters
          the thread pool in the HttpServer instance. Make sure you
          run this destructor before you terminate the HttpServer
          instance. Please note that calling this destructor may take
          considerable time as the destructor must wait for all
          threads to complete.

          The dispatcher must be locked prior to calling the
          destructor. See the \ref DispatcherMutext "Dispatcher Mutex"
          for more information.

      */
      ~HttpCmdThreadPool();
   private:
#endif
      HttpCmdThreadPoolIntf super;
      DoubleList freeList;
      DoubleList runningList;
      SoDisp* dispatcher;
      HttpServer* server;
      HttpCmdThread* pool;
} HttpCmdThreadPool;



#ifdef __cplusplus
extern "C" {
#endif
/** Create workers and bind the pool to a server.
 * @param[out] o Caller-owned pool object.
 * @param[in,out] server Required initialized server that outlives the pool.
 * @param[in] priority ThreadPriority for each worker.
 * @param[in] stackSize Per-worker stack size in bytes.
 * No recoverable construction status is returned.
 * @see HttpCmdThreadPool::HttpCmdThreadPool */
BA_API void HttpCmdThreadPool_constructor(HttpCmdThreadPool* o,
                                          HttpServer* server,
                                          ThreadPriority priority,
                                          int stackSize);
/** Wait for worker completion and detach the pool.
 * @param[in,out] o Initialized pool, destroyed before its server. The caller
 * must hold the dispatcher mutex; it is released and reacquired while waiting.
 * Do not call from one of this pool's workers. */
BA_API void HttpCmdThreadPool_destructor(HttpCmdThreadPool* o);
#ifdef __cplusplus
}
inline HttpCmdThreadPool::HttpCmdThreadPool(HttpServer* server,
                                            ThreadPriority priority,
                                            int stackSize) {
   HttpCmdThreadPool_constructor(this, server, priority, stackSize); }
inline HttpCmdThreadPool::~HttpCmdThreadPool() {
   HttpCmdThreadPool_destructor(this); }
#endif

/** @} */ /* end of ThreadLib group */


#endif
