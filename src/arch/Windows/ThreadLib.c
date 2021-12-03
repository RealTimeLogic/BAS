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
 *   $Id: ThreadLib.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2020
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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>
#include <ThreadLib.h>
#include <process.h>

void
_baClckGettime(BaTimeEx* spec)
{
   FILETIME ft;
   ULARGE_INTEGER wintime;
   GetSystemTimeAsFileTime(&ft);
   wintime.LowPart  = ft.dwLowDateTime;
   wintime.HighPart = ft.dwHighDateTime;
   wintime.QuadPart -= 116444736000000000i64;  /* 1jan1601 to 1jan1970 */
   spec->sec = wintime.QuadPart / 10000000i64; /* seconds */
   spec->nsec = wintime.QuadPart % 10000000i64 *100; /* nano-seconds */
   spec->offset=0;
}


static unsigned __stdcall Thread_threadStart(void* self)
{
   Thread* thread = (Thread*)self;
   thread->runnable(thread);
   return 0;
}

BA_API void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
  unsigned threadid;
  HANDLE h;
  int prioNT;

  switch(priority) {
    case ThreadPrioLowest:  prioNT = THREAD_PRIORITY_LOWEST; break;
    case ThreadPrioLow:     prioNT = THREAD_PRIORITY_BELOW_NORMAL; break;
    case ThreadPrioNormal:  prioNT = THREAD_PRIORITY_NORMAL; break;
    case ThreadPrioHigh:    prioNT = THREAD_PRIORITY_ABOVE_NORMAL; break;
    case ThreadPrioHighest: prioNT = THREAD_PRIORITY_HIGHEST; break;
    default: baAssert(0); prioNT = THREAD_PRIORITY_NORMAL; break;
  }

  h = (HANDLE) _beginthreadex(NULL, stackSize, Thread_threadStart, o,
                     CREATE_SUSPENDED, &threadid);

  if (h && h != (HANDLE)-1) {
    if (prioNT != THREAD_PRIORITY_NORMAL)
      SetThreadPriority(h, prioNT);

    o->id = h;      /* unfortunate name clash */
    o->runnable = r;
  }
  else {
    o->id = (HANDLE)-1;
    o->runnable = NULL;
  }
}


BA_API void Thread_destructor(Thread* o)
{
  if (o->id && o->id != (HANDLE)-1) {
    CloseHandle(o->id);
    o->id = (HANDLE)-1;
  }
}
