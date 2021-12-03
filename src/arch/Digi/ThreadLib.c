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
 *   $Id: ThreadLib.c 171 2005-04-28 00:03:13Z  $
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
 *
 */

#include <ThreadLib.h>
#include <tx_api.h>
#include <bsp_api.h>
#include <time.h>


static void Thread_terminate(struct ThreadBase* o);


/****************************************************************************
 *                         Thread termination handling
 ***************************************************************************/


#define TERM_QUEUE_LEN 3
U8 termQueueBuf[TERM_QUEUE_LEN*sizeof(void*)];
static int termThreadStarted=0;
static TX_QUEUE termThreadQueue;
static struct ThreadBase termThread;


/* This function will never be called.
   This code verifies that the correct macros for NetOS is set.
   The macros are incorrect if you get compile errors.

   Note, this code is to verify that BA for NetOS for 7.4.2 and newer
   is correct.
   Otherwise, remove the code.
*/
static void*
compilerThreadXTest(TX_THREAD* tx)
{
   return &tx->tx_gnu_clib_reent;
}

static void
termThreadFunc(struct ThreadBase* o)
{
   (void)o;
   for(;;)
   {
      ThreadBase* t;
      TST_OK(tx_queue_receive(&termThreadQueue, &t, TX_WAIT_FOREVER));
      Thread_terminate(t);
      compilerThreadXTest(0);
   }
}


static void
initThreadTerm()
{
   termThreadStarted=1;
   /* queue messages are pointers */
   TST_OK(tx_queue_create(&termThreadQueue, "BaThreadTerm",TX_1_ULONG,
                          termQueueBuf, sizeof(termQueueBuf)));
   Thread_constructor(&termThread, termThreadFunc, ThreadPrioNormal, 1000);
   Thread_start(&termThread);
}



/****************************************************************************
 *                         Thread
 ***************************************************************************/

static VOID
Thread_threadStart(ULONG self)
{
   Thread* o = (Thread*)self;
   o->runnable(o);
   Thread_destructor(o);
   baAssert(0);
}


void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   UINT txPrio;

   UINT p = tx_interrupt_control(TX_INT_DISABLE);
   if(!termThreadStarted) initThreadTerm();
   tx_interrupt_control(p);

   txPrio = NATranslatePriorityLevel(naDefaultPriority);
   memset(o, 0, sizeof(ThreadBase));
   o->runnable = r;
   switch(priority)
   {
      case ThreadPrioLowest:  txPrio -= 2; break;
      case ThreadPrioLow:     txPrio -= 1; break;
      case ThreadPrioNormal:               break;
      case ThreadPrioHigh:    txPrio += 1; break;
      case ThreadPrioHighest: txPrio += 2; break;
      default:
         baAssert(0);
   }
   stackSize += TX_MINIMUM_STACK;
   o->stackStart = baMalloc(stackSize);
   if(o->stackStart)
   {
      TST_OK(tx_thread_create(&o->tid,
                              "BTh",
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


static void
Thread_terminate(struct ThreadBase* o)
{
   VOID* stackStart = o->stackStart;
   o->stackStart=0;
   tx_thread_terminate(&o->tid);
   TST_OK(tx_thread_delete(&o->tid));
   baFree(stackStart);
}


void
Thread_destructor(struct ThreadBase* o)
{
   TX_THREAD* self = tx_thread_identify();
   if(self && o->stackStart)
   {
      /* If the running thread is terminating itself */
      if(self->tx_thread_id == o->tid.tx_thread_id)
      {
         TST_OK(tx_queue_send(&termThreadQueue, &o, TX_WAIT_FOREVER));
         tx_thread_terminate(&o->tid);
      }
      else /* Terminating another thread */
      {
         Thread_terminate(o);
      }
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
Thread_sleep(U32 milliseconds)
{
   tx_thread_sleep(milliseconds * NABspTicksPerSecond / 1000);
}


U32
baGetUnixTime(void)
{
   return time(0);
}
