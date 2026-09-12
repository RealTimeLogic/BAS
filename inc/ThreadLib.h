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
 *   $Id: ThreadLib.h 5978 2026-09-11 16:13:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2004 - 2008
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

/** @file ThreadLib.h */

#ifndef ThreadLib_hpp

#include <TargConfig.h>

/** @defgroup ThreadLib Thread handling

    \sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>

   @{
 */

/** Portable priority choices, mapped to the target operating system.
    Actual scheduling policy and supported priority differences are port-specific.
    You can set 1 of 5 priorities: 
   ThreadPrioLowest, 
   ThreadPrioLow, 
   ThreadPrioNormal, 
   ThreadPrioHigh and
   ThreadPrioHighest 

*/
typedef enum { 
   ThreadPrioLowest, 
   ThreadPrioLow, 
   ThreadPrioNormal, 
   ThreadPrioHigh, 
   ThreadPrioHighest 
} ThreadPriority;


#ifndef __DOXYGEN__
struct HttpRequest;
struct ThreadMutex;
struct Thread;
#endif

/** Entry point invoked in the new thread after Thread::start.
 * @param[in,out] th Borrowed originating Thread object, valid until execution
 * has finished. Returning finishes the entry point; it does not free th.
 * The dispatcher mutex is not automatically held on entry.
 */
typedef void (*Thread_Run)(struct Thread* th);

#ifdef __cplusplus
extern "C" {
#else
struct ThreadReleaseLock;
#endif

BA_API void ThreadReleaseLock_internalConstructor(
   struct ThreadReleaseLock* o, struct HttpRequest* req);

#ifdef __cplusplus
}
#endif




#if defined(__cplusplus)

#ifndef __DOXYGEN__
struct ThreadReleaseLock;
#endif

/** This class provides an easy and less error-prone way of locking
    and releasing a mutex. The class should be instantiated on the
    stack such that the constructor locks the mutex when the scope
    becomes active and the destructor releases the mutex when the
    scope becomes inactive.

    \code
    { ThreadLock lock(dispMutex);
       //Protected web-server calls here
    }
   \endcode

   This is a C++ class only. Use a named local variable so it lives for the
   intended scope. Do not copy a lock object or acquire a mutex recursively;
   recursive acquisition is not portable across ThreadLib implementations.

   \sa ThreadMutex ThreadReleaseLock
*/
struct ThreadLock
{
      /** Acquire the mutex until scope exit.
       * @param[in,out] m Borrowed initialized mutex, valid for the lock lifetime. */
      ThreadLock(ThreadMutex& m);

      /** Acquire the mutex until scope exit.
       * @param[in,out] m Required non-NULL borrowed initialized mutex. */
      ThreadLock(ThreadMutex* m);

      /** Unlock the mutex. */
      ~ThreadLock();
   private:
      friend struct ThreadReleaseLock;
      struct ThreadMutex* mutex;
};


/** This class provides an easy and less error-prone way of
    temporarily releasing a locked mutex. This class is the inverse of
    class ThreadLock.

    \code
    // Inside a request handler entered with the dispatcher mutex held:
    {
       ThreadReleaseLock rlock(request);
       // Perform blocking application work here, without calling BAS APIs
       // or accessing shared BAS objects while their mutex is released.
    } // The same mutex is acquired again before BAS work resumes.
    \endcode

   \sa ThreadMutex ThreadLock
*/
struct ThreadReleaseLock
{
      /** Temporarily release the owned mutex; reacquire at scope exit.
       * @param[in,out] req Required live request whose dispatcher mutex the current thread owns. */
      ThreadReleaseLock(struct HttpRequest* req);
      /** Temporarily release the owned mutex; reacquire at scope exit.
       * @param[in,out] m Borrowed initialized mutex currently owned by this thread. */
      ThreadReleaseLock(ThreadMutex& m);
      /** Temporarily release the owned mutex; reacquire at scope exit.
       * @param[in,out] m Required non-NULL initialized mutex owned by this thread. */
      ThreadReleaseLock(ThreadMutex* m);
      /** Temporarily release the owned mutex; reacquire at scope exit.
       * @param[in,out] tl Borrowed active lock object, valid until this release guard ends. */
      ThreadReleaseLock(ThreadLock& tl);
      /** Temporarily release the owned mutex; reacquire at scope exit.
       * @param[in,out] tl Required non-NULL active lock object, valid until this guard ends. */
      ThreadReleaseLock(ThreadLock* tl);
      /** Reacquire the same mutex before leaving scope. Do not copy this guard. */
      ~ThreadReleaseLock();
   private:
      struct ThreadMutex* mutex;
};


#else
#define ThreadMutexBase ThreadMutex
#define ThreadSemaphoreBase ThreadSemaphore
#define ThreadBase Thread
#endif


#include "ThreadLibArch.h"

#if defined(__cplusplus)

/** A mutual exclusion class.
   \sa ThreadLock, ThreadReleaseLock and
   \ref DispatcherMutext "Dispatcher Mutex"
 */
