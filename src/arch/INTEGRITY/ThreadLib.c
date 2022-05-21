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
 *   $Id: ThreadLib.c 5146 2022-05-10 17:15:54Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2011
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


#define BA_MIN_TASK_PRIO 10

static U32 timeBase;


/* Should be called at system startup by code using BA.
   Argument 'time' is the time since the Epoch (00:00:00 UTC, January 1,
   1970), measured in seconds.
*/
void
baSetUnixTime(U32 time)
{
  timeBase=time;
}


U32 baGetUnixTime(void)
{
  Time t;
  Thread_ce(GetClockTime(HighestResStandardClock, &t));
  return timeBase + t.Seconds;
}


U32 baGetMsClock(void)
{
  Time t;
  Thread_ce(GetClockTime(HighestResStandardClock, &t));

  /*
    Info from Joe:

    The Fraction portion of the Time structure is a UINT4 that reports
    back the fraction of the current second that has expired, but it
    is reported in hex.  Kind of painful, I agree, but it is done this
    way for speed.  So, 0xffffffff is 999.99999ms.  0x80000000 is
    500ms, and so on.  After a quick bit of math, I calculate 1ms to
    be 0x00418937.  So you can just divide the fraction by that number
    to give you the number of milliseconds.
  */

  return t.Seconds * 1000 + t.Fraction/0x00418937; /* wraparound OK */
}



void
Thread_cef(int status, const char* file, int line)
{
   if(status)
   {
      baFatalEf(FE_THREAD_LIB, status, file, line);
   }
}


int
ThreadMutex_isOwner(ThreadMutex* o)
{
   Value tid;
   Thread_ce(GetTaskUniqueId(CurrentTask(), &tid));
   return o->tid == tid;
}


static Thread*
getThread(void)
{
   Address arg;
   Thread_ce(GetTaskIdentification(CurrentTask(),&arg));
   return (Thread*)arg;
}


static void
Thread_threadStart(void)
{
   Thread* o = getThread();
   o->runnable(o);
   Thread_ce(ExitTask(o->task, 0));
}


void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   Value tprio;
   o->runnable = r;

   switch(priority)
   {
      case ThreadPrioLow:     tprio = BA_MIN_TASK_PRIO+1; break;
      case ThreadPrioNormal:  tprio = BA_MIN_TASK_PRIO+2; break;
      case ThreadPrioHigh:    tprio = BA_MIN_TASK_PRIO+3; break;
      case ThreadPrioHighest: tprio = BA_MIN_TASK_PRIO+4; break;

      case ThreadPrioLowest:
      default:
         tprio = BA_MIN_TASK_PRIO;
   }


   Thread_ce(CommonCreateTask(
                tprio,(Address)Thread_threadStart,stackSize,"BA",&o->task));
   SetTaskIdentification(o->task, (Address)o);
}

