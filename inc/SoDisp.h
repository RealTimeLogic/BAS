/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: SoDisp.h 5078 2022-02-10 22:52:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2002-2021
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
 *
 */

#ifndef _SoDisp_h
#define _SoDisp_h

#include <HttpCfg.h>
#include <ThreadLib.h>


struct SoDispCon;
struct HttpServer;

/*! \page DispatcherMutext The SoDisp mutex.

The Barracuda code is protected by one single lock, the SoDisp
mutex. Callback functions, running in the context of the SoDisp
or from a thread in the HttpCmdThreadPool, have this mutex locked. The
HttpDir and HttpPage service functions are examples of callback
functions running in a context of the dispatcher. These callback
functions or any code running in the context of the dispatcher need
normally not be concerned about the SoDisp mutex. However, any
code running from other threads or from the thread running the
dispatcher itself must explicitly set this lock prior to calling any
Barracuda functions.

\sa ThreadMutex, SoDisp::getLock, SoDisp::mutexSet,
SoDisp::mutexRelease,
HttpServer::getDispatcher, HttpRequest::getServer,
HttpResponse::getRequest, SoDispCon::getDispatcher,
<a href ="../../introduction.html#threads">Using multiple threads</a>,
*/


/** The SoDisp dispatches any socket connection that contains data by
 * calling the SoDispCon::execute member function in a SoDispCon
 * object.
 * <p>
 * This class contains the socket dispatcher loop which waits for data on
 * all registered socket connections. A socket connection is stored in
 * a SoDispCon object, and the dispatcher object dispatches the socket
 * connection by calling SoDispCon::execute.
 * </p>
 * <p>
 * The SoDisp class is platform dependent and may be implemented
 * differently for various operating systems. The "generic SoDisp" is
 * using "socket select" and waits in a loop for the next active
 * socket connection.</p>
 */
typedef struct SoDisp
{
#ifdef __cplusplus

      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** The constructor.
          \param mutex is protecting the web-server code.
       */
      SoDisp(ThreadMutex* mutex);

      /** Lock the dispatcher thread.  This function protects the
       * web-server with a mutex and makes it possible for threads to
       * call functions in the web-server.  One mutexSet call must be
       * followed by one HttpServer::mutexRelease call.
       */
      void mutexSet();

      /** Releases the mutex if this is the last 'pop'.
       * Releases the mutex set with function SoDisp::mutexSet.
       */
      void mutexRelease();

      /** Returns the SoDisp mutex.
       */
      ThreadMutex* getMutex();

      void addConnection(SoDispCon* con);
      void activateRec(SoDispCon* con);
      void deactivateRec(SoDispCon* con);
      void activateSend(SoDispCon* con);
      void deactivateSend(SoDispCon* con);
      void removeConnection(SoDispCon* con);

      /** The callbacks can force the dispatcher to exit the run method.
       */
      void setExit();

      /** Call the socket dispatcher loop, which dispatches HTTP requests.
          The default argument for C++ is to set timeout=-1, which makes
          the function block, i.e. never return. C code must always provide
          an argument, which is normally -1. The function can be used with a
          polled-based system by setting the timeout value.
          \code
          for(;;)
          {
             printf("Execute SoDisp::run\n");
             dispatcher->run(1100); //Set poll frequency to 1.1 second.
          }
          \endcode
          This method is used with the generic dispatcher.
      */
      void run(S32 timeout=-1);
#endif
      DISPATCHER_DATA;
      ThreadMutex* mutex;
      void* timeoutO;
      BaBool doExit;
} SoDisp;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void SoDisp_constructor(SoDisp* o, ThreadMutex* mutex);
#ifndef SoDisp_destructor
#define SoDisp_destructor(o)
#endif
BA_API void SoDisp_addConnection(SoDisp* o, struct SoDispCon* con);
BA_API void SoDisp_activateRec(SoDisp* o, struct SoDispCon* con);
BA_API void SoDisp_deactivateRec(SoDisp* o, struct SoDispCon* con);
#ifdef NO_ASYNCH_RESP
#define SoDisp_activateSend(o, con)
#define SoDisp_deactivateSend(o, con)
#else
BA_API void SoDisp_activateSend(SoDisp* o, struct SoDispCon* con);
BA_API void SoDisp_deactivateSend(SoDisp* o, struct SoDispCon* con);
#endif
BA_API void SoDisp_removeConnection(SoDisp* o, struct SoDispCon* con);
BA_API void SoDisp_run(SoDisp* o, S32 timeout);
#ifdef OSE
union SIGNAL* SoDisp_receive(SoDisp* o, S32 time, SIGSELECT* sel);
#endif
#define SoDisp_getMutex(o) ((o) ? ((o)->mutex) : 0)
#define SoDisp_mutexSet(o) ThreadMutex_set((o)->mutex)
#define SoDisp_mutexRelease(o) ThreadMutex_release((o)->mutex)
#define SoDisp_setExit(o) (o)->doExit=TRUE

BA_API void SoDisp_newCon(SoDisp*, struct SoDispCon*);

#ifdef __cplusplus
}
inline SoDisp::SoDisp(ThreadMutex* mutex) {
   SoDisp_constructor(this, mutex); }
inline void SoDisp::addConnection(SoDispCon* con) {
   SoDisp_addConnection(this, con); }
inline void SoDisp::activateRec(SoDispCon* con) {
   SoDisp_activateRec(this, con); }
inline void SoDisp::deactivateRec(SoDispCon* con) {
   SoDisp_deactivateRec(this, con); }
inline void SoDisp::activateSend(SoDispCon* con) {
   SoDisp_activateSend(this, con); }
inline void SoDisp::deactivateSend(SoDispCon* con) {
   SoDisp_deactivateSend(this, con); }
inline void SoDisp::removeConnection(SoDispCon* con) {
   SoDisp_removeConnection(this, con); }
inline void SoDisp::run(S32 timeout) {
   SoDisp_run(this, timeout); }
inline ThreadMutex* SoDisp::getMutex() {
   return this->mutex; }
#ifdef OSE
inline union SIGNAL* SoDisp::receive(S32 time, SIGSELECT* sel) {
   return SoDisp_receive(this, time, sel); }
#endif
inline void SoDisp::mutexSet() { SoDisp_mutexSet(this); }
inline void SoDisp::mutexRelease() {SoDisp_mutexRelease(this);}
inline void SoDisp::setExit() { SoDisp_setExit(this); }

#endif

void SoDisp_setTimeout(
   SoDisp* o, BaTime t, void (*cb)(void* object), void* object);


#endif
