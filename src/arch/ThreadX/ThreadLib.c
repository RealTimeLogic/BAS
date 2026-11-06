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
 *   $Id: ThreadLib.c 6760 2026-11-06 00:03:47Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2009 - 2026
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
#include <nx_system.h>
#include <tx_timer.h>
#include <tx_thread.h>

#ifdef NOT_ATOMIC_32
#error Wrong_CPU. Designed_for_32_bit_microprocessors.
#endif


/****************************************************************************
 *                         Memory handling
 ***************************************************************************/
 

/****************************************************************************
 *                         Time functions
 ***************************************************************************/

static volatile U32 unixTime;

static VOID
oneSecondTimer(ULONG notUsed)
{
   (void)notUsed;
   unixTime++;
}


void
baSetUnixTime(U32 time)
{
   static TX_TIMER timer;
   static BaBool initialized;
   BaBool createTimer = FALSE;
   TX_INTERRUPT_SAVE_AREA;
   TX_DISABLE;
   unixTime = time;
   if(!initialized)
   {
      initialized = TRUE;
      createTimer = TRUE;
   }
   TX_RESTORE;
   if(createTimer)
   {
      TST_OK(tx_timer_create(&timer,
                            "B sec tick",
                            oneSecondTimer,
                            0,
                            NX_IP_PERIODIC_RATE,
                            NX_IP_PERIODIC_RATE,
                            TX_AUTO_ACTIVATE));
   }
}


BaTime
baGetUnixTime(void)
{ 
   return unixTime;
}


U32
baGetMsClock(void)
{
   return (U32)((U64)tx_time_get() * (U64)1000 /
                (U64)NX_IP_PERIODIC_RATE);
}


U32
baMsTime2TxTicks(U32 msec)
{
   U64 ticks = ((U64)msec * NX_IP_PERIODIC_RATE + 999) / 1000;
   /* A positive timeout must not become a poll or TX_WAIT_FOREVER. */
   return ticks >= 0xFFFFFFFFU ? 0xFFFFFFFEU : (U32)ticks;
}


/****************************************************************************
 *                         Thread
 ***************************************************************************/

static VOID
Thread_threadStart(ULONG self)
{
   Thread* o = (Thread*)self;
   o->runnable(o);
   /* Returning lets ThreadX mark the thread completed for its owner to delete. */
}


void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   UINT txPrio = 16;
   memset(o, 0, sizeof(ThreadBase));
   o->runnable = r;
   switch(priority)
   {
      case ThreadPrioLowest:
         txPrio += 2;
         break;
      case ThreadPrioLow:
         txPrio += 1;
         break;
      case ThreadPrioNormal:
         break;
      case ThreadPrioHigh:
         txPrio -= 1;
         break;
      case ThreadPrioHighest:
         txPrio -= 2;
         break;
      default:
         baAssert(0);
   }
   stackSize += TX_MINIMUM_STACK;
   o->stackStart = baMalloc(stackSize);
   if(o->stackStart)
   {
      TST_OK(tx_thread_create(&o->tid,
                              "BAS",
                              Thread_threadStart,
                              (ULONG)o,
                              o->stackStart,
                              stackSize,
                              txPrio,
                              txPrio,
                              TX_NO_TIME_SLICE,
                              TX_DONT_START));
   }
}


void
Thread_destructor(struct ThreadBase* o)
{
   if(o->stackStart)
   {
      TX_THREAD* self;
      VOID* stackStart = o->stackStart;
      TX_THREAD_GET_CURRENT(self);
      if(self == &o->tid)
         baFatalE(FE_THREAD_LIB,o->tid.tx_thread_id);
      o->stackStart=0;
      tx_thread_terminate(&o->tid);
      TST_OK(tx_thread_delete(&o->tid));
      baFree(stackStart);
   }
}


void
Thread_start(ThreadBase* o)
{
   if(o->stackStart)
   {
      TST_OK(tx_thread_resume(&o->tid));
   }
}


void
Thread_sleep(U32 msec)
{
   tx_thread_sleep(baMsTime2TxTicks(msec));
}
