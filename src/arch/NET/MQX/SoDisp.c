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
 *   $Id: SoDisp.c 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004
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

#include <HttpServer.h>
#include <HttpTrace.h>
#include <rpctypes.h>
#include <stddef.h>
#include <stdlib.h>





extern UserDefinedErrHandler barracudaUserDefinedErrHandler;


void
baFatalEf(BaFatalErrorCodes ecode1, unsigned int ecode2,
            const char* file, int line)
{
#ifdef HTTP_TRACE
   static int recursiveCall=0;
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
   for(;;) Thread_sleep(100000);
}




#define link2Con(l) \
 (SoDispCon*)((U8*)l-offsetof(SoDispCon,dispatcherLink))


/* Wait 'tmo' for socket 'read' activity.
   Returns 0 on pending data and -1 on timeout.
*/
static int
SoDispCon_rtmo(SoDispCon* o)
{
   SocketHandle slist[1];
   slist[0] = o->httpSocket.hndl;
   return RTCS_selectset(slist, 1, (U32)o->rtmo*50) ? 0 : -1;
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
            return E_TIMEOUT;
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
            return E_TIMEOUT;
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


void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   o->mutex = mutex;
   DoubleList_constructor(&o->conList);
   DoubleList_constructor(&o->pendingList);

   o->sockListLen = 20;
   o->sockList = baMalloc(sizeof(SocketHandle*) * o->sockListLen);
   if( ! o->sockList )
      baFatalE(FE_MALLOC, sizeof(SocketHandle*) * o->sockListLen);
   o->pollDelay = 1000;
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


void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   DoubleLink_constructor(&con->dispatcherLink);
}

void
SoDisp_addConnection(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_dispatcherHasCon(con));
   SoDispCon_setDispatcherHasCon(con);
}

void
SoDisp_activateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_recEvActive(con));
   SoDispCon_setRecEvActive(con);
   baAssert( !DoubleList_isInList(&o->pendingList, &con->dispatcherLink) );
   baAssert(SoDispCon_isValid(con));
   DoubleList_insertLast(&o->pendingList, &con->dispatcherLink);
}

void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_recEvActive(con));
   SoDispCon_setRecEvInactive(con);
   baAssert(DoubleLink_isLinked(&con->dispatcherLink));
   if(&con->dispatcherLink == o->curL)
   {
      o->curL = DoubleListEnumerator_nextElement(&o->iter);
   }
   DoubleLink_unlink(&con->dispatcherLink);
}


void
SoDisp_removeConnection(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_dispatcherHasCon(con));
   SoDispCon_clearDispatcherHasCon(con);
}



void
SoDisp_run(SoDisp* o, S32 timeout)
{
   U32 pollDelay;
   o->doExit = FALSE;
   if(timeout < 0)
      timeout = 0; /* RTC_selectset: wait indefinitely */
   else if(timeout == 0)
      timeout = -1; /* RTC_selectset: do not block */

   SoDisp_mutexSet(o);

   SoDisp_moveFromPending2ConList(o);
   if(DoubleList_isEmpty(&o->conList))
      baFatalE(FE_NO_SERV_CON, 0);

   do
   {
      DoubleLink* curL;
      SocketHandle selStat;
      int noOfSocks=0;

      SoDisp_moveFromPending2ConList(o);
      baAssert( ! DoubleList_isEmpty(&o->conList) );

      /* Add to socket set i.e. prepare for RTCS_selectset.*/
      DoubleListEnumerator_constructor(&o->iter, &o->conList);
      curL = DoubleListEnumerator_getElement(&o->iter);
      while(curL)
      {
         SoDispCon* con = link2Con(curL);
         if(SoDispCon_isValid(con))
         {
            if(noOfSocks == o->sockListLen)
            {
               /* Must extend the list */
               SocketHandle* newList;
               o->sockListLen += 10;
               newList = baMalloc(sizeof(SocketHandle*) * o->sockListLen);
               if( ! newList )
                  baFatalE(FE_MALLOC,
                             sizeof(SocketHandle*) * o->sockListLen);
               memcpy( newList,
                       o->sockList,
                       sizeof(SocketHandle*) * (o->sockListLen-10) );
               baFree(o->sockList);
               o->sockList = newList;
            }
            baAssert(noOfSocks < o->sockListLen);
            o->sockList[noOfSocks++] = con->httpSocket.hndl;
            curL = DoubleListEnumerator_nextElement(&o->iter);
         }
         else
         {
            o->curL = curL;
            SoDispCon_dispRecEvent(con);
            if(o->curL == curL)
               curL = DoubleListEnumerator_nextElement(&o->iter);
            else
               curL = o->curL;
         }
      }

      if(timeout)
         pollDelay = timeout;
      else
         pollDelay = o->pollDelay;

      SoDisp_mutexRelease(o);
      selStat = RTCS_selectset(o->sockList, noOfSocks, pollDelay);
      SoDisp_mutexSet(o);

      if(selStat == 0)
      { /* timeout */
         if(o->pollDelay != 1000 && timeout == 0)
         {
            o->pollDelay = o->pollDelay + o->pollDelay/15;
            if(o->pollDelay > 1000)
               o->pollDelay = 1000;
            continue;
         }
         if(o->pollDelay == 0)
            o->pollDelay=1000;
      }

      /*Check socket and execute if data.*/
      if(selStat == RTCS_SOCKET_ERROR)
      {
         TRPR(("Select failed\n"));
      }
      else
      {
         DoubleListEnumerator_constructor(&o->iter, &o->conList);
         o->curL = curL = DoubleListEnumerator_getElement(&o->iter);
         while(curL)
         {
            SoDispCon* con = link2Con(curL);
            if( ! SoDispCon_isValid(con) ||
                con->httpSocket.hndl == selStat )
            {
               SoDispCon_setDispHasRecData(con);
               SoDispCon_dispRecEvent(con);
            }
            if(o->curL == curL)
               o->curL = DoubleListEnumerator_nextElement(&o->iter);
            curL = o->curL;
         }
      }
   } while(timeout == 0 && ! o->doExit);
   SoDisp_mutexRelease(o);
}
