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
 *   $Id: ThreadLibArch.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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
#define ThreadLib_hpp

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <TargConfig.h>

#ifndef POLICY
#define POLICY SCHED_FIFO
#endif

#ifdef __CYGWIN__
#include <string.h>
#define SYGW_SEM_FIX(sem) memset(&sem, 0, sizeof(sem))
#else
#define SYGW_SEM_FIX(sem)
#endif

#define INFINITE ((unsigned long)~0)

#ifdef NDEBUG
#define Thread_ce(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
BA_API void Thread_cef(int status, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#endif

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
      pthread_t tid; /* Lock owner */
      pthread_mutex_t mutex;
} ThreadMutexBase;

#define ThreadMutex_destructor(o) Thread_ce(pthread_mutex_destroy(&(o)->mutex))
#define ThreadMutex_set(o) do {\
   Thread_ce(pthread_mutex_lock(&(o)->mutex));\
   (o)->tid = pthread_self();\
} while(0)
#define ThreadMutex_release(o) do{\
   (o)->tid=0;\
   Thread_ce(pthread_mutex_unlock(&(o)->mutex));\
} while(0)
#define ThreadMutex_isOwner(o) ((o)->tid == pthread_self())
#ifdef __cplusplus
extern "C" {
#endif
BA_API void ThreadMutex_constructor(struct ThreadMutexBase* o);
#ifdef __cplusplus
}
#endif


typedef struct ThreadSemaphoreBase
{
#ifdef __APPLE__
      sem_t* semPtr;
#else
      sem_t sem;
#endif
} ThreadSemaphoreBase;

#ifdef __cplusplus
extern "C" {
#endif
BA_API void ThreadSemaphore_constructor(struct ThreadSemaphoreBase* o);
BA_API void ThreadSemaphore_destructor(struct ThreadSemaphoreBase* o);
#ifdef __cplusplus
}
#endif

#ifdef __APPLE__
#define SEM_PTR(o) (o)->semPtr
#else
#define SEM_PTR(o) &(o)->sem
#endif

#if defined(EINTR)
/* extern */
#ifdef __cplusplus
extern "C" {
#endif
BA_API int xsem_wait(sem_t *sem);
#ifdef __cplusplus
}
#endif
#else
#define xsem_wait sem_wait
#endif
#define ThreadSemaphore_wait(o) \
   Thread_ce(xsem_wait(SEM_PTR(o)))
#define ThreadSemaphore_signal(o) \
   Thread_ce(sem_post(SEM_PTR(o)))



typedef struct ThreadBase
{
      ThreadSemaphoreBase startSem;
      pthread_t tid;
      pthread_attr_t attr;
      Thread_Run runnable;
} ThreadBase;

#define Thread_sleep(ms) do {                   \
      struct timespec tp;                       \
      tp.tv_sec = (ms)/1000;                    \
      tp.tv_nsec = ((ms) % 1000) * 1000000;     \
      nanosleep(&tp,0);                         \
   } while(0)

#define Thread_destructor(o)
#ifdef __cplusplus
extern "C" {
#endif
BA_API void Thread_start(Thread* o);
BA_API void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
#ifdef __cplusplus
}
#endif


#endif
