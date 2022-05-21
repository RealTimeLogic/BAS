/*
 *     ____             _________                __                _
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/
 *                                                       /____/
 *
 *                  Barracuda Embedded Web-Server
 ****************************************************************************
 *            PROGRAM MODULE
 *
 *   $Id: SoDisp.c 2953 2013-08-01 14:06:32Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2002 - 2004
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
 */

#ifndef BA_LIB
#define BA_LIB 1
#endif

#define sodisp_c 1

#ifdef __cplusplus
#error Cannot compile any Barracuda code in C++ mode
#endif

#include <HttpServer.h>
#include <HttpTrace.h>
#include <stddef.h>
#include <stdlib.h>


extern UserDefinedErrHandler barracudaUserDefinedErrHandler;



BA_API void
baFatalEf(BaFatalErrorCodes ecode1, unsigned int ecode2,
            const char* file, int line)
{
   static int recursiveCall=0;
#ifdef HTTP_TRACE
   if( ! recursiveCall )
   {
      recursiveCall=1;
      HttpTrace_printf(0,"Fatal error detected in Barracuda.\n"
                       "E1 = %d, E2=%d\n"
                       "%s, line %d",
                       ecode1, ecode2,
                       file, line);
      HttpTrace_flush();
   }
#endif
   if(barracudaUserDefinedErrHandler)
      (*barracudaUserDefinedErrHandler)(ecode1, ecode2, file, line);
   else
   {
      for(;;) Thread_sleep(100000);
   }
   recursiveCall=0;
}


void
HttpSockaddr_gethostbyname(
   HttpSockaddr* o, const char* host, BaBool ip6, int* status)
{
   o->isIp6=ip6;
   *status=-1;
   HttpSockaddr_inetAddr(o, host, ip6, status);
   if(*status)
   {
      NU_HOSTENT* hentry;
      STATUS nstatus;
      hentry = NU_Get_IP_Node_By_Name(
         (CHAR*)host,
         ip6 ? NU_FAMILY_IP6 : NU_FAMILY_IP,
         ip6 ? DNS_ALL | DNS_V4MAPPED : 0,
         &nstatus);
      if(hentry)
      {
         if(status == NU_SUCCESS)
         {
            int i;
            int len = ip6 ? 16 : 4;
            for(i=0 ; i < len ; i++)
            {
               if(hentry->h_addr_list[i] != 0)
                  break;
            }
            if(i != len)
            {
               memcpy(o->addr,hentry->h_addr_list, len);
               *status=0;
            }
         }
         NU_Free_Host_Entry(hentry);
      }
   }
}


#define link2Con(l) \
 (SoDispCon*)((U8*)l-offsetof(SoDispCon,dispatcherLink))


#define SoDispCon_add2SockSet(o, sockSet, highSockDesc) do { \
   int s = (o)->httpSocket.hndl; \
   NU_FD_Set(s, &(sockSet)); \
   if(s > highSockDesc) \
      highSockDesc = s; \
} while(0)


/* Wait 'tmo' for socket 'read' activity.
   Returns 0 on pending data and -1 on timeout.
*/
static int
SoDispCon_rtmo(SoDispCon* o)
{
   FD_SET recSet;
   int highSockDesc=0;
   BaTime tmo = (BaTime)o->rtmo*50;
   NU_FD_Init(&recSet);
   SoDispCon_add2SockSet(o, recSet, highSockDesc);
   return NU_Select(
      highSockDesc+1, &recSet, 0, 0, baMsTime2TxTicks(tmo)) > 0 ? 0 : -1;
}


