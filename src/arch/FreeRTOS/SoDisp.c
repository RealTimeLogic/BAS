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
 *   $Id: SoDisp.c 5142 2022-05-03 19:08:40Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2019 - 2022
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
 * Platform: FreeRTOS+TCP.
 */

#ifdef BA_FREERTOS_PLUS_TCP

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

#if ipconfigSUPPORT_SELECT_FUNCTION != 1
#error ipconfigSUPPORT_SELECT_FUNCTION not set
#endif

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
                       "%s, line %d\n",
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




#define link2Con(l)                                             \
   ((SoDispCon*)((U8*)l-offsetof(SoDispCon,dispatcherLink)))


#define SoDispCon_add2SockSet(o, sockSet, highSockDesc) do {    \
      int s = (o)->httpSocket.hndl;                             \
      FD_SET(s, &(sockSet));                                    \
      if(s > highSockDesc)                                      \
         highSockDesc = s;                                      \
   } while(0)


static void
SoDispCon_setRTMO(SoDispCon* o)
{
   TickType_t tmoTks;
   tmoTks = pdMS_TO_TICKS(o->rtmo*50);
   FreeRTOS_setsockopt(o->httpSocket.xSocket,0,FREERTOS_SO_RCVTIMEO,&tmoTks,0);
}

static void
SoDispCon_clearRTMO(SoDispCon* o)
{
   TickType_t tmoTks=portMAX_DELAY;
   FreeRTOS_setsockopt(o->httpSocket.xSocket,0,FREERTOS_SO_RCVTIMEO,&tmoTks,0);
}



void HttpSocket_constructor(HttpSocket* o)
{
   memset(o, 0, sizeof(HttpSocket));
   o->xSocket = FREERTOS_INVALID_SOCKET;
}

#define RETURN_ON_INV_SOCK(xSocket,statusPtr) do { if(xSocket == FREERTOS_INVALID_SOCKET) {*statusPtr=-1; return;} } while(0)

void
HttpSocket_send(HttpSocket* o, struct ThreadMutex* m,
                BaBool* isTerminated, const void* data, int len, int* retLen)
{
   size_t left;
   U8* ptr;
   RETURN_ON_INV_SOCK(o->xSocket, retLen);
   if(o->isNonBlock)
      *retLen = FreeRTOS_send(o->xSocket,data,(size_t)len,0);
   else
   {
      *retLen = len;
      for(left = (size_t)len, ptr = (U8*)data ; left > 0 ; )
      {
         BaseType_t x = FreeRTOS_send(o->xSocket,ptr,left,0);
         if(x <= 0)
         {
            *retLen = x == 0 ? -1 : x;
            break;
         }
         ptr+=left;
         baAssert(left >= (size_t)x);
         left -= x;
      }
   }
}


int
_HttpSocket_setNonblockingOnOff(HttpSocket* o, int on)
{
   TickType_t tmoTks = on ? 0 : portMAX_DELAY;
   if(o->xSocket == FREERTOS_INVALID_SOCKET)
      return -1;
   FreeRTOS_setsockopt(o->xSocket,0,FREERTOS_SO_RCVTIMEO,&tmoTks,0);
   FreeRTOS_setsockopt(o->xSocket,0,FREERTOS_SO_SNDTIMEO,&tmoTks,0);
   o->isNonBlock=on;
   return 0;
}


void
HttpSockaddr_gethostbyname(
   HttpSockaddr* addr, const char* host, BaBool ip6, int* status)
{
   uint32_t ip=0;
   if (ip6)
      goto L_err;
   if(host)
      ip=FreeRTOS_gethostbyname((uint8_t*)host);
   if(!host || ip)
   {
      memcpy(addr->addr,&ip,4);
      addr->isIp6 = FALSE;
      *status = 0;
   }
   else
   {
     L_err:
      *status = -1;
   }
}


void
HttpSocket_setTCPNoDelay(HttpSocket* o, int enable, int* status)
{
   /* Nagle not implemented in +TCP */
   (void)o;
   (void)enable;
   *status=0;
}


void
HttpSocket_sockStream(HttpSocket* o, const char* iname ,BaBool ip6, int* status)
{
   o->xSocket = FreeRTOS_socket(
      FREERTOS_AF_INET,FREERTOS_SOCK_STREAM,FREERTOS_IPPROTO_TCP );
   *status = o->xSocket == FREERTOS_INVALID_SOCKET ? -1 : 0;
}


void
HttpSocket_bind(HttpSocket* o, HttpSockaddr* addr, U16 port, int* status)
{
   struct freertos_sockaddr xAddr;
   RETURN_ON_INV_SOCK(o->xSocket, status);
   xAddr.sin_family = FREERTOS_AF_INET;
   memcpy(&xAddr.sin_addr, addr->addr, 4);
   o->port = xAddr.sin_port = FreeRTOS_htons(port);
   *status = FreeRTOS_bind(o->xSocket, &xAddr, sizeof(xAddr));
}


