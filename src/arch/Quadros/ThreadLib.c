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
 *			      
 *
 *   $Id: ThreadLib.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2012
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

/*  3 <= BA_BASE_PRIO <= 124 */
#define BA_BASE_PRIO 51


#include <ThreadLib.h>


/****************************************************************************
 *                         Thread
 ***************************************************************************/

static void
Thread_threadStart(void)
{
   Thread* o = (Thread*)KS_GetTaskEnvArg(KS_GetTaskID());
   o->runnable(o);
   KS_TerminateSelfTask();
}


void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   o->tprop.priority = BA_BASE_PRIO;
   o->runnable = r;
   Thread_ce(KS_OpenTask(0, &o->tid));
   switch(priority)
   {
      case ThreadPrioLowest:  o->tprop.priority += 2; break;
      case ThreadPrioLow:     o->tprop.priority += 1; break;
      case ThreadPrioNormal:                          break;
      case ThreadPrioHigh:    o->tprop.priority -= 1; break;
      case ThreadPrioHighest: o->tprop.priority -= 2; break;
      default:
         baAssert(0);
   }
   o->tprop.stackbase = (char *)baMalloc(stackSize);
   if(o->tprop.stackbase)
   {
      o->tprop.taskentry = ( void(*)(void) )Thread_threadStart;
      o->tprop.stacksize = stackSize;
      KS_DefTaskProp(o->tid, &o->tprop);
      KS_DefTaskEnvArg(o->tid, o);
   }
}


void
Thread_destructor(struct ThreadBase* o)
{
   if(o->tprop.stackbase)
   { 
      void* stackbase = o->tprop.stackbase;
      if(KS_GetTaskID() == o->tid)
         baFatalE(FE_THREAD_LIB,o->tid);
      o->tprop.stackbase=0;
      Thread_ce(KS_CloseTask(o->tid));
      baFree(stackbase);       
   }
}