struct ThreadMutex : public ThreadMutexBase
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Initialize target mutex resources (C: ThreadMutex_constructor).
       * Resource-failure handling is port-specific; no status is returned. */
      ThreadMutex() { ThreadMutex_constructor(this); }
      /** Release target resources after all owners and waiters have stopped
       * (C: ThreadMutex_destructor). Do not destroy a locked/in-use mutex. */
      ~ThreadMutex() { ThreadMutex_destructor(this); }
      /** Wait until the mutex can be acquired (C: ThreadMutex_set).
       * Do not recursively acquire it; portability requires one matching release. */
      void set() { ThreadMutex_set(this); }
      /** Release a mutex owned by this thread (C: ThreadMutex_release).
       * Calling from another thread or without acquisition is incorrect usage. */
      void release() { ThreadMutex_release(this); }
      /** @return True if the current thread owns the mutex, false otherwise.
          C equivalent: ThreadMutex_isOwner. This is an ownership check,
          not a test that another thread can safely acquire the mutex.
      */
      bool isOwner() { return ThreadMutex_isOwner(this)?true:false; }
};

/** A simple semaphore implementation.
    The semaphore class is used as a simple mechanism where one
    thread can send a message to another thread. This means that the
    underlying implementation of the thread class does not necessarily
    have to be a semaphore.
*/
struct ThreadSemaphore : public ThreadSemaphoreBase
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Create an initially unsignaled notification object
       * (C: ThreadSemaphore_constructor). No construction status is returned.
       */
      ThreadSemaphore(){ ThreadSemaphore_constructor(this); }
      /** Release target resources only after signalers and waiters have stopped
       * (C: ThreadSemaphore_destructor).
       */
      ~ThreadSemaphore(void) { ThreadSemaphore_destructor(this); }

      /** Block until a notification can be consumed (C: ThreadSemaphore_wait).
       * This call has no timeout parameter and returns no status.
       */
      void wait() { ThreadSemaphore_wait(this); }

      /** Signal a waiter or make a notification available for a later wait
       * (C: ThreadSemaphore_signal). Queued-notification capacity and whether
       * repeated signals accumulate are port-specific. Returns no status.
       */
      void signal() { ThreadSemaphore_signal(this); }
};


/** A simple thread class.
    The thread class makes it possible to design a platform
    independent system using threads.
*/
struct Thread : public ThreadBase
{
      /** Uninitialized storage; call Thread_constructor before use/destruction. */
      Thread() {}
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Create a thread.
          \param r Required entry-point callback invoked after start. Keep the
          Thread object and callback dependencies alive until execution finishes.
          \param priority One of the five ThreadPriority values, mapped by the port.
          \param stackSize Requested stack allocation in bytes, a positive value
          large enough for the callback and its libraries. Ports can round up
          or add overhead; this is not a portable exact stack-size guarantee.
          Construction creates thread resources but start releases the entry
          point to run. No failure status is returned; major ports call baFatalE
          for creation failures. C equivalent: Thread_constructor.
      */
      Thread(Thread_Run r, ThreadPriority priority, int stackSize) {
         Thread_constructor(this, r, priority, stackSize); }
      /** Release platform thread resources (C: Thread_destructor).
       * This is not a portable join or cancellation API. Arrange completion
       * before destroying storage used by a running thread. */
      ~Thread() { Thread_destructor(this); }
      /** Release the constructed thread to run its entry point once
       * (C: Thread_start). Call only once; does not wait for completion.
       */
      void start() {  Thread_start(this); }

      /** Delay the calling thread (C: Thread_sleep).
       * @param[in] milliseconds Requested delay in milliseconds. Resolution,
       * rounding and zero-delay behavior are platform-specific. Does not
       * release an application mutex; scheduler delays can extend the wait.
       */
      static void sleep(unsigned int milliseconds) {
         Thread_sleep(milliseconds); }
};

inline ThreadLock::ThreadLock(ThreadMutex& m) : mutex(&m) { mutex->set(); }
inline ThreadLock::ThreadLock(ThreadMutex* m) : mutex(m) { mutex->set(); }
inline ThreadLock::~ThreadLock() { mutex->release(); }

inline ThreadReleaseLock::ThreadReleaseLock(struct HttpRequest* req) {
   ThreadReleaseLock_internalConstructor(this, req);
}
inline ThreadReleaseLock::ThreadReleaseLock(ThreadMutex& m) :
   mutex(&m) { mutex->release(); }
inline ThreadReleaseLock::ThreadReleaseLock(ThreadMutex* m) : 
   mutex(m) { mutex->release(); }
inline ThreadReleaseLock::ThreadReleaseLock(ThreadLock& tl) :
   mutex(tl.mutex) { mutex->release(); }
inline ThreadReleaseLock::ThreadReleaseLock(ThreadLock* tl) :
   mutex(tl->mutex) { mutex->release(); }
inline ThreadReleaseLock::~ThreadReleaseLock() { mutex->set(); }

#endif

/** @} */ /* end of ThreadLib group */

#endif
