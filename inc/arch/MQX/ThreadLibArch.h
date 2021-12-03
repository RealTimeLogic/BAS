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
 *   $Author$
 *   $Date$
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

#include <mutex.h>
#include <TargConfig.h>
#include <stddef.h>

#ifdef NDEBUG
#define TST_OK(x) x
#else
#define TST_OK(x) baAssert(MQX_OK == x)
#endif

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
      _task_id tid;  /* Lock owner */
      MUTEX_STRUCT mutex;
} ThreadMutexBase;

#define  ThreadMutex_constructor(o)  do{\
   (o)->tid = MQX_NULL_TASK_ID;\
   TST_OK(_mutex_init(&(o)->mutex,0));\
}while(0)

#define ThreadMutex_destructor(o) TST_OK(_mutex_destroy(&(o)->mutex))

#define ThreadMutex_set(o)  do{\
   TST_OK(_mutex_lock(&(o)->mutex));\
   (o)->tid = _task_get_id();\
}while(0)

#define ThreadMutex_release(o)  do{\
   (o)->tid = MQX_NULL_TASK_ID;\
   TST_OK(_mutex_unlock(&(o)->mutex));\
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == _task_get_id())


typedef struct ThreadSemaphoreBase
{
      LWSEM_STRUCT sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) \
   TST_OK(_lwsem_create(&(o)->sem,0))
#define ThreadSemaphore_destructor(o) \
   TST_OK(_lwsem_destroy(&(o)->sem))
#define ThreadSemaphore_wait(o) \
   TST_OK(_lwsem_wait(&(o)->sem))
#define ThreadSemaphore_signal(o) \
   TST_OK(_lwsem_post(&(o)->sem))


typedef struct ThreadBase
{
      _task_id tid;
      Thread_Run runnable;
} ThreadBase;

#define Thread_sleep(milliseconds) _time_delay(milliseconds)
#define Thread_destructor(o)

#ifdef __cplusplus
extern "C" {
#endif
void Thread_constructor(
   struct ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
int Thread_start(struct ThreadBase* o);
#ifdef __cplusplus
}
#endif




#endif
