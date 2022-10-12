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
 *   $Id: ThreadLibArch.h 5293 2022-10-12 10:34:41Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2015 - 2022
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
 * Platform: FreeRTOS.
 *
 */

#ifndef ThreadLib_hpp
#define ThreadLib_hpp

#include <TargConfig.h>
#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#else
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#endif



typedef struct ThreadMutexBase
{
      TaskHandle_t tid; /* Lock owner */
      SemaphoreHandle_t mutex;
} ThreadMutexBase;

#define ThreadMutex_constructor(o) do{\
   (o)->tid = 0;\
   (o)->mutex=xSemaphoreCreateMutex(); \
   baAssert((o)->mutex != 0);\
}while(0)

#define ThreadMutex_destructor(o)  vSemaphoreDelete((o)->mutex)

#define ThreadMutex_set(o)  do{\
   TST_OK(xSemaphoreTake((o)->mutex,portMAX_DELAY));\
   (o)->tid = xTaskGetCurrentTaskHandle();          \
}while(0)

#define ThreadMutex_release(o)  do{\
   (o)->tid = 0;\
   TST_OK(xSemaphoreGive((o)->mutex));\
}while(0)

#define ThreadMutex_isOwner(o) ((o)->tid == xTaskGetCurrentTaskHandle())


typedef struct ThreadSemaphoreBase
{
   TaskHandle_t waitTask;
   int preWaitCount;
} ThreadSemaphoreBase;

#define ThreadSemaphore_constructor(o)  do{\
   (o)->waitTask=0;                        \
   (o)->preWaitCount=0;                    \
}while(0)

#define ThreadSemaphore_destructor(o)

#ifdef __cplusplus
extern "C" {
#endif
void ThreadSemaphore_wait(ThreadSemaphoreBase*);
void ThreadSemaphore_signal(ThreadSemaphoreBase*);
void ThreadSemaphore_signal_from_isr(ThreadSemaphoreBase*);
#ifdef __cplusplus
}
#endif 

typedef struct ThreadBase
{
   Thread_Run runnable;
   TaskHandle_t tid;
   SemaphoreHandle_t sem;
} ThreadBase;

#ifdef NDEBUG
#ifndef BA_THREADNAME
#define BA_THREADNAME 0
#endif
#else
#ifndef BA_THREADNAME
#define BA_THREADNAME 1
#endif
#endif

#if configUSE_TIMERS
#ifndef BA_TIMER_EXT_TICK
#define BA_TIMER_EXT_TICK 1
#endif
#endif

#if BA_TIMER_EXT_TICK
#define BaTimer_installTrigger(callback,ticklenInMs)                    \
   TST_OK(xTimerStart(xTimerCreate("BaTimer",                           \
                                   pdMS_TO_TICKS(ticklenInMs),pdTRUE,   \
                                   0,(TimerCallbackFunction_t)callback), 0))
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if BA_THREADNAME
#define Thread_constructor(o, r, priority, stackSize)           \
   _Thread_constructor(o, r, priority, stackSize, __FILE__)
void _Thread_constructor(ThreadBase* o, Thread_Run r, ThreadPriority priority,
                         int stackSize, const char*);
#else
void Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize);
#endif

void Thread_destructor(ThreadBase* o);
#define Thread_sleep(milliseconds) vTaskDelay(milliseconds/portTICK_PERIOD_MS)
#define Thread_start(o) TST_OK(xSemaphoreGive((o)->sem))
#ifdef __cplusplus
}
#endif

#endif
