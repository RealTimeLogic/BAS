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
 *   $Id: ThreadLib.c 3 2006-05-03 01:29:04Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2006 - 2011
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

#include "ThreadLib.h"


static DWORD WINAPI
Thread_threadStart(void* self)
{
   Thread* thread = (Thread*)self;
   thread->runnable(thread);
   return 0;
}


BA_API void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   DWORD targetThreadId;
   HANDLE hT;
   int prioNT;
   switch(priority)
   {
      case ThreadPrioLowest:  prioNT = THREAD_PRIORITY_LOWEST; break;
      case ThreadPrioLow:     prioNT = THREAD_PRIORITY_BELOW_NORMAL; break;
      case ThreadPrioNormal:  prioNT = THREAD_PRIORITY_NORMAL; break;
      case ThreadPrioHigh:    prioNT = THREAD_PRIORITY_ABOVE_NORMAL; break;
      case ThreadPrioHighest: prioNT = THREAD_PRIORITY_HIGHEST; break;
   }
   hT = CreateThread(NULL, /*LPSECURITY_ATTRIBUTES*/
                     stackSize,
                     Thread_threadStart,
                     o,
                     CREATE_SUSPENDED,
                     &targetThreadId);
   if(prioNT != THREAD_PRIORITY_NORMAL)
      SetThreadPriority(hT, prioNT);
   o->id = hT;
   o->runnable = r;
}


BA_API void
Thread_destructor(Thread* o)
{
  if (o->id && o->id != (HANDLE)-1) {
    CloseHandle(o->id);
    o->id = (HANDLE)-1;
  }
}


static unsigned
FileTimeToUnixTime(FILETIME* pft)
{
   __int64 t64 = ((((__int64)pft->dwHighDateTime << 32) | 
                   (__int64)pft->dwLowDateTime))
      /* we 1/10 usecs since 1601 - convert to 1/ 10 usecs since 1970 */
      - (__int64)116444736000000000; /* ms magic offset */
   return ((unsigned) (t64 / (__int64)10000000));
}

BA_API unsigned
baGetUnixTime(void)
{
   SYSTEMTIME st;
   FILETIME ft;
   GetSystemTime(&st);
   if (SystemTimeToFileTime(&st, &ft))
      return FileTimeToUnixTime(&ft);
   else
      return 0;
}
