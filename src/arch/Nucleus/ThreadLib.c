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
 *   $Id: ThreadLib.c 5146 2022-05-10 17:15:54Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2014
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
 *               http://realtimelogic.com
 ****************************************************************************
 *
 *
 */

#include <ThreadLib.h>
#include <networking/target.h>

#ifndef BAS_BASE_PRIO
#define BAS_BASE_PRIO 126
#endif

extern volatile UNSIGNED TMD_System_Clock;

static U32 unixTimeBase;

void
baSetUnixTime(U32 time)
{
   unixTimeBase = time;
}


U32
baGetUnixTime(void)
{ 
   return unixTimeBase + (U32)(NU_Retrieve_Clock64()/(U64)NU_TICKS_PER_SECOND);
}


U32
baGetMsClock(void)
{
   return (U32)((U64)TMD_System_Clock * (U64)1000 /
                (U64)NU_TICKS_PER_SECOND);
}


U32
baMsTime2TxTicks(U32 msec)
{
   U32 sec;
    /* Make sure msec * 1000 does not wrap on 32 bit.
     * Assuming that NU_TICKS_PER_SECOND <= 1000
     */
   if(msec < 4294967)
      return msec * NU_TICKS_PER_SECOND / 1000;

   sec = msec / 1000;
   msec = msec % 1000;
   return sec * NU_TICKS_PER_SECOND +
      msec * NU_TICKS_PER_SECOND / 1000;
}


int
ThreadMutex_isOwner(ThreadMutex* o)
{
   NU_TASK             *task; 
   TST_OK(NU_Get_Semaphore_Owner(&o->mutex, &task));
   return NU_Current_Task_Pointer() == task;
}


void
Thread_sleep(U32 milliseconds)
{
   NU_Sleep(milliseconds * NU_TICKS_PER_SECOND / 1000);
}


static VOID
Thread_threadStart(UNSIGNED argc, VOID *argv)
{
   ((Thread*)argv)->runnable((Thread*)argv);
   Thread_destructor((Thread*)argv);
}



void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   NU_MEMORY_POOL *    memPool;
   UINT prio = BAS_BASE_PRIO;
   memset(o, 0, sizeof(ThreadBase));
   o->runnable = r;
   switch(priority)
   {
      case ThreadPrioLowest:  prio += 2; break;
      case ThreadPrioLow:     prio += 1; break;
      case ThreadPrioNormal:             break;
      case ThreadPrioHigh:    prio -= 1; break;
      case ThreadPrioHighest: prio -= 2; break;
      default:
         baAssert(0);
   }
   o->stackStart=0;
   TST_OK(NU_System_Memory_Get(&memPool, NU_NULL));
   TST_OK(NU_Allocate_Memory(memPool, &o->stackStart, stackSize, NU_SUSPEND));
   TST_OK(NU_Create_Task(&o->task,
                         "BAS",
                         Thread_threadStart, 
                         1, o,
                         o->stackStart,
                         stackSize,
                         prio,
                         0,
                         NU_PREEMPT,
                         NU_NO_START));
}


typedef struct
{
   ThreadBase* t;
} ThreadCont;

static NU_QUEUE tdQueue;
static U8 queueState;
static VOID fixPoorNucleusResourceManagement(UNSIGNED argc, VOID *argv);

void
Thread_destructor(struct ThreadBase* o)
{
   if(o->stackStart)
   {
      ThreadCont tc;
      ESAL_AR_INT_CONTROL_VARS;

      tc.t = o;
      if(NU_Current_Task_Pointer() == &o->task)
      {
         while(queueState == 1) NU_Sleep(2);
         ESAL_GE_INT_ALL_DISABLE();
         if( queueState == 0 )
         {
            NU_MEMORY_POOL* memPool;
            VOID* pointer;
            VOID* stack;
            queueState=1;
            ESAL_GE_INT_ALL_RESTORE();
            TST_OK(NU_System_Memory_Get(&memPool, NU_NULL));
            TST_OK(NU_Allocate_Memory(memPool, 
                                      &pointer, 
                                      sizeof(ThreadCont),
                                      NU_SUSPEND));
            TST_OK(NU_Create_Queue(&tdQueue, 
                                   "bas", 
                                   pointer, 
                                   sizeof(ThreadCont)/sizeof(UNSIGNED),
                                   NU_FIXED_SIZE, 
                                   sizeof(ThreadCont)/sizeof(UNSIGNED),
                                   NU_FIFO));
            TST_OK(NU_Allocate_Memory(memPool, &stack, 1000, NU_SUSPEND));
            TST_OK(NU_Create_Task(&o->task,
                                  "NFIX",
                                  fixPoorNucleusResourceManagement, 
                                  0, 0,
                                  stack,
                                  1000,
                                  BAS_BASE_PRIO-3,
                                  0,
                                  NU_PREEMPT,
                                  NU_START));
            queueState = 2;
         }
         else
         {
            ESAL_GE_INT_ALL_RESTORE();
         }
         TST_OK(NU_Send_To_Queue(&tdQueue,
                                 &tc,
                                 sizeof(ThreadCont)/sizeof(UNSIGNED),
                                 NU_SUSPEND));
      }
      NU_Terminate_Task(&o->task);
      TST_OK(NU_Delete_Task(&o->task));
      TST_OK(NU_Deallocate_Memory(o->stackStart));
      o->stackStart=0;
   }
}



static VOID
fixPoorNucleusResourceManagement(UNSIGNED argc, VOID *argv)
{
   for(;;)
   {
      UNSIGNED size=0;
      ThreadCont tc;
      TST_OK(NU_Receive_From_Queue(&tdQueue,
                                   &tc,
                                   sizeof(ThreadCont)/sizeof(UNSIGNED),
                                   &size, 
                                   NU_SUSPEND));
      NU_Terminate_Task(&tc.t->task); 
      TST_OK(NU_Delete_Task(&tc.t->task)); 
      TST_OK(NU_Deallocate_Memory(tc.t->stackStart));
      tc.t->stackStart=0;
   }
}
