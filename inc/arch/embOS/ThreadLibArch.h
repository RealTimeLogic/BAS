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
 *   $Id: TargConfig.h 2195 2010-09-16 12:26:20Z wini $
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
 *
 */
#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>

#ifdef NDEBUG
#define TST_OK(x) x
#else
#define TST_OK(x) baAssert((x) <= 1)
#endif

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
      OS_RSEMA mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o)  OS_CREATERSEMA(&(o)->mutex)
#define ThreadMutex_destructor(o)   OS_DeleteRSema(&(o)->mutex)
#define ThreadMutex_set(o)          TST_OK(OS_Use(&(o)->mutex))
#define ThreadMutex_release(o)      OS_Unuse(&(o)->mutex)
#define ThreadMutex_isOwner(o)      (OS_GetResourceOwner(&(o)->mutex) == OS_GetpCurrentTask())


typedef struct ThreadSemaphoreBase
{
      OS_CSEMA sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) OS_CREATECSEMA(&(o)->sem)
#define ThreadSemaphore_destructor(o)  OS_DeleteCSema(&(o)->sem)
#define ThreadSemaphore_wait(o)        OS_WaitCSema(&(o)->sem)
#define ThreadSemaphore_signal(o)      OS_SignalCSema(&(o)->sem)


typedef struct ThreadBase
{
      OS_TASK task;
      int     *stack;
} ThreadBase;

#define Thread_sleep(milliseconds) OS_Delay(milliseconds)
#define Thread_start(o)       OS_Resume(&(o)->task)
#define Thread_destructor(o)  do{\
   OS_Terminate(&(o)->task);\
   baFree(&(o)->stack);\
}while(0)   

#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   struct ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
#ifdef __cplusplus
}
#endif




#endif
