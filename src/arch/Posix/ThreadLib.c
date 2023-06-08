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
 *   $Id: ThreadLib.c 5370 2023-01-02 23:34:32Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2023
 *
 *   this software is copyrighted by and is the sole property of Real
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

#include <ThreadLib.h>
#include <BaErrorCodes.h>
#include <HttpCfg.h>
#include <HttpTrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/stat.h>
#endif

void
_baClckGettime(BaTimeEx* spec)
{
#ifdef __APPLE__
   struct timeval t;
   if(gettimeofday(&t, NULL))
   {
      spec->sec=0;
      spec->nsec=0;
      return;
   }
   spec->sec=t.tv_sec;
   spec->nsec=t.tv_usec*1000;
#else
   struct timespec t;
#ifdef CLOCK_REALTIME
   clock_gettime(CLOCK_REALTIME, &t);
#else
   clock_gettime(0, &t);
#endif
   spec->sec=t.tv_sec;
   spec->nsec=t.tv_nsec;
#endif
   spec->offset=0;
}


BA_API unsigned int
baGetMsClock(void)
{
#ifdef __APPLE__
   struct timeval t;
   if(gettimeofday(&t, NULL))
      return 0;
   return 1000*t.tv_sec + t.tv_usec/1000;
#else
   struct timespec t;
#ifdef CLOCK_REALTIME
   /* BaTimer.c works better with MONOTONIC when system time is updated */
   if(clock_gettime(CLOCK_MONOTONIC, &t))
      return 0;
#else
   if(clock_gettime(0, &t))
      return 0;
#endif
   return 1000*t.tv_sec + t.tv_nsec/1000000;
#endif
}


BA_API void
Thread_cef(int status, const char* file, int line)
{
   if(status)
   {
#ifndef NDEBUG
      perror("ThreadLib");
#endif
      baFatalEf(FE_THREAD_LIB, status, file, line);
   }
}

/* For old cygwin */
#if 0
#if defined(__CYGWIN__)
#undef CLOCK_REALTIME
#define CLOCK_REALTIME 0
static int
clock_gettime(clockid_t na, struct timespec* val)
{
   clock_t t = clock();
   baAssert(CLK_TCK == 1000);
   val->tv_sec = t/CLK_TCK;
   val->tv_nsec = (t % CLK_TCK) * 1000;
   return 0;
}
#endif
#endif

BA_API void
ThreadMutex_constructor(ThreadMutex* o)
{
   int err;
   SYGW_SEM_FIX(o->mutex);
   o->tid=0;
   if( (err=pthread_mutex_init(&o->mutex,0)) != 0 )
      baFatalE(FE_THREAD_LIB, err);
}


BA_API void
ThreadSemaphore_constructor(ThreadSemaphore* o)
{
#ifdef __APPLE__
   char buf[32];
   sprintf(buf,"BA:%p",o);
   o->semPtr=sem_open(buf, O_CREAT,S_IRWXU, 0);
   if(!o->semPtr)
   {
      int e=errno;
      TRPR(("sem_open %d\n",e));
      baFatalE(FE_THREAD_LIB,e);
   }
#else
   SYGW_SEM_FIX(o->sem);
   Thread_ce(sem_init(&o->sem, 0, 0));
#endif
}

BA_API void
ThreadSemaphore_destructor(ThreadSemaphore* o)
{
#ifdef __APPLE__
   Thread_ce(sem_close(SEM_PTR(o)));
#else
   Thread_ce(sem_destroy(SEM_PTR(o)));
#endif
}



BA_API void*
Thread_threadStart(void* self)
{
   Thread* thread = (Thread*)self;
   ThreadSemaphore_wait(&thread->startSem);
   ThreadSemaphore_destructor(&thread->startSem);
   thread->runnable(thread);
   return 0;
}


BA_API void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority priority, int stackSize)
{
   static const char efmt[] = {"Threadlib: %s failed.\n"};
   int err;
   struct sched_param param;
   ThreadSemaphore_constructor(&o->startSem);
   memset(&o->attr, 0, sizeof(pthread_attr_t));
   o->runnable = r;
   if( (err=pthread_attr_init(&o->attr)) != 0 )
      baFatalE(FE_THREAD_LIB, err);
   if(priority != ThreadPrioNormal)
   {
      if( (err=pthread_attr_setschedpolicy(&o->attr, SCHED_RR)) != 0 )
         TRPR((0,efmt,"SCHED_RR"));
      if( (pthread_attr_getschedparam(&o->attr, &param)) != 0 )
         TRPR((0,efmt,"getschedparam"));
      else
      {
         switch(priority)
         {
            case ThreadPrioLowest:  param.sched_priority = -2; break;
            case ThreadPrioLow:     param.sched_priority = -1; break;
            case ThreadPrioNormal:                             break;
            case ThreadPrioHigh:    param.sched_priority =  1; break;
            case ThreadPrioHighest: param.sched_priority =  2; break;
         }
         if( (err=pthread_attr_setschedparam(&o->attr, &param)) != 0 )
            TRPR((0,efmt,"setschedparam prio"));
      }
   }
   /* POSIX creates threads as joinable by default. Marking the thread
    * as detached gives the system a chance to reclaim the resources
    * for that thread immediately when it exits.
    */
   pthread_attr_setdetachstate(&o->attr, PTHREAD_CREATE_DETACHED);

   /* Add another 200K bytes, which is needed when calling general
    * (Linux) libs. General Linux libs eat memory!!!
    */
   stackSize+=(1024*200);
   if(stackSize < PTHREAD_STACK_MIN) stackSize=PTHREAD_STACK_MIN;
   /* Reduce stack size. Default on Linux is 8 Mbyte */
   pthread_attr_setstacksize(&o->attr, stackSize);
   if( (err=pthread_create(&o->tid, &o->attr, Thread_threadStart, o)) != 0 )
      baFatalE(FE_THREAD_LIB, err);
}

BA_API void Thread_start(Thread* o)
{
   ThreadSemaphore_signal(&o->startSem);
}


#if defined(EINTR)
int xsem_wait(sem_t *sem)
{
  int ret;
  do {
    errno=0;
    ret = sem_wait(sem);
    if(ret!=0)
    {
       ret=errno;
       if(ret==EINTR)
          continue;
    }
    break;
  } while(1);
  return ret;
}
#endif