void
HttpSocket_listen(HttpSocket* o, HttpSockaddr* na, int qsize,int* status)
{
   (void)na;
   RETURN_ON_INV_SOCK(o->xSocket, status);
   *status = FreeRTOS_listen(o->xSocket, qsize); 
}


void
HttpSocket_connect(HttpSocket* o, HttpSockaddr* addr, U16 port, int* status)
{
   struct freertos_sockaddr xAddr;
   RETURN_ON_INV_SOCK(o->xSocket, status);
   xAddr.sin_family = FREERTOS_AF_INET;
   memcpy(&xAddr.sin_addr, addr->addr, 4);
   o->port = xAddr.sin_port = FreeRTOS_htons(port);
   *status = FreeRTOS_connect(o->xSocket, &xAddr, sizeof(xAddr));
}


void
HttpSocket_accept(HttpSocket* o, HttpSocket* conSock, int* status)
{
   struct freertos_sockaddr xAddr;
   socklen_t xSize = sizeof(xAddr);
   RETURN_ON_INV_SOCK(o->xSocket, status);
   conSock->xSocket = FreeRTOS_accept(o->xSocket, &xAddr, &xSize);
   conSock->port = xAddr.sin_port;
   *status = conSock->xSocket == FREERTOS_INVALID_SOCKET ? -1 : 0;
}


void
HttpSocket_close(HttpSocket* o)
{
   if(o->xSocket != FREERTOS_INVALID_SOCKET)
   {
      FreeRTOS_shutdown(o->xSocket, FREERTOS_SHUT_RDWR);
      FreeRTOS_closesocket(o->xSocket);
      o->xSocket = FREERTOS_INVALID_SOCKET;
   }
}


void
HttpSocket_move(HttpSocket* o,HttpSocket* newS)
{
   newS->xSocket = o->xSocket;
   newS->port = o->port;
   newS->isNonBlock = o->isNonBlock;
   o->xSocket = FREERTOS_INVALID_SOCKET;
}


void
HttpSocket_getAddr(HttpSocket* o, HttpSockaddr* addr, U16* port,
                   BaBool ip6, int* status, int localAddr)
{
   struct freertos_sockaddr xAddr;
   *status = 0;
   if(localAddr)
   {
      xAddr.sin_port = o->port;
      xAddr.sin_addr = FreeRTOS_GetIPAddress();
   }
   else
   {
      if(o->xSocket == FREERTOS_INVALID_SOCKET ||
         sizeof(struct freertos_sockaddr) !=
         FreeRTOS_GetRemoteAddress(o->xSocket, &xAddr))
      {
         *status = -1;
      }
   }
   xAddr.sin_family = FREERTOS_AF_INET;
   memcpy(addr->addr, &xAddr.sin_addr, 4);
   if(port)
      *port = FreeRTOS_htons(xAddr.sin_port);
   addr->isIp6=FALSE;
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
   if(o->httpSocket.xSocket == FREERTOS_INVALID_SOCKET)
      return E_SOCKET_READ_FAILED;
   if(m && ThreadMutex_isOwner(m))
   {
      SoDisp* disp=0;
      if(o->rtmo)
      {
         if(SoDispCon_recEvActive(o))
         {
            disp=SoDispCon_getDispatcher(o);
            SoDisp_deactivateRec(disp, o);
         }
         SoDispCon_setRTMO(o);
      }
      ThreadMutex_release(m);
      HttpSocket_recv(&o->httpSocket,data,len,&status);
      ThreadMutex_set(m);
      if(*isTerminated)
         return E_SOCKET_READ_FAILED;
      if(disp && status >= 0)
         SoDisp_activateRec(disp, o); 
   }
   else
   {
      if(o->rtmo)
         SoDispCon_setRTMO(o);
      HttpSocket_recv(&o->httpSocket,data,len,&status);
   }
   if(status < 0)
   {
      SoDispCon_closeCon(o);
      return E_SOCKET_READ_FAILED;
   }
   if( ! SoDispCon_isNonBlocking(o) )
      SoDispCon_clearSocketHasNonBlockData(o);
   if(o->rtmo)
   {
      SoDispCon_clearRTMO(o);
      return status == 0 ? E_TIMEOUT : status;
   }
   return status; /* where zero is timeout on non blocking */
}


void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   o->mutex = mutex;
   DoubleList_constructor(&o->conList);
   o->defaultPollDelay = o->pollDelay = 1000;
   o->xFDSet = FreeRTOS_CreateSocketSet();
   o->doExit = FALSE;
}


