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
#include <time.h> /* For some strange reason: compile error if not included */
#include <ThreadLib.h>
#include <HttpTrace.h>

void
Thread_constructor(Thread* o, Thread_Run r, ThreadPriority tp, int stackSize)
{
   unsigned char prio;

   prio = OS_GetPriority(OS_GetpCurrentTask());
   switch (tp)
   {
      case ThreadPrioLowest:  prio += 2; break;
      case ThreadPrioLow:     prio += 1; break;
      case ThreadPrioNormal:  prio += 0; break;
      case ThreadPrioHigh:    prio -= 1; break;
      case ThreadPrioHighest: prio -= 2; break;
   }

   o->stack = baMalloc(sizeof(int) * ((stackSize + sizeof(int) - 1) / sizeof(int)));
   if(o->stack == NULL)
   {
      TRPR(("Thread::Thread, stack creation failed"));
      return;
   }
   OS_EnterRegion();
   OS_CreateTaskEx(&(o)->task, "thread", prio, (void(*)(void*))r, o->stack, stackSize, 0, (void*)o);
   OS_Suspend(&(o)->task);
   OS_LeaveRegion();
}


U32
baGetUnixTime(void)
{
   return (U32)OS_GetTime32() / 1000;  /* one tick each ms */
}


U32
baGetMsClock(void)
{
   return (U32)OS_GetTime32();  
}
