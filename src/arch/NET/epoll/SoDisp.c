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
 *   $Id: SoDisp.c 5468 2023-08-02 13:57:49Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2008 - 2017
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


Socket Dispatcher EPOLL implementation.
http://linux.die.net/man/7/epoll

Note: Check the following on Linux kernel >= 2.6.28
/proc/sys/fs/epoll/max_user_watches

Make sure this is sufficiently large for the server's connection requirements.
Modify by adding the following to /etc/sysctl.conf
fs.epoll.max_user_instances = 8192
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

#define link2Con(l) \
 (SoDispCon*)((U8*)l-offsetof(SoDispCon,dispatcherLink))


BA_API void
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


int
BaAddrinfo_platConnect(BaAddrinfo* addr, HttpSocket* s, U32 timeout)
{
   int status;
   struct timeval tv={0}; 
   if(timeout)
   {
      tv.tv_sec = timeout / 1000;
      tv.tv_usec = (timeout % 1000) * 1000;
      setsockopt(s->hndl, SOL_SOCKET, SO_RCVTIMEO,
                 (char *)&tv,sizeof(struct timeval));
   }
   else
      HttpSocket_setNonblocking(s, &status);
   status = socketConnect(s->hndl, addr->ai_addr, addr->ai_addrlen);
   if(status)
   {
      if(timeout)
         status = E_CANNOT_CONNECT;
      else
      {
         HttpSocket_wouldBlock(s, &status);
         if( ! status )
            status = E_CANNOT_CONNECT;
         else
            status = 0; /* pending */
      }
   }
   else
      status = 1; /* We are done */
   if(timeout)
   {
      tv.tv_sec = tv.tv_usec = 0; 
      setsockopt(s->hndl, SOL_SOCKET, SO_RCVTIMEO,
                 (char *)&tv,sizeof(struct timeval));
   }
   return status;
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

   if(m && ! ThreadMutex_isOwner(m))
      m=0;
   if(o->rtmo)
   {
      SoDisp* disp;
      struct timeval tv={0};
      BaTime tmo = (BaTime)o->rtmo * 50;
      o->rtmo=0;
      tv.tv_sec = tmo / 1000;
      tv.tv_usec = (tmo % 1000) * 1000; 
      if(SoDispCon_recEvActive(o))
      {
         disp=SoDispCon_getDispatcher(o);
         SoDisp_deactivateRec(disp, o);
      }
      else
         disp=0;
      setsockopt(o->httpSocket.hndl, SOL_SOCKET, SO_RCVTIMEO,
                 (char *)&tv,sizeof(struct timeval));
      if(m) ThreadMutex_release(m);
      status = recv(o->httpSocket.hndl,data,len,0);
      if(m) 
      {
         ThreadMutex_set(m);
         if(*isTerminated)
            return E_SOCKET_READ_FAILED;
      }
      if(status < 0)
      {
         int e=errno;
         status = (e == EAGAIN || e == EWOULDBLOCK) ?
            E_TIMEOUT : E_SOCKET_READ_FAILED;
      }
      else if(status == 0) /* graceful disconnect */
         status = E_SOCKET_CLOSED;
      if(status > 0 || status == E_TIMEOUT)
      {
         tv.tv_sec = tv.tv_usec = 0;
         setsockopt(o->httpSocket.hndl, SOL_SOCKET, SO_RCVTIMEO,
                    (char *)&tv,sizeof(struct timeval));
         if(disp)
            SoDisp_activateRec(disp, o);
         if(status == E_TIMEOUT)
            return status;
      }
   }
   else
   {
      if(m) ThreadMutex_release(m);
      HttpSocket_recv(&o->httpSocket,data,len,&status);
      if(m) 
      {
         ThreadMutex_set(m);
         if(*isTerminated)
            return E_SOCKET_READ_FAILED;
      }
   }
   if( ! SoDispCon_isNonBlocking(o) )
      SoDispCon_clearSocketHasNonBlockData(o);
   if(status < 0)
   {
      SoDispCon_closeCon(o);
      return status == -1 ? E_SOCKET_READ_FAILED : status;
   }
   /* status=len, which can be zero if a non blocking socket.
    */
   return status;
}