/* Returns 0 if no data, >0 if data and <0 on error.
   This function is called unprotected i.e. the SoDisp mutex is not set.
   isTerminated is a variable handled by the caller.
   Can be set to TRUE by a another thread.
   If TRUE, the object "o" is no longer valid.
*/
int
SoDispCon_platReadData(SoDispCon* o, ThreadMutex* m, BaBool* isTerminated,
                       void* data, int len)
{
   int status;
   if(m && ThreadMutex_isOwner(m))
   {
      ThreadMutex_release(m);
      if(o->rtmo)
      {
         status=SoDispCon_rtmo(o);
         if(*isTerminated || status)
         {
            ThreadMutex_set(m);
            if(*isTerminated)
               return E_SOCKET_READ_FAILED;
            o->rtmo=0;
            return E_SOCKET_READ_TIMEOUT;
         }
      }
      HttpSocket_recv(&o->httpSocket,data,len,&status);
      ThreadMutex_set(m);
      if(*isTerminated)
         return E_SOCKET_READ_FAILED;
   }
   else
   {
      if(o->rtmo)
      {
         status=SoDispCon_rtmo(o);
         o->rtmo=0;
         if(status)
            return E_SOCKET_READ_TIMEOUT;
      }
      HttpSocket_recv(&o->httpSocket,data,len,&status);
   }
   if( ! SoDispCon_isNonBlocking(o) )
      SoDispCon_clearSocketHasNonBlockData(o);
   if(status < 0)
   {
      SoDispCon_closeCon(o);
      return E_SOCKET_READ_FAILED;
   }
   /* status=len, which can be zero if a non blocking socket.
    */
   return status;
}


BA_API void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   o->mutex = mutex;
   DoubleList_constructor(&o->conList);
   DoubleList_constructor(&o->pendingList);
   o->defaultPollDelay = o->pollDelay = 1000;
   o->doExit = FALSE;
}


static void
SoDisp_moveFromPending2ConList(SoDisp* o)
{
   while( ! DoubleList_isEmpty(&o->pendingList) )
   {
      DoubleLink* l = DoubleList_removeFirst(&o->pendingList);
      DoubleList_insertLast(&o->conList, l);
   }
}


BA_API void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   (void)o; /* not used */
   DoubleLink_constructor(&con->dispatcherLink);
}

BA_API void
SoDisp_addConnection(SoDisp* o, SoDispCon* con)
{
   (void)o; /* not used */
   baAssert(!SoDispCon_dispatcherHasCon(con));
   SoDispCon_setDispatcherHasCon(con);
}


BA_API void
SoDisp_activateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_recEvActive(con));
   SoDispCon_setRecEvActive(con);
   if( ! SoDispCon_sendEvActive(con) )
   {
      if( ! DoubleList_isInList(&o->pendingList, &con->dispatcherLink) )
         DoubleList_insertLast(&o->pendingList, &con->dispatcherLink);
   }
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_recEvActive(con));
   baAssert(DoubleLink_isLinked(&con->dispatcherLink));
   SoDispCon_setRecEvInactive(con);
   if( ! SoDispCon_sendEvActive(con) )
   {
      if(&con->dispatcherLink == o->curL)
      {
         o->curL = DoubleListEnumerator_nextElement(&o->iter);
      }
      DoubleLink_unlink(&con->dispatcherLink);
   }
}


void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_isValid(con));
   baAssert(!SoDispCon_sendEvActive(con));
   SoDispCon_setSendEvActive(con);
   if( ! SoDispCon_recEvActive(con) )
   {
      if( ! DoubleList_isInList(&o->pendingList, &con->dispatcherLink) )
         DoubleList_insertLast(&o->pendingList, &con->dispatcherLink);
   }
}


void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_sendEvActive(con));
   baAssert(DoubleLink_isLinked(&con->dispatcherLink));
   SoDispCon_setSendEvInactive(con);
   if( ! SoDispCon_recEvActive(con) )
   {
      if(&con->dispatcherLink == o->curL)
      {
         o->curL = DoubleListEnumerator_nextElement(&o->iter);
      }
      DoubleLink_unlink(&con->dispatcherLink);
   }
}


void
SoDisp_removeConnection(SoDisp* o, SoDispCon* con)
{
   (void)o; /* not used */
   baAssert(SoDispCon_dispatcherHasCon(con));
   baAssert(!SoDispCon_recEvActive(con));
   baAssert(!SoDispCon_sendEvActive(con));
   SoDispCon_clearDispatcherHasCon(con);
}

