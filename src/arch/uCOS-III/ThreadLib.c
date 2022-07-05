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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic, 2022
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

#ifndef BA_THREAD_PRIO_BASE
#define BA_THREAD_PRIO_BASE 12
#endif

#ifndef BA_OS_MSG_QTY
#define BA_OS_MSG_QTY 5
#endif

#include <ThreadLib.h>
#include <string.h>


BaTime
baGetUnixTime(void)
{
   return (baGetMsClock() / 1000);
}


BaTime
baGetMsClock()
{
   OS_ERR err;
   OS_TICK num_ticks = OSTimeGet(&err);

   (void) err;
   return (num_ticks);
}


static void
wait4ThreadStart(void)
{
   OS_ERR  err;
   OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, 0, &err);
   TST_OK(err);
}


static void
Thread_threadStart(void* thread)
{
   wait4ThreadStart();
   ((Thread*)thread)->runnable((Thread*)thread);
   Thread_destructor((Thread*)thread);
}


void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   CPU_STK* stk;
   OS_ERR err;
   OS_PRIO taskPrio=BA_THREAD_PRIO_BASE;
   switch(priority)
   {
      case ThreadPrioLowest:  taskPrio += 2; break;
      case ThreadPrioLow:     taskPrio += 1; break;
      case ThreadPrioNormal:                 break;
      case ThreadPrioHigh:    taskPrio -= 1; break;
      case ThreadPrioHighest: taskPrio -= 2; break;
      default:
         baAssert(0);
   }
   o->runnable = r;
   stackSize+=100;
   stk = (CPU_STK*)baMalloc(stackSize);
   baAssert(0 != stk);

   stackSize /= sizeof (CPU_STK);       /* Stack size, in elements (uints) */
   OSTaskCreate(&o->tid,
                "BAS",
                Thread_threadStart,
                o,
                taskPrio,
                stk,
                stackSize / 8,
                stackSize,
                BA_OS_MSG_QTY,
                0,
                0,
                OS_OPT_TASK_STK_CHK + OS_OPT_TASK_STK_CLR,
                &err);
   TST_OK(err);
}