static void
SoDisp_changeConState(SoDisp* o,SoDispCon* con, int ctlType)
{
   struct epoll_event ev;
   SoDispConBucket* cb;
   U16 offs;
   memset(&ev,0,sizeof(ev));
   if(ctlType == EPOLL_CTL_DEL)
   {
      offs = (U16)con->mask;
      cb = &(*o->bucket[offs >> 8])[offs & 0xFF];
      if( cb->con != con )
      {
         /* Already removed i.e. deactivateSend+deactivateRec and non
          * valid connection.
          */
         return;
      }
      baAssert( cb->con->mask == con->mask );
      baAssert( (con->mask & 0x0000FFFF) == offs );
      cb->con=0;
      con->mask=0;
      if(o->freeList)
      {
         o->freeListTail->con = (SoDispCon*)cb; /* use as next ptr */
         o->freeListTail = cb;
      }
      else
         o->freeList=o->freeListTail=cb;
   }
   else
   {
      if(SoDispCon_recEvActive(con))
	ev.events |= EPOLLIN;
      if(SoDispCon_sendEvActive(con))
	ev.events |= EPOLLOUT;
      ev.events |= EPOLLHUP | EPOLLERR;
      if(ctlType == EPOLL_CTL_ADD)
      {
         if(o->freeList)
         {
            cb=o->freeList;
            o->freeList=(SoDispConBucket*)cb->con; /* con used as next ptr */
            offs=(U16)cb->mask;
         }
         else
         {
            SoDispConBucketTab* bt;
            U16 bix=o->bucketIx >> 8;
            if(bix >= BA_MAX_BUCKETS)
               baFatalE(FE_SOCKET,0);
            bt = o->bucket[bix];
            if(!bt)
            {
               bt = (SoDispConBucketTab*)malloc(sizeof(SoDispConBucketTab));
               if(!bt)
                  baFatalE(FE_MALLOC,0);
               o->bucket[bix]=bt;
               memset(bt,0,sizeof(SoDispConBucketTab));
            }
            cb=&(*bt)[o->bucketIx & 0xFF];
            offs=o->bucketIx++;
         }
         o->nextId++;
         if(o->nextId == 0) o->nextId++;
         baAssert(con->mask == 0);
         cb->con=con;
         cb->mask=con->mask = ((U32)o->nextId << 16) | offs;
      }
   }
   ev.data.u32=con->mask;
   if(epoll_ctl(o->epfd, ctlType, SoDispCon_getId(con), &ev) < 0 && 
      ctlType != EPOLL_CTL_DEL)
   {
      TRPR(("epoll_ctl failed on %d:  %s\n",
            SoDispCon_getId(con),strerror(errno)));
      SoDispCon_closeCon(con);
      if( ! DoubleList_isInList(&o->termList, &con->dispatcherLink) )
         DoubleList_insertLast(&o->termList, &con->dispatcherLink);
   }
}


BA_API void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   DoubleList_constructor(&o->termList);
   /* Unlikely that we need more than this for changed states */
   o->maxevents = 20;
   o->epfd = epoll_create(o->maxevents); /* arg ignored by epoll_create */
   o->mutex = mutex;
   o->defaultPollDelay = o->pollDelay = 1000;
   o->doExit = FALSE;
   o->nextId=1;
   o->events = (struct epoll_event*)baMalloc(
      sizeof(struct  epoll_event) * o->maxevents);
   if(!o->events)
      baFatalE(FE_MALLOC,0);
   HttpTrace_printf(0, "EPOLL dispatcher; maxcon: %u\n",BA_MAX_BUCKETS*256);
}

void
_SoDisp_destructor(struct SoDisp* o)
{
   if(o->events)
   {
      int ix = (int)o->bucketIx;
      baFree(o->events);
      o->events=0;
      while(--ix >= 0)
         baFree(o->bucket[ix]);
   }
}


BA_API void
SoDisp_newCon(SoDisp* o, struct SoDispCon* con)
{
   /* not used */
   (void)o;
   (void)con;
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
   int ctlType = SoDispCon_sendEvActive(con) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
   baAssert(!SoDispCon_recEvActive(con));
   if(SoDispCon_isValid(con))
   {
      SoDispCon_setRecEvActive(con);
      SoDisp_changeConState(o,con,ctlType);
   }
   else if( ! DoubleList_isInList(&o->termList, &con->dispatcherLink) )
      DoubleList_insertLast(&o->termList, &con->dispatcherLink);
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   int ctlType;
   if(SoDispCon_sendEvActive(con) && SoDispCon_isValid(con))
      ctlType=EPOLL_CTL_MOD;
   else
   {
      ctlType=EPOLL_CTL_DEL;
      if(DoubleLink_isLinked(&con->dispatcherLink))
         DoubleLink_unlink(&con->dispatcherLink);
   }
   baAssert(SoDispCon_recEvActive(con));
   SoDispCon_setRecEvInactive(con);
   SoDisp_changeConState(o,con,ctlType);
}


