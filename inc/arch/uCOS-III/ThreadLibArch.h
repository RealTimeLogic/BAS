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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic, 2022
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

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>
#include <os.h>

#if !defined(OS_CFG_MUTEX_DEL_EN) || OS_CFG_MUTEX_DEL_EN == 0
#error OS_CFG_MUTEX_DEL_EN must be enabled
#endif

#define TST_OK(err) baAssert(OS_ERR_NONE == err)

typedef struct ThreadMutexBase
{
      OS_TCB* tid; /* Lock owner */
      OS_MUTEX mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do{                  \
      OS_ERR  err;                                    \
      (o)->tid = 0;                                     \
      OSMutexCreate(&(o)->mutex, "BAS",&err);            \
      TST_OK(err);                                      \
   }while(0)

#define ThreadMutex_destructor(o) do{                   \
      OS_ERR  err;                                              \
      OSMutexDel(&(o)->mutex, OS_OPT_DEL_ALWAYS ,&err);            \
      TST_OK(err);                                                \
   }while(0)


#define ThreadMutex_set(o)  do{                                         \
      OS_ERR  err;                                                    \
      OSMutexPend(&(o)->mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);        \
      TST_OK(err);                                                      \
      (o)->tid = OSTCBCurPtr;                                           \
   }while(0)

#define ThreadMutex_release(o)  do{                                     \
      OS_ERR  err;                                                    \
      (o)->tid = 0;                                                     \
      OSMutexPost(&(o)->mutex, OS_OPT_POST_NONE, &err);                  \
      TST_OK(err);                                                      \
   }while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == OSTCBCurPtr)


typedef struct ThreadSemaphoreBase
{
   OS_SEM sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o)  do{                             \
      OS_ERR  err;                                                    \
      OSSemCreate(&(o)->sem, "BAS", 0, &err);                           \
      TST_OK(err);                                                      \
   }while(0)

#define ThreadSemaphore_destructor(o)  do{                              \
      OS_ERR  err;                                                    \
      OSSemDel(&(o)->sem, OS_OPT_DEL_ALWAYS, &err);                     \
      TST_OK(err);                                                      \
   }while(0)

#define ThreadSemaphore_wait(o)  do{                                    \
      OS_ERR  err;                                                    \
      OSSemPend(&(o)->sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);           \
      TST_OK(err);                                                      \
   }while(0)

#define ThreadSemaphore_signal(o)  do{                                  \
      OS_ERR  err;                                                    \
      OSSemPost(&(o)->sem, OS_OPT_POST_1,&err);                         \
      TST_OK(err);                                                      \
   }while(0)


typedef struct ThreadBase
{
   Thread_Run runnable;
   OS_TCB tid;
} ThreadBase;


#ifdef __cplusplus
extern "C" {
#endif

void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);

/* Not implemented */
#define Thread_destructor(o) baAssert(0)

#define Thread_sleep(ms)   do{                                          \
      OS_ERR err;                                                     \
      OSTimeDlyHMSM(0,0,0,ms,OS_OPT_TIME_HMSM_NON_STRICT,&err);              \
      TST_OK(err);                                                      \
   } while(0)


#define Thread_start(o)   do{                                           \
      OS_ERR  err;                                                    \
      OSTaskSemPost(&(o)->tid, OS_OPT_POST_NONE,&err);                  \
      TST_OK(err);                                                      \
   }while(0)
#ifdef __cplusplus
}
#endif

#endif
