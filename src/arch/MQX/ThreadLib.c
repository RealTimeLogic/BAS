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
 *   $Id: ThreadLib.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2014
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

#include <ThreadLib.h>
#include <HttpTrace.h>
#include <psp.h>
#include <mem_prv.h>
#include <mqx_prv.h>

#ifdef USE_MQX_MALLOC
void*
baRealloc(void* ptr, unsigned long size)
{
   void* newPtr = _mem_alloc_system(size);
   if(newPtr)
   {
      memcpy(newPtr, ptr, size);
      _mem_free(ptr);
   }
   return newPtr;
}
#endif

static void
Thread_runnable(uint_32 thread)
{
   Thread* o = (Thread*)thread;
   baAssert(o &&
          o->tid == _task_get_id() &&
          o->runnable);
   if(o)
      o->runnable(o);
   _task_destroy(_task_get_id());
}


void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority tp, int stackSize)
{
   TASK_TEMPLATE_STRUCT tts;
   _mqx_uint mqxPrio;
   o->runnable = r;
   o->tid = MQX_NULL_TASK_ID;
   TST_OK(_task_get_priority(_task_get_id(),&mqxPrio));
   switch(tp)
   {
      case ThreadPrioLowest:  mqxPrio +=  2; break;
      case ThreadPrioLow:     mqxPrio +=  1; break;
      case ThreadPrioNormal:                 break;
      case ThreadPrioHigh:    mqxPrio -= 1; break;
      case ThreadPrioHighest: mqxPrio -= 2; break;
   }

   memset(&tts, 0, sizeof(TASK_TEMPLATE_STRUCT));
   tts.TASK_TEMPLATE_INDEX = 0;
   tts.TASK_ADDRESS       = Thread_runnable;
   tts.TASK_STACKSIZE     = stackSize;
   tts.TASK_PRIORITY      = mqxPrio;
   tts.TASK_NAME          = "BT";
   tts.TASK_ATTRIBUTES = 0;
   tts.CREATION_PARAMETER = (uint_32)o;
   tts.DEFAULT_TIME_SLICE = 0;

   o->tid = _task_create_blocked(0, 0, (uint_32)&tts);
   if(o->tid == MQX_NULL_TASK_ID)
   {
      TRPR(("Thread::Thread, task_create failed: %d",_task_get_error()));
      return;
   }
}

int
Thread_start(struct Thread* o)
{
   if(o->tid != MQX_NULL_TASK_ID)
   {
      TD_STRUCT_PTR td = _task_get_td(o->tid);
      baAssert(td);
      baAssert(td->STATE == BLOCKED);
      _task_ready(td);
      return 0;
   }
   return -1;
}

U32
baGetUnixTime(void)
{
   TIME_STRUCT t;
   _time_get(&t);
   return t.SECONDS;
}


U32
baGetMsClock(void)
{
   TIME_STRUCT t;
   _time_get_elapsed(&t);
   /* This will wrap, but that is OK */
   return t.SECONDS*1000 + t.MILLISECONDS;
}
