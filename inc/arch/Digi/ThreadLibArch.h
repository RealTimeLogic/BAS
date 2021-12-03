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
 *   $Id: ThreadLibArch.h 109 2005-01-11 12:48:49Z  $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008
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
 * Platform: ThreadX (Digi).
 *
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp


#include <TargConfig.h>

struct ThreadSemaphoreBase;
struct ThreadMutexBase;

typedef struct ThreadMutexBase
{
      TX_THREAD* tid; /* Lock owner */
      TX_MUTEX mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o)  do{\
   memset(o,0,sizeof(ThreadMutexBase));\
   TST_OK(tx_mutex_create(&(o)->mutex, (CHAR*)"BM", TX_INHERIT));\
}while(0)

#define ThreadMutex_destructor(o)  TST_OK(tx_mutex_delete(&(o)->mutex))

#define ThreadMutex_set(o)  do{\
   TST_OK(tx_mutex_get(&(o)->mutex,TX_WAIT_FOREVER));\
   (o)->tid = tx_thread_identify();\
}while(0)

#define ThreadMutex_release(o)  do{\
   (o)->tid = 0;\
   TST_OK(tx_mutex_put(&(o)->mutex));\
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == tx_thread_identify())


typedef struct ThreadSemaphoreBase
{
      TX_SEMAPHORE sem;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o) \
   do {memset(o,0,sizeof(ThreadSemaphore));\
   TST_OK(tx_semaphore_create(&(o)->sem, (CHAR*)"BS", 0));}while(0)
#define ThreadSemaphore_destructor(o)  TST_OK(tx_semaphore_delete(&(o)->sem))
#define ThreadSemaphore_wait(o)        TST_OK(tx_semaphore_get(&(o)->sem, TX_WAIT_FOREVER))
#define ThreadSemaphore_signal(o)      TST_OK(tx_semaphore_put(&(o)->sem))


typedef struct ThreadBase
{
      TX_THREAD tid;
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
void Thread_start(ThreadBase* o);
#ifdef __cplusplus
}
#endif

#endif
