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
 *   $Id: ThreadLib.c 5672 2025-10-17 00:14:58Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2022 - 2025
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

#define BAS_PRIO_BASE 10

#include <ThreadLib.h>
#include <BaErrorCodes.h>
#include <HttpCfg.h>
#include <HttpServer.h>  /* For HttpCommand */

static time_t unix_time_offset = 0;

BaTime baGetUnixTime(void)
{
   struct timeval tv;
   if (gettimeofday(&tv, NULL) == 0)
   {
      return tv.tv_sec;
   }
   return (time_t)-1;
}


void HttpSockaddr_gethostbynameF(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status) 
{
   if(host)
   {
      struct zsock_addrinfo hints;
      struct zsock_addrinfo* retAddrInfoPtr;
      hints.ai_flags = AI_ALL;
      hints.ai_family = useIp6 ? AF_INET6 : AF_INET;
      if(zsock_getaddrinfo(host, 0, &hints, &retAddrInfoPtr) == 0)
      {
         struct zsock_addrinfo * addrInfoPtr;
         addrInfoPtr = retAddrInfoPtr;
         if (addrInfoPtr->ai_family == AF_INET && !useIp6)
         {
            struct sockaddr_in * tempSockInPtr =
               (struct sockaddr_in*)addrInfoPtr->ai_addr;
            *((U32*)o->addr) = tempSockInPtr->sin_addr.s_addr;
         }
         else if(useIp6)
         {
            struct sockaddr_in6 * tempSockIn6Ptr =
               (struct sockaddr_in6*)addrInfoPtr->ai_addr;
            memcpy(o->addr, &tempSockIn6Ptr->sin6_addr, 16);
         }
         else
            *status=-1;
      }
      else
         *status=-1;
   }
   else if(useIp6)
   {
      memset(o->addr, 0, 16);
      o->isIp6=TRUE;
   }
   else
   {
      unsigned long ipAddr;
      ipAddr = baHtonl(INADDR_ANY);
      *((U32*)o->addr) = (U32)ipAddr;
      o->isIp6=FALSE;
   }
}

static struct k_thread baCtrlThread;
K_THREAD_STACK_DEFINE(baCtrlStack, 2048);
K_MSGQ_DEFINE(baCtrlThreadMsg, sizeof(void*), 2, sizeof(void*));

/* Releases stack when an "owner" thread calls Thread_destructor
 */
static void
baReleaseStackThread(void* dummy1, void* dummy2, void* dummy3)
{
   (void)dummy1;
   (void)dummy2;
   (void)dummy3;

   for(;;)
   {
      void* memBlock;
      k_msgq_get(&baCtrlThreadMsg, &memBlock, K_FOREVER);
      baFree(memBlock);
   }
}


static void
Thread_threadEntry(void* o, void* dummy1, void* dummy2)
{
   (void)dummy1;
   (void)dummy2;
   k_sem_take(((ThreadBase*)o)->startSem, K_FOREVER);
   baFree(((ThreadBase*)o)->startSem);
   ((ThreadBase*)o)->runnable((ThreadBase*)o);
   ((ThreadBase*)o)->startSem=0; /* Signal Thread_destructor */
}
 

void
Thread_constructor(
   ThreadBase* o, Thread_Run r, ThreadPriority priority, int stackSize)
{ 
   int prio=BAS_PRIO_BASE;
   
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
   o->runnable=r;
   stackSize+=sizeof(k_thread_stack_t);
   o->stackStart = (k_thread_stack_t*)baMalloc(stackSize);
   o->startSem = (struct k_sem*)baMalloc(sizeof(struct k_sem));
   if( ! o->stackStart || !  o->startSem )
   {
      baFatalE(FE_MALLOC, sizeof(HttpCommand));
   }
   k_sem_init(o->startSem, 0, 1);
   o->tid=k_thread_create(&o->thread,
                          o->stackStart, stackSize,
                          Thread_threadEntry, o, NULL, NULL,
                          prio, 0, K_NO_WAIT);
}


void
Thread_destructor(struct ThreadBase* o)
{
   if(o->tid == k_current_get()) /* if owner */
   {
      static int started=0;
      k_sched_lock();
      if( ! started )
      {
         started=1;
         k_sched_unlock();
         k_thread_create(&baCtrlThread, baCtrlStack,
                         K_THREAD_STACK_SIZEOF(baCtrlStack),
                         baReleaseStackThread, NULL, NULL, NULL,
                         BAS_PRIO_BASE+3, 0, K_NO_WAIT);
      }
      else
         k_sched_unlock();
      k_msgq_put(&baCtrlThreadMsg, o->stackStart, K_FOREVER);
      o->stackStart=0;
   }
   else
   {
      if(o->startSem)  /* If still running */
         baFatalE(FE_THREAD_LIB,__LINE__);
      if(o->stackStart)
      {
         baFree(o->stackStart);
         o->stackStart=0;
      }
   }
}