void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   int ctlType = SoDispCon_recEvActive(con) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
   baAssert(!SoDispCon_sendEvActive(con));
   if(SoDispCon_isValid(con))
   {
      SoDispCon_setSendEvActive(con);
      SoDisp_changeConState(o,con,ctlType);
   }
   else if( ! DoubleList_isInList(&o->termList, &con->dispatcherLink) )
      DoubleList_insertLast(&o->termList, &con->dispatcherLink);
}


void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   int ctlType;
   if(SoDispCon_recEvActive(con) && SoDispCon_isValid(con))
      ctlType=EPOLL_CTL_MOD;
   else
   {
      ctlType=EPOLL_CTL_DEL;
      if(DoubleLink_isLinked(&con->dispatcherLink))
         DoubleLink_unlink(&con->dispatcherLink);
   }
   baAssert(SoDispCon_sendEvActive(con));
   SoDispCon_setSendEvInactive(con);
   SoDisp_changeConState(o,con,ctlType);
}


void
SoDisp_removeConnection(SoDisp* o, SoDispCon* con)
{
   (void)o; /* not used */
   baAssert(SoDispCon_dispatcherHasCon(con));
   baAssert(!SoDispCon_recEvActive(con));
   baAssert(!SoDispCon_sendEvActive(con));
   SoDispCon_clearDispatcherHasCon(con);
   if(DoubleLink_isLinked(&con->dispatcherLink))
      DoubleLink_unlink(&con->dispatcherLink);
}

void
SoDisp_run(SoDisp* o, S32 timeout)
{
   int n;
   int modTimeout;
   SoDisp_mutexSet(o);
   o->doExit = FALSE;
   if(timeout < 0) timeout=-1;
   do
   {
      if(timeout >= 0)
         modTimeout=timeout;
      else
         modTimeout=o->pollDelay;
      SoDisp_mutexRelease(o);
      n = epoll_wait(o->epfd,o->events,o->maxevents,modTimeout);
      SoDisp_mutexSet(o);
      if(n > 0)
      {
         struct epoll_event* ev=o->events;
         struct epoll_event* end=ev+n;
         for(; ev < end ; ev++)
         {
            SoDispCon* con;
            U16 offs;
            offs = (U16)ev->data.u32;
            con=(*o->bucket[offs >> 8])[offs & 0xFF].con;
            if(con && con->mask == ev->data.u32)
            {
               if((ev->events&(EPOLLHUP|EPOLLERR)) || !SoDispCon_isValid(con))
               {
                  if(SoDispCon_sendEvActive(con))
                     SoDispCon_dispSendEvent(con);
                  else
                  {
                     SoDispCon_setDispHasRecData(con);
                     SoDispCon_dispRecEvent(con);
                  }
                  if(con == (*o->bucket[offs >> 8])[offs & 0xFF].con &&
                     SoDispCon_isValid(con))
                  {
                     SoDispCon_closeCon(con);
                     goto L_rec;
                  }
               }
               else
               {
                  if(ev->events & EPOLLOUT)
                  {
                     SoDispCon_dispSendEvent(con);
                     if( (ev->events & EPOLLIN) &&
                         con == (*o->bucket[offs >> 8])[offs & 0xFF].con )
                     {
                        goto L_rec;
                     }
                  }
                  else if(ev->events & EPOLLIN)
                  {
                    L_rec:
                     SoDispCon_setDispHasRecData(con);
                     SoDispCon_dispRecEvent(con);
                  }
               }
            }
            else
            {
#ifdef BA_DEBUG
               if(con)
                  TRPR(("EVNF %x:%x\n",ev->data.u32,con->mask));
               else
                  TRPR(("EVNF %x\n",ev->data.u32));
#endif
            }
         }
      }
      else if(n == 0)
      {
         while( ! DoubleList_isEmpty(&o->termList) )
         {
            DoubleLink* l = DoubleList_removeFirst(&o->termList);
            SoDispCon* con = link2Con(l);
            TRPR(("TERMLIST %x\n",con->mask));
            if(SoDispCon_sendEvActive(con))
               SoDispCon_dispSendEvent(con);
            else
            {
               SoDispCon_setDispHasRecData(con);
               SoDispCon_dispRecEvent(con);
            }
         }
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
      else if(EINTR != errno)
      {
         TRPR(("epoll_wait failed: %s\n",strerror(errno)));
      }
   } while( (timeout < 0 || n > 0) && ! o->doExit );
   SoDisp_mutexRelease(o);
}
