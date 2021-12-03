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
 *			      SOURCE
 *
 *   $Id: ThreadLib.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2015 - 2020
 *               http://www.realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 *
 *
 */



#include <ThreadLib.h>

#if configUSE_TIMERS != 1
#error configUSE_TIMERS must be enabled
#endif

#ifndef ESP_PLATFORM

static U32 unixTime;

static void secTimerCallback(TimerHandle_t notUsed)
{
   (void)notUsed;
   unixTime++;
}

void baSetUnixTime(BaTime time)
{
   static int started=0;
   unixTime = time;
   if(!started)
   {
      started=1;

      xTimerStart(xTimerCreate("BAS-tick",
                   1000 / portTICK_PERIOD_MS,
                   pdTRUE,
                   0,
                   secTimerCallback),0);
   }
}

BaTime baGetUnixTime(void)
{
   return unixTime;
}
#endif /* not ESP_PLATFORM */


unsigned int baGetMsClock()
{
   return xTaskGetTickCount() * portTICK_PERIOD_MS;
}


void ThreadSemaphore_wait(ThreadSemaphore* o)
{
   if(o->waitTask)
   {
      baAssert(xTaskGetCurrentTaskHandle() == o->waitTask);
   }
   else
   {
      o->waitTask = xTaskGetCurrentTaskHandle();
   }
   if(o->preWaitCount <= 0)
   {
      ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
   }
   o->preWaitCount--;
}


void ThreadSemaphore_signal(ThreadSemaphore* o)
{
   o->preWaitCount++;
   if(o->waitTask)
   {
      xTaskNotifyGive(o->waitTask);
   }
}


static void
Thread_threadStart(void* thread)
{
   while(xSemaphoreTake(((Thread*)thread)->sem, portMAX_DELAY) != pdTRUE);
   vSemaphoreDelete(((Thread*)thread)->sem);
   ((Thread*)thread)->runnable((Thread*)thread);
   Thread_destructor((Thread*)thread);
   baAssert(0);
}


void Thread_destructor(ThreadBase* o)
{
   TaskHandle_t tid = o->tid;
   if(tid)
   {
      o->tid=0;
      vTaskDelete(tid);
   }
}


#if BA_THREADNAME
void
_Thread_constructor(ThreadBase* o, Thread_Run r, ThreadPriority priority,
                    int stackSize, const char* filename)
#else
void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
#endif
{
   UBaseType_t taskPrio=4;
   
#if BA_THREADNAME
   for(;;)
   {
      const char* ptr = strchr(filename, '/');
      if(!ptr)
         break;
      filename=ptr+1;
   }
#endif
    
   switch(priority)
   {
      case ThreadPrioLowest:  taskPrio -= 2; break;
      case ThreadPrioLow:     taskPrio -= 1; break;
      case ThreadPrioNormal:                 break;
      case ThreadPrioHigh:    taskPrio += 1; break;
      case ThreadPrioHighest: taskPrio += 2; break;
      default:
         baAssert(0);
   }
   
   o->sem=xSemaphoreCreateBinary();
   if(!o->sem)
      baFatalE(FE_THREAD_LIB,__LINE__);
   o->tid=0;
   o->runnable = r;

   TST_OK(xTaskCreate(
             Thread_threadStart, 
#if BA_THREADNAME
             filename,
#else
             "BAS",
#endif
#ifdef ESP_PLATFORM
             stackSize+4000, /*  In bytes + ESP APIs need more mem */
#else
             stackSize / 4,  /* Assume it's on 32 bit arch */
#endif
             o,
             taskPrio, 
             &o->tid));
   if(!o->tid)
      baFatalE(FE_THREAD_LIB,__LINE__);
}
