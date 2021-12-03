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
 *   $Id: HttpCmdThreadPool.h 4466 2019-11-08 20:11:49Z wini $
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

          Please note that failure to create the necessary resources
          is considered a fatal error.

         \param server is the HttpServer instance you bind this object to.
         \param priority is the priority for the created thread(s).
         \param stackSize is the stack size for the created thread(s).
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
          destructor. See the \ref DispatcherMutext "Dispatcher Mutext"
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
#ifdef BA_SELECT_DISP
      ThreadSemaphore sem;
#endif
} HttpCmdThreadPool;



#ifdef __cplusplus
extern "C" {
#endif
BA_API void HttpCmdThreadPool_constructor(HttpCmdThreadPool* o,
                                          HttpServer* server,
                                          ThreadPriority priority,
                                          int stackSize);
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
