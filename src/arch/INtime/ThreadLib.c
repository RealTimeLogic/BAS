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
 *   $Id$
 *
 *   COPYRIGHT:  Real Time Logic, 2020
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

#ifndef BA_LIB
#define BA_LIB 1
#endif

#ifndef BA_THREAD_PRIO_OFFS
#define BA_THREAD_PRIO_OFFS 200
#endif

#include <ThreadLib.h>
#include <BaErrorCodes.h>

void
_baClckGettime(BaTimeEx* spec)
{
   SYSTEMTIME st;
   GetRtSystemTime(&st);
   spec->sec = st.wSecond;
   spec->nsec = st.dwMicroseconds*1000;
   spec->offset = 0;
}


BA_API U32
baGetMsClock(void)
{
   SYSTEMTIME st;
   GetRtSystemTime(&st);
   return 1000*st.wSecond + st.dwMicroseconds/1000;
}


#ifndef NDEBUG
BA_API void
Thread_cef(BOOLEAN ok, const char* file, int line)
{
   if( ! ok ) baFatalEf(FE_THREAD_LIB, 0, file, line);
}
BA_API RTHANDLE
Thread_hndlf(RTHANDLE rth, const char* file, int line)
{
   if(BAD_RTHANDLE == rth) baFatalEf(FE_THREAD_LIB, 0, file, line);
   return rth;
}
#endif


static void
Thread_threadStart(void* self)
{
   Thread* thread = (Thread*)self;
   ThreadSemaphore_wait(&thread->startSem);
   ThreadSemaphore_destructor(&thread->startSem);
   thread->runnable(thread);
   DeleteRtThread(thread->tid);
}


BA_API void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   BYTE prio;
   ThreadSemaphore_constructor(&o->startSem);
   o->runnable = r;
   switch(priority)
   {
      case ThreadPrioLowest:  prio = BA_THREAD_PRIO_OFFS +2; break;
      case ThreadPrioLow:     prio = BA_THREAD_PRIO_OFFS +1; break;
      case ThreadPrioHigh:    prio = BA_THREAD_PRIO_OFFS -1; break;
      case ThreadPrioHighest: prio = BA_THREAD_PRIO_OFFS -2; break;
      default:                prio = BA_THREAD_PRIO_OFFS;     break;
   }
   o->tid = Thread_hndl(CreateRtThread(prio,Thread_threadStart,stackSize,o));
}


BA_API void
Thread_start(Thread* o)
{
   ThreadSemaphore_signal(&o->startSem);
}
