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
 *   $Id: ThreadLib.cpp 1926 2009-12-23 08:27:06Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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

#ifndef NDEBUG
void
Thread_cef(BOOLEAN ok, const char* file, int line)
{
   if(!ok)
   {
      baFatalEf(FE_THREAD_LIB, smx_ct->err, file, line);
   }
}
#endif


static u32 __unixTimeBase;
void baSetUnixTime(u32 time)
{
   __unixTimeBase = time;
}
u32 baGetUnixTime(void)
{
   return sb_OS_STIME_GET() + __unixTimeBase;
}

u32 baGetClock()
{
   return sb_OS_STIME_GET();
}



static void
Thread_threadStart(void)
{
   TCB_PTR task = sb_OS_TASK_GET_CURRENT();
   Thread* thread = (Thread*)task->thisptr;
   thread->runnable(thread);
   sb_OS_TASK_DELETE(&task);
   baAssert(0);
}


void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   u8 taskPrio = sb_OS_TASK_GET_CURRENT()->pri;
   o->runnable = r;
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
   o->task=smx_TaskCreate(Thread_threadStart,taskPrio,stackSize,SMX_FL_NONE,0);
   if(!o->task)
      baFatalE(FE_THREAD_LIB,smx_errno);
   o->task->thisptr=o;
}


void
Thread_start(ThreadBase* o)
{
   smx_TaskStart(o->task);
}
