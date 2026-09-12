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
 *   $Id: SoDisp.h 5978 2026-09-11 16:13:48Z wini $
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

/** @file SoDisp.h */

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

\sa ThreadMutex, SoDisp::getMutex, SoDisp::mutexSet,
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
 * differently for various operating systems. Keep registered connection
 * objects alive until they have been deactivated and removed. Registration
 * transfers no allocation ownership. Hold the dispatcher mutex when changing
 * connection registration or event state from another thread. The "generic SoDisp" is
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
          \param mutex Borrowed initialized mutex protecting the server, valid
          throughout dispatcher use. Use a real mutex for multithreaded builds;
          NULL is only suitable for ports/configurations supporting no mutex.
       */
      SoDisp(ThreadMutex* mutex);

      /** Lock the dispatcher thread.  This function protects the
       * web-server with a mutex and makes it possible for threads to
       * call functions in the web-server.  One mutexSet call must be
       * followed by one SoDisp::mutexRelease call.
       */
      void mutexSet();

      /** Release the mutex currently owned by this thread.
       * Recursive acquisition/release is not portable across ThreadLib ports.
       * Releases the mutex set with function SoDisp::mutexSet.
       */
      void mutexRelease();

      /** @return Borrowed configured mutex, possibly NULL in a no-mutex setup.
       * Does not acquire it.
       */
      ThreadMutex* getMutex();

      /** Register a connection without enabling events.
       * @param[in,out] con Borrowed initialized connection associated with this
       * dispatcher, not already registered. Call activateRec/activateSend next. */
      void addConnection(SoDispCon* con);
      /** Enable receive events.
       * @param[in,out] con Registered connection whose receive events are
       * currently inactive. Callback storage must remain valid while enabled. */
      void activateRec(SoDispCon* con);
      /** Disable receive events without closing or unregistering the connection.
       * @param[in,out] con Registered connection with receive events active. */
      void deactivateRec(SoDispCon* con);
      /** Enable send-ready events.
       * @param[in,out] con Registered valid connection with a send callback and
       * send events currently inactive. No operation in NO_ASYNCH_RESP builds. */
      void activateSend(SoDispCon* con);
      /** Disable send-ready events without closing or unregistering the connection.
       * @param[in,out] con Connection with send events active. No operation in
       * NO_ASYNCH_RESP builds. */
      void deactivateSend(SoDispCon* con);
      /** Unregister a connection without closing or freeing it.
       * @param[in,out] con Registered connection. Deactivate both receive and
       * send events before removal; repeated removal is incorrect usage. */
      void removeConnection(SoDispCon* con);

      /** Request that the current run loop exits after it regains control.
       * Does not interrupt a running callback or necessarily wake a socket
       * wait. The generic dispatcher clears this flag when run starts, so
       * setting it before run does not cancel that future call.
       */
      void setExit();

      /** Run the generic socket dispatcher loop.
       * @param[in] timeout Per-wait timeout in milliseconds. Negative values
       * (default -1) keep dispatching until setExit. Nonnegative values allow
       * return after a wait without events, but repeated activity can extend
       * the call indefinitely. Zero polls available events when sockets exist.
       * With no monitored sockets, the generic port sleeps for its internal
       * poll delay even when timeout is zero. This is not a total time budget.
       * Call from one dispatcher thread, without already owning its mutex.
       * The loop acquires the mutex for callbacks and releases it while waiting.
       * This void interface reports no select error; scheduling behavior differs
       * in non-generic dispatcher implementations.
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
/** @copydoc SoDisp::SoDisp
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_constructor(SoDisp* o, ThreadMutex* mutex);
#ifndef SoDisp_destructor
#define SoDisp_destructor(o)
#endif
/** @copydoc SoDisp::addConnection
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_addConnection(SoDisp* o, struct SoDispCon* con);
/** @copydoc SoDisp::activateRec
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_activateRec(SoDisp* o, struct SoDispCon* con);
/** @copydoc SoDisp::deactivateRec
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_deactivateRec(SoDisp* o, struct SoDispCon* con);
#ifdef NO_ASYNCH_RESP
#define SoDisp_activateSend(o, con)
#define SoDisp_deactivateSend(o, con)
#else
/** @copydoc SoDisp::activateSend
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_activateSend(SoDisp* o, struct SoDispCon* con);
/** @copydoc SoDisp::deactivateSend
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_deactivateSend(SoDisp* o, struct SoDispCon* con);
#endif
/** @copydoc SoDisp::removeConnection
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_removeConnection(SoDisp* o, struct SoDispCon* con);
/** @copydoc SoDisp::run
 * @param[in,out] o Caller-owned dispatcher instance. */
BA_API void SoDisp_run(SoDisp* o, S32 timeout);
#ifdef OSE
union SIGNAL* SoDisp_receive(SoDisp* o, S32 time, SIGSELECT* sel);
#endif
/** @param[in] o Dispatcher, or NULL.
 * @return Borrowed configured mutex, or NULL for no dispatcher/mutex. */
#define SoDisp_getMutex(o) ((o) ? ((o)->mutex) : 0)
/** Acquire the configured mutex; do not acquire recursively.
 * @param[in,out] o Initialized dispatcher with a usable mutex. */
#define SoDisp_mutexSet(o) ThreadMutex_set((o)->mutex)
/** Release the configured mutex owned by this thread.
 * @param[in,out] o Initialized dispatcher. */
#define SoDisp_mutexRelease(o) ThreadMutex_release((o)->mutex)
/** @copydetails SoDisp::setExit
 * @param[in,out] o Dispatcher whose active run loop should exit. */
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
