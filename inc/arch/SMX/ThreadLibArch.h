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


#ifdef NDEBUG
#define Thread_ce(x) x
#else
#ifdef __cplusplus
extern "C" {
#endif
void Thread_cef(BOOLEAN ok, const char* file, int line);
#ifdef __cplusplus
}
#endif
#define Thread_ce(x) Thread_cef(x, __FILE__, __LINE__)
#endif


struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadSemaphoreBase
{
      SCB_PTR sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) do {    \
      (o)->sem=smx_SemCreate(0,0,0);           \
      baAssert((o)->sem);                      \
} while(0)
#define ThreadSemaphore_destructor(o) Thread_ce(smx_SemDelete(&(o)->sem))
#define ThreadSemaphore_wait(o) Thread_ce(smx_SemTest((o)->sem,SMX_TMO_INF))
#define ThreadSemaphore_signal(o) Thread_ce(smx_SemSignal((o)->sem)) 





typedef struct ThreadMutexBase
{
   TCB_PTR tid;
   MUCB_PTR mutex;
} ThreadMutexBase;


#define ThreadMutex_constructor(o)  do {      \
   (o)->mutex = smx_MutexCreate(1,0,0);       \
   baAssert((o)->mutex);                      \
} while(0)

#define ThreadMutex_destructor(o) Thread_ce(smx_MutexDelete(&(o)->mutex))

#define ThreadMutex_set(o) do {                                 \
      Thread_ce(smx_MutexGet((o)->mutex,SMX_TMO_INF));          \
      (o)->tid=sb_OS_TASK_GET_CURRENT();                        \
   } while(0)

#define ThreadMutex_release(o) do{              \
      (o)->tid=0;                               \
      Thread_ce(smx_MutexRelease((o)->mutex));  \
   } while(0)

#define ThreadMutex_isOwner(o) sb_OS_TASK_IS_CURRENT((o)->tid)



typedef struct ThreadBase
{
      Thread_Run runnable;
      TCB_PTR task;
} ThreadBase;

#define Thread_destructor(o)

#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
void Thread_start(ThreadBase* o);
#ifdef __cplusplus
}
#endif

#define Thread_sleep(milliseconds) sb_OS_WAIT_MSEC_MT(milliseconds)


#endif
