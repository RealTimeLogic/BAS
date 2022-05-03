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
 *   $Id: ThreadLibArch.h 5134 2022-04-27 22:40:57Z wini $
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
 *               https://realtimelogic.com
 ****************************************************************************
 *
 * Platform: Zephyr
 *
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>

#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/arch_interface.h>
#undef KB /* Used by Lua */

typedef struct ThreadMutexBase
{
      k_tid_t tid; /* Lock owner */
      struct k_mutex mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do{\
   (o)->tid = 0;\
   k_mutex_init(&(o)->mutex); \
}while(0)

#define ThreadMutex_destructor(o)

#define ThreadMutex_set(o)  do{\
   k_mutex_lock(&(o)->mutex, K_FOREVER); \
   (o)->tid = k_current_get();          \
}while(0)

#define ThreadMutex_release(o)  do{\
   (o)->tid = 0; \
   k_mutex_unlock(&(o)->mutex); \
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == k_current_get())

typedef struct ThreadSemaphoreBase
{
   struct k_sem sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) k_sem_init(&(o)->sem, 0, 1)

#define ThreadSemaphore_destructor(o)

#define ThreadSemaphore_wait(o) k_sem_take(&(o)->sem, K_FOREVER)

#define ThreadSemaphore_signal(o) k_sem_give(&(o)->sem)


typedef struct ThreadBase
{
   struct k_thread thread;
   Thread_Run runnable;
   struct k_sem* startSem;
   k_thread_stack_t* stackStart;
   k_tid_t tid;
} ThreadBase; 

#ifdef __cplusplus
extern "C" {
#endif

void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
void Thread_destructor(ThreadBase* o);
#define Thread_sleep(milliseconds) k_msleep(milliseconds)
#define Thread_start(o) k_sem_give((o)->startSem)
#ifdef __cplusplus
}
#endif

#endif
