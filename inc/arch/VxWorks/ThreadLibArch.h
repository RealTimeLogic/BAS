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
 *   COPYRIGHT:  Real Time Logic, 2007 - 2019
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

#include <TargConfig.h>
#include <semLib.h>
#include <taskLib.h>
#include <tickLib.h>
//#include <timerDev.h>

struct ThreadSemaphoreBase;
struct ThreadMutexBase;


#ifdef NDEBUG
#define Thread_ce(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
void Thread_cef(int status, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#endif
 


typedef struct ThreadMutexBase
{
#if (VXWORKS_VER < 0x690)
   int tid;
#else
   TASK_ID tid;
#endif
   SEM_ID sem;
} ThreadMutexBase;

#define ThreadMutex_constructor(o)  do{\
   (o)->tid = 0;\
  (o)->sem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);\
}while(0)


#define ThreadMutex_destructor(o) Thread_ce(semDelete((o)->sem))

#define ThreadMutex_set(o)  do{\
   Thread_ce(semTake((o)->sem, WAIT_FOREVER));\
   (o)->tid = taskIdSelf();\
}while(0)


#define ThreadMutex_release(o)  do{\
   (o)->tid = 0;\
   Thread_ce(semGive((o)->sem));\
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == taskIdSelf())


typedef struct ThreadSemaphoreBase
{
      SEM_ID sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) \
  ((o)->sem = semCCreate(SEM_Q_FIFO, SEM_EMPTY))
#define ThreadSemaphore_destructor(o) Thread_ce(semDelete((o)->sem))
#define ThreadSemaphore_wait(o) \
   Thread_ce(semTake ((o)->sem, WAIT_FOREVER))
#define ThreadSemaphore_signal(o) \
   Thread_ce(semGive((o)->sem))


typedef struct ThreadBase
{
#if (VXWORKS_VER < 0x690)
   int tid;
#else
   TASK_ID tid;
#endif
   Thread_Run runnable;
} ThreadBase;

#define Thread_start(o) Thread_ce(taskActivate((o)->tid))

#define Thread_sleep(ms) do{ \
   U32 sysClkRate = (U32)sysClkRateGet(); \
   taskDelay((U32)(ms) > 0xFFFFFFFF / sysClkRate ? \
             ((U32)(ms) / 1000) * sysClkRate : \
             ((U32)(ms) * sysClkRate) / 1000); \
} while(0)

#define Thread_destructor(o) taskDelete((o)->tid)
#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
#ifdef __cplusplus
}
#endif


#endif
