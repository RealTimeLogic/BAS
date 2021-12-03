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
 *   COPYRIGHT:  Real Time Logic, 2007 - 2019
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
#include <BaErrorCodes.h>
#include <HttpCfg.h>

#ifdef UseVxWorksGethostbyname
void HttpSockaddr_gethostbynameF(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status) 
{
   if(host)
   {
      struct addrinfo hints;
      struct addrinfo * retAddrInfoPtr;
      hints.ai_flags = AI_ALL;
      hints.ai_family = useIp6 ? AF_INET6 : AF_INET;
      if(ipcom_getaddrinfo(host, 0, &hints, &retAddrInfoPtr) == 0)
      {
         struct addrinfo * addrInfoPtr;
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
#endif

#ifndef NDEBUG
void
Thread_cef(int status, const char* file, int line)
{
   if(status != OK)
   {
      baFatalEf(FE_THREAD_LIB, status, file, line);
   }
}
#endif

static int
Thread_threadStart(
   VX_USR_ARG_T     arg1,
   VX_USR_ARG_T     arg2,
   VX_USR_ARG_T     arg3,
   VX_USR_ARG_T     arg4,
   VX_USR_ARG_T     arg5,
   VX_USR_ARG_T     arg6,
   VX_USR_ARG_T     arg7,
   VX_USR_ARG_T     arg8,
   VX_USR_ARG_T     arg9,
   VX_USR_ARG_T     arg10)
{
   Thread* thread = (Thread*)arg1;
   (void)arg2;
   (void)arg3;
   (void)arg4;
   (void)arg5;
   (void)arg6;
   (void)arg7;
   (void)arg8;
   (void)arg9;
   (void)arg10;
   thread->runnable(thread);
   taskExit(0);
   return 0;
}


void
Thread_constructor(
   Thread* o, Thread_Run r, ThreadPriority tpri, int stackSize)
{
   int priority;
   taskPriorityGet(taskIdSelf(), &priority);
   o->runnable = r;
   switch(tpri)
   {
      case ThreadPrioLowest:  priority += 2; break;
      case ThreadPrioLow:     priority += 1; break;
      case ThreadPrioNormal:                 break;
      case ThreadPrioHigh:    priority -=  1; break;
      case ThreadPrioHighest: priority -=  2; break;
   }

   o->tid = 
#if _WRS_VXWORKS_MAJOR == 5
      taskCreat
#else
      taskCreate
#endif
      (
        "Barracuda",       /* name of new task (stored at pStackBase) */
        priority,   /* priority of new task */
        VX_FP_TASK,  /* Required since Lua can use floating point */
        stackSize,  /* size (bytes) of stack needed */
        Thread_threadStart,    /* entry point of new task */
        (VX_USR_ARG_T)o,   /* arg1 = object pointer */
        2, 3, 4, 5, 6, 7, 8, 9, 10);


}
