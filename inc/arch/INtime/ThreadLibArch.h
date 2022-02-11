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
 *   $Id: ThreadLibArch.h 5076 2022-02-10 16:59:48Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2020
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

#include <rtbase.h>

#ifdef NDEBUG
#define Thread_ce(x)
#define Thread_hndl(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
BA_API RTHANDLE Thread_hndlf(RTHANDLE rth, const char* file, int line);
BA_API void Thread_cef(BOOLEAN ok, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#define Thread_hndl(x) Thread_hndlf(x, __FILE__, __LINE__)
#endif

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
   RTHANDLE tid; /* Lock owner */
   RTHANDLE mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do {                             \
   (o)->mutex = Thread_hndl(CreateRtSemaphore(1,1,PRIORITY_QUEUING)); \
   (o)->tid=BAD_RTHANDLE;                                           \
}while (0)

#define ThreadMutex_destructor(o) do {          \
   Thread_ce(DeleteRtSemaphore((o)->mutex));    \
} while(0)


#ifdef NDEBUG
#define ThreadMutex_set(o) do {                                      \
   WaitForRtSemaphore((o)->mutex, 1, WAIT_FOREVER));                 \
   baAssert((o)->tid == BAD_RTHANDLE);                               \
   (o)->tid = GetRtThreadHandles(THIS_THREAD);                       \
} while(0)
#else
#define ThreadMutex_set(o) do {                                         \
   Thread_ce(WAIT_FAILED != WaitForRtSemaphore((o)->mutex, 1, WAIT_FOREVER)); \
   baAssert((o)->tid == BAD_RTHANDLE);                                  \
   (o)->tid = GetRtThreadHandles(THIS_THREAD);                          \
} while(0)
#endif

#define ThreadMutex_release(o) do{                   \
   baAssert(ThreadMutex_isOwner(o));                 \
   (o)->tid=BAD_RTHANDLE;                            \
   Thread_ce(ReleaseRtSemaphore((o)->mutex,1));      \
} while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == GetRtThreadHandles(THIS_THREAD))



typedef struct ThreadSemaphoreBase
{
   RTHANDLE sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) do {                          \
   (o)->sem = Thread_hndl(CreateRtSemaphore(0,1,PRIORITY_QUEUING));  \
}while (0)

#define ThreadSemaphore_destructor(o) Thread_ce(DeleteRtSemaphore((o)->sem))

#ifdef NDEBUG
#define ThreadSemaphore_wait(o) WaitForRtSemaphore((o)->sem),1,WAIT_FOREVER)
#else
#define ThreadSemaphore_wait(o) \
   Thread_ce(WAIT_FAILED != WaitForRtSemaphore((o)->sem,1,WAIT_FOREVER))
#endif

#define ThreadSemaphore_signal(o) Thread_ce(ReleaseRtSemaphore((o)->sem,1))













typedef struct ThreadBase
{
      ThreadSemaphoreBase startSem;
      RTHANDLE tid;
      Thread_Run runnable;
} ThreadBase;

#define Thread_sleep(ms) RtSleep(ms)

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
