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
 *   COPYRIGHT:  Real Time Logic, 2014
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
 * Platform: Nucleus.
 *
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase {
   NU_SEMAPHORE mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do{                                  \
      memset(o, 0, sizeof(ThreadMutexBase));                            \
      TST_OK(NU_Create_Semaphore(&(o)->mutex, "BAS-M", 1, NU_PRIORITY_INHERIT)); \
   }while(0)
#define ThreadMutex_destructor(o)  TST_OK(NU_Delete_Semaphore(&(o)->mutex))
#define ThreadMutex_set(o)  TST_OK(NU_Obtain_Semaphore(&(o)->mutex, NU_SUSPEND))
#define ThreadMutex_release(o)  NU_Release_Semaphore(&(o)->mutex)
#ifdef __cplusplus
extern "C"
#endif
int ThreadMutex_isOwner(ThreadMutex* o);

typedef struct ThreadSemaphoreBase {
   NU_SEMAPHORE sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) do{                              \
      memset(o, 0, sizeof(ThreadMutexBase));                            \
      TST_OK(NU_Create_Semaphore(&(o)->sem, "BAS-S", 0, NU_PRIORITY_INHERIT));  \
   }while(0)
#define ThreadSemaphore_destructor(o)  TST_OK(NU_Delete_Semaphore(&(o)->sem))
#define ThreadSemaphore_wait(o) TST_OK(NU_Obtain_Semaphore(&(o)->sem, NU_SUSPEND))
#define ThreadSemaphore_signal(o) TST_OK(NU_Release_Semaphore(&(o)->sem))


typedef struct ThreadBase
{
   NU_TASK task;
   VOID* stackStart;
   Thread_Run runnable;
} ThreadBase;

#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
void Thread_destructor(ThreadBase* o);
void Thread_sleep(U32 milliseconds);
#define Thread_start(o) TST_OK(NU_Resume_Task(&(o)->task))
#ifdef __cplusplus
}
#endif

#endif
