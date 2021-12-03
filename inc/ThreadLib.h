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
 *   $Id: ThreadLib.h 4915 2021-12-01 18:26:55Z wini $
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

#ifndef ThreadLib_hpp

#include <TargConfig.h>

/** @defgroup ThreadLib Thread handling

    \sa <a href="../../index.html" _target="top"> Barracuda Introduction </a>

   @{
 */

/** Thread priority list.
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

/** Prototype for the thread run method.
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

/** This class provides an easy and less error prone way of locking
    and releasing a mutex. The class should be instantiated on the
    stack such that the constructor locks the mutex when the scope
    becomes active and the destructor releases the mutex when the
    scope becomes inactive.

    \code
    { ThreadLock lock(dispMutex);
       //Protected web-server calls here
    }
   \endcode

   This is a C++ class only.

   \sa ThreadMutex ThreadReleaseLock
*/
struct ThreadLock
{
      /** Lock a region of code. */
      ThreadLock(ThreadMutex& m);

      /** Lock a region of code. */
      ThreadLock(ThreadMutex* m);

      /** Unlock the mutex. */
      ~ThreadLock();
   private:
      friend struct ThreadReleaseLock;
      struct ThreadMutex* mutex;
};


/** This class provides an easy and less error prone way of
    temporarily releasing a locked mutex. This class is the inverse of
    class ThreadLock.

    \code
    void // An HttpPage service function
    myPagePage::service(HttpPage* page,
                         HttpRequest* request,
                         HttpResponse* response)
    {
       //Normal web-server calls here
       { ThreadReleaseLock rlock(req);
            //Non web-server calls here
       }
       //Normal web-server calls here
    }
    \endcode

   \sa ThreadMutex ThreadLock
*/
struct ThreadReleaseLock
{
      /** Temporarily unlock a locked mutex from within a resource. */
      ThreadReleaseLock(struct HttpRequest* req);
      /** Temporarily unlock a locked mutex. */
      ThreadReleaseLock(ThreadMutex& m);
      /** Temporarily unlock a locked mutex. */
      ThreadReleaseLock(ThreadMutex* m);
      /** Temporarily unlock a locked mutex. */
      ThreadReleaseLock(ThreadLock& tl);
      /** Temporarily unlock a locked mutex. */
      ThreadReleaseLock(ThreadLock* tl);
      /** Lock the temporarily unlock mutex. */
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
   \ref DispatcherMutext "Dispatcher Mutext"
 */
struct ThreadMutex : public ThreadMutexBase
{
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Create a mutex. */
      ThreadMutex() { ThreadMutex_constructor(this); }
      /** Destroy a mutex. */
      ~ThreadMutex() { ThreadMutex_destructor(this); }
      /** Lock the mutex. */
      void set() { ThreadMutex_set(this); }
      /** Release the mutex. */
      void release() { ThreadMutex_release(this); }
      /** Returns true if the mutex is locked and the current thread
          is the owner.
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

      /** Create a semaphore and set the counter to zero.
       */
      ThreadSemaphore(){ ThreadSemaphore_constructor(this); }
      /** destroy the semaphore
       */
      ~ThreadSemaphore(void) { ThreadSemaphore_destructor(this); }

      /** Wait (block) for another thread to signal (start) the thread.
       */
      void wait() { ThreadSemaphore_wait(this); }

      /** Signal a waiting (blocking) thread.
       */
      void signal() { ThreadSemaphore_signal(this); }
};


/** A simple thread class.
    The thread class makes it possible to design a platform
    independent system using threads.
*/
struct Thread : public ThreadBase
{
      Thread() {}
      void *operator new(size_t s) { return ::baMalloc(s); }
      void operator delete(void* d) { if(d) ::baFree(d); }
      void *operator new(size_t, void *place) { return place; }
      void operator delete(void*, void *) { }

      /** Create a thread.
          \param r a pointer to the thread run method.
          \param priority the thread priority.
          \param stackSize the thread stack size.
      */
      Thread(Thread_Run r, ThreadPriority priority, int stackSize) {
         Thread_constructor(this, r, priority, stackSize); }
      ~Thread() { Thread_destructor(this); }
      /** Start the thread.
       */
      void start() {  Thread_start(this); }

      /** Suspend the current thread for n milliseconds.
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
