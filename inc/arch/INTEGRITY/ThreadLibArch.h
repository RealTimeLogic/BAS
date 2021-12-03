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
 *   COPYRIGHT:  Real Time Logic, 2011
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
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>
#include <time.h>

#define BA_OS_CHECK

#if defined(NDEBUG) && !defined(BA_OS_CHECK)
#define Thread_ce(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
BA_API void Thread_cef(Error status, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#endif


typedef struct ThreadMutexBase
{
      Value tid; /* Lock owner */
      LocalMutex mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) \
  Thread_ce(CreateLocalMutex(&(o)->mutex))

#define ThreadMutex_destructor(o) \
  Thread_ce(CloseLocalMutex((o)->mutex))

#define ThreadMutex_set(o) do {\
   Thread_ce(WaitForLocalMutex((o)->mutex));\
   Thread_ce(GetTaskUniqueId(CurrentTask(), &(o)->tid));\
} while(0)

#define ThreadMutex_release(o) do{\
   (o)->tid=0;\
   Thread_ce(ReleaseLocalMutex((o)->mutex));\
} while(0)

#ifdef __cplusplus
extern "C" {
#endif
int ThreadMutex_isOwner(ThreadMutex* o);
#ifdef __cplusplus
}
#endif



typedef struct {
  Semaphore sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) Thread_ce(CreateSemaphore(0, &(o)->sem))
#define ThreadSemaphore_destructor(o) Thread_ce(CloseSemaphore((o)->sem))
#define ThreadSemaphore_wait(o) Thread_ce(WaitForSemaphore((o)->sem))
#define ThreadSemaphore_signal(o) Thread_ce(ReleaseSemaphore((o)->sem))




typedef struct ThreadBase
{
      Task task;
      Thread_Run runnable;
} ThreadBase;



#define Thread_sleep(milliseconds) usleep(1000*milliseconds)
#define Thread_start(o) Thread_ce(RunTask((o)->task))
#define Thread_destructor(o) Thread_ce(CommonCloseTask((o)->task))

#ifdef __cplusplus
extern "C" {
#endif
BA_API void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
#ifdef __cplusplus
}
#endif


#endif