void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   (void)o; /* not used */
   DoubleLink_constructor(&con->dispatcherLink);
}

void
SoDisp_addConnection(SoDisp* o, SoDispCon* con)
{
   (void)o; /* not used */
   baAssert(!SoDispCon_dispatcherHasCon(con));
   SoDispCon_setDispatcherHasCon(con);
}


void
SoDisp_activateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_recEvActive(con));
   SoDispCon_setRecEvActive(con);
   if( ! DoubleList_isInList(&o->conList, &con->dispatcherLink) )
      DoubleList_insertLast(&o->conList, &con->dispatcherLink);
   con->httpSocket.selectBit |= 1;
   o->selectSet=TRUE;
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_recEvActive(con));
   baAssert(DoubleLink_isLinked(&con->dispatcherLink));
   SoDispCon_setRecEvInactive(con);
   if(con->httpSocket.xSocket != FREERTOS_INVALID_SOCKET)
      FreeRTOS_FD_CLR(con->httpSocket.xSocket, o->xFDSet, eSELECT_READ);
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
   if( ! DoubleList_isInList(&o->conList, &con->dispatcherLink) )
      DoubleList_insertLast(&o->conList, &con->dispatcherLink);
   con->httpSocket.selectBit |= 2;
   o->selectSet=TRUE;
}


void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_sendEvActive(con));
   baAssert(DoubleLink_isLinked(&con->dispatcherLink));
   SoDispCon_setSendEvInactive(con);
   if(con->httpSocket.xSocket != FREERTOS_INVALID_SOCKET)
      FreeRTOS_FD_CLR(con->httpSocket.xSocket, o->xFDSet, eSELECT_WRITE);
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
   DoubleLink* curL;
   SoDispCon* con;
   TickType_t tmoTicks;
   BaseType_t status;
   SoDisp_mutexSet(o);
   o->doExit = FALSE;
   if(timeout >= 0)
      tmoTicks = pdMS_TO_TICKS(timeout);
   else
      tmoTicks = pdMS_TO_TICKS(o->pollDelay);
   do
   {
      SoDisp_mutexRelease(o);
      if(DoubleList_isEmpty(&o->conList))
      {
         status = 0;
         vTaskDelay(tmoTicks);
      }
      else
      {
         if(o->selectSet)
         {
            DoubleListEnumerator_constructor(&o->iter, &o->conList);
            curL = DoubleListEnumerator_getElement(&o->iter);
            while(curL)
            {
               HttpSocket* s;
               s = &link2Con(curL)->httpSocket;
               if(s->selectBit)
               {
                  if(s->selectBit & 1)
                     FreeRTOS_FD_SET(s->xSocket, o->xFDSet, eSELECT_READ);
                  if(s->selectBit & 2)
                     FreeRTOS_FD_SET(s->xSocket, o->xFDSet, eSELECT_WRITE);
                  s->selectBit=0;
               }
               curL = DoubleListEnumerator_nextElement(&o->iter);
            }
            o->selectSet=FALSE;
         }
         status = FreeRTOS_select(o->xFDSet, tmoTicks);
      }
      SoDisp_mutexSet(o);

      /*Check socket descriptors and execute if data.*/
      if(status)
      {
         DoubleListEnumerator_constructor(&o->iter, &o->conList);
         o->curL = curL = DoubleListEnumerator_getElement(&o->iter);
         while(curL)
         {
            con = link2Con(curL);
            if(SoDispCon_isValid(con))
            {
               int executed=FALSE;
               BaseType_t xSocketBits = FreeRTOS_FD_ISSET(
                  con->httpSocket.xSocket, o->xFDSet);
               if(xSocketBits)
               {
                  if(SoDispCon_sendEvActive(con) && 
                     (xSocketBits & eSELECT_WRITE))
                  {
                     SoDispCon_dispSendEvent(con);
                     executed=TRUE;
                  }
                  if( SoDispCon_recEvActive(con) &&
                      (xSocketBits & eSELECT_READ) &&
                      o->curL == curL)
                  {
                     SoDispCon_setDispHasRecData(con);
                     SoDispCon_dispRecEvent(con);
                     executed=TRUE;
                  }
                  if(!executed)
                  {
                     if(xSocketBits & (eSELECT_EXCEPT | eSELECT_INTR))
                     {
                        HttpSocket_close(&con->httpSocket);
                        goto L_SockErr;
                     }
                  }
               }
            }
            else /* Socket failure */
            {
              L_SockErr:
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
      else
      {
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
   } while( (timeout < 0 || status) && ! o->doExit );
   SoDisp_mutexRelease(o);
}

#endif /* BA_FREERTOS_PLUS_TCP */