void
SoDisp_run(SoDisp* o, S32 timeout)
{
   INT descriptors=0;
   SoDisp_mutexSet(o);
   o->doExit = FALSE;
   SoDisp_moveFromPending2ConList(o);
   do
   {
      FD_SET recSet;
      FD_SET sendSet;
      DoubleLink* curL;
      UNSIGNED ttmo; /* Tick timeout */
      INT highSockDesc=0;
      SoDisp_moveFromPending2ConList(o);
      /* Add to socket set i.e. prepare for select.*/
      NU_FD_Init(&recSet);
      NU_FD_Init(&sendSet);
      DoubleListEnumerator_constructor(&o->iter, &o->conList);
      curL = DoubleListEnumerator_getElement(&o->iter);
      while(curL)
      {
         SoDispCon* con = link2Con(curL);
         if(SoDispCon_isValid(con))
         {
            if(SoDispCon_recEvActive(con))
               SoDispCon_add2SockSet(con, recSet, highSockDesc);
            if(SoDispCon_sendEvActive(con))
               SoDispCon_add2SockSet(con, sendSet, highSockDesc);
            curL = DoubleListEnumerator_nextElement(&o->iter);
         }
         else
         {
            o->curL = curL;
            SoDispCon_setDispHasRecData(con);
            SoDispCon_dispRecEvent(con);
            if(o->curL == curL)
               curL = DoubleListEnumerator_nextElement(&o->iter);
            else
               curL = o->curL;
         }
      }
      ttmo = baMsTime2TxTicks(timeout >= 0 ? timeout :  o->pollDelay);
      SoDisp_mutexRelease(o);
      if(highSockDesc)
         descriptors = NU_Select(highSockDesc+1, &recSet, &sendSet, 0, ttmo);
      else
      {
         descriptors=0;
         Thread_sleep(o->pollDelay);
      }
      SoDisp_mutexSet(o);

      /*Check socket descriptors and execute if data.*/
      if(descriptors >= 0)
      {
         DoubleListEnumerator_constructor(&o->iter, &o->conList);
         o->curL = curL = DoubleListEnumerator_getElement(&o->iter);
         while(curL)
         {
            SoDispCon* con = link2Con(curL);
            if(SoDispCon_isValid(con))
            {
               if(NU_FD_Check(con->httpSocket.hndl, &sendSet))
               {
                  SoDispCon_dispSendEvent(con);
               }
               if(o->curL == curL &&
                  NU_FD_Check(con->httpSocket.hndl, &recSet))
               {
                  SoDispCon_setDispHasRecData(con);
                  SoDispCon_dispRecEvent(con);
               }
            }
            else /* Socket failure. Can only occur in multithread mode */
            {
               if(SoDispCon_sendEvActive(con))
               {
                  SoDispCon_dispSendEvent(con);
               }
               else
               {
                  SoDispCon_setDispHasRecData(con);
                  SoDispCon_dispRecEvent(con);
               }
            }
            if(o->curL == curL)
               o->curL = DoubleListEnumerator_nextElement(&o->iter);
            curL = o->curL;
         }
      }
      else if(descriptors == 0)
      {
         baAssert(tvp != 0);
         if(o->pollDelay != o->defaultPollDelay)
         {
            if(o->pollDelay == 0)
               o->pollDelay=o->defaultPollDelay;
            else
            {
               o->pollDelay = o->pollDelay + o->pollDelay/15;
               if(o->pollDelay > o->defaultPollDelay)
                  o->pollDelay = o->defaultPollDelay;
            }
         }
      }
      else
      {
         SoDisp_mutexRelease(o);
         /* Give other thread time to cleanup (remove) socket */
         Thread_sleep(10);
         SoDisp_mutexSet(o);
      }
   } while( (timeout < 0 || descriptors > 0) && ! o->doExit );
   SoDisp_mutexRelease(o);
}
