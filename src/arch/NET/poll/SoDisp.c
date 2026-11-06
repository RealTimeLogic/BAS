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
 *   $Id: SoDisp.c 6746 2026-11-04 17:20:37Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2026
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
 *  Portable POSIX poll dispatcher. Use the matching poll HttpCfg.h with
 *  this source. No socket wait in this port depends on FD_SETSIZE.
 *
 *  The poll array is a snapshot owned by the dispatcher thread. Removing
 *  a connection clears its snapshot entry before the connection can be
 *  freed. Entries are never reused until the next wait, so even reuse of
 *  the same address and fd cannot deliver an old event to a new object.
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
#include <limits.h>
#include <time.h>

/* Normal input matches select's read set, without requesting band data
   which the normal recv path would leave pending on some POSIX systems. */
#define BA_POLL_READ POLLRDNORM
#define BA_POLL_ERROR (POLLERR | POLLHUP | POLLNVAL)

#define link2Con(l) \
 (SoDispCon*)((U8*)l - offsetof(SoDispCon,dispatcherLink))

extern UserDefinedErrHandler barracudaUserDefinedErrHandler;

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
                       ecode1, ecode2, file, line);
      HttpTrace_flush();
   }
#endif
   if(barracudaUserDefinedErrHandler)
      (*barracudaUserDefinedErrHandler)(ecode1, ecode2, file, line);
   else
   {
      for(;;) Thread_sleep(100000);
   }
#ifdef HTTP_TRACE
   recursiveCall=0;
#endif
}


/* Timeout is in milliseconds; negative means infinite. Preserve the
   deadline across EINTR and split waits longer than poll's int range. */
static int
SoDisp_pollWait(struct pollfd* fds, nfds_t count, S64 timeout)
{
   struct timespec start;
   S64 remaining=timeout;
   if(timeout > 0 && clock_gettime(CLOCK_MONOTONIC, &start))
      return -1;
   for(;;)
   {
      int status;
      int delay = remaining < 0 ? -1 :
         (remaining > INT_MAX ? INT_MAX : (int)remaining);
      status=poll(fds, count, delay);
      if(status > 0 || (status < 0 && errno != EINTR))
         return status;
      if(timeout == 0)
      {
         if(status == 0)
            return 0;
      }
      else if(timeout > 0)
      {
         struct timespec now;
         S64 elapsed;
         if(clock_gettime(CLOCK_MONOTONIC, &now))
            return -1;
         elapsed = ((S64)now.tv_sec - start.tv_sec) * 1000 +
            (now.tv_nsec - start.tv_nsec) / 1000000;
         remaining=timeout - elapsed;
         if(remaining <= 0)
         {
            nfds_t i;
            /* An interrupted poll need not initialize revents. */
            for(i=0; i < count; i++)
               fds[i].revents=0;
            return 0;
         }
      }
   }
}


BA_API int
BaAddrinfo_platConnect(BaAddrinfo* addr, HttpSocket* s, U32 timeout)
{
   int status;
   HttpSocket_setNonblocking(s, &status);
   if(status)
      goto L_failed;
   status=socketConnect(s->hndl, addr->ai_addr, addr->ai_addrlen);
   if(status == 0)
      return 1;
   if(errno != EINTR)
   {
      HttpSocket_wouldBlock(s, &status);
      if( ! status )
         goto L_failed;
   }
   if(timeout)
   {
      struct pollfd fd;
      socklen_t size=sizeof(status);
      fd.fd=s->hndl;
      fd.events=POLLOUT;
      fd.revents=0;
      if(SoDisp_pollWait(&fd, 1, timeout) <= 0 ||
         (fd.revents & POLLNVAL))
         goto L_failed;
      /* Writable also reports a failed asynchronous connect. */
      if(getsockopt(s->hndl, SOL_SOCKET, SO_ERROR, &status, &size) || status)
         goto L_failed;
      return 1;
   }
   return 0; /* Pending; the dispatcher will report write readiness. */

L_failed:
   HttpSocket_close(s);
   return E_CANNOT_CONNECT;
}


/* Called with the supplied mutex held, or with exclusive ownership when
   no mutex is supplied. The caller's termination flag survives destruction
   of o while the mutex is released. Never inspect o before checking it. */
int
SoDispCon_platReadData(SoDispCon* o, ThreadMutex* m, BaBool* isTerminated,
                       void* data, int len)
{
   int status;
   SoDisp* disp=0;
   int timeout=(int)o->rtmo * 50;
   struct pollfd fd;
   if(m && ! ThreadMutex_isOwner(m))
      m=0;
   fd.fd=o->httpSocket.hndl;
   fd.events=BA_POLL_READ;
   fd.revents=0;
   o->rtmo=0;
   if(timeout && SoDispCon_recEvActive(o))
   {
      disp=SoDispCon_getDispatcher(o);
      SoDisp_deactivateRec(disp, o);
   }
   if(m) ThreadMutex_release(m);
   status = timeout ? SoDisp_pollWait(&fd, 1, timeout) : 1;
   if(status > 0 && !(fd.revents & POLLNVAL))
   {
      do
      {
         status=(int)recv(fd.fd, data, len, 0);
      } while(status < 0 && errno == EINTR);
      if(status == 0)
         status=E_SOCKET_READ_FAILED; /* Graceful disconnect. */
      else if(status < 0)
         status = errno == EAGAIN || errno == EWOULDBLOCK ?
            0 : E_SOCKET_READ_FAILED;
   }
   else
      status = status == 0 ? E_TIMEOUT : E_SOCKET_READ_FAILED;
   if(m)
   {
      ThreadMutex_set(m);
      if(*isTerminated)
         return E_SOCKET_READ_FAILED;
   }
   if(status >= 0 || status == E_TIMEOUT)
   {
      if(disp && SoDispCon_dispatcherHasCon(o) && SoDispCon_isValid(o) &&
         !SoDispCon_recEvActive(o))
         SoDisp_activateRec(disp, o);
      if(status == E_TIMEOUT)
         return status;
   }
   if( ! SoDispCon_isNonBlocking(o) )
      SoDispCon_clearSocketHasNonBlockData(o);
   if(status < 0)
   {
      SoDispCon_closeCon(o);
      return E_SOCKET_READ_FAILED;
   }
   return status;
}


BA_API void
SoDisp_constructor(SoDisp* o, ThreadMutex* mutex)
{
   memset(o, 0, sizeof(SoDisp));
   o->mutex=mutex;
   DoubleList_constructor(&o->conList);
   o->defaultPollDelay=o->pollDelay=1000;
}


BA_API void
SoDisp_pollDestructor(SoDisp* o)
{
   /* Stop run and remove all connections before destroying the dispatcher. */
   baAssert(o->pollCount == 0);
   baAssert(DoubleList_isEmpty(&o->conList));
   baFree(o->pollSet);
   baFree(o->pollCon);
   o->pollSet=0;
   o->pollCon=0;
   o->pollCapacity=0;
}


BA_API void
SoDisp_newCon(SoDisp* o, SoDispCon* con)
{
   (void)o;
   DoubleLink_constructor(&con->dispatcherLink);
   con->pollIndex=-1;
}


BA_API void
SoDisp_addConnection(SoDisp* o, SoDispCon* con)
{
   (void)o;
   baAssert(!SoDispCon_dispatcherHasCon(con));
   SoDispCon_setDispatcherHasCon(con);
}


/* Invalidate interests in the current snapshot, including a disable/enable
   pair during a callback or wait. Newly enabled interests wait for the next
   snapshot. pollSet itself must remain untouched while poll() is running. */
static void
SoDisp_clearPollInterest(SoDisp* o, SoDispCon* con, short events)
{
   int ix=con->pollIndex;
   if(ix >= 0)
   {
      SoDispPollCon* pc=o->pollCon + ix;
      baAssert(ix < o->pollCount && pc->con == con);
      pc->events &= (short)~events;
      if( ! pc->events )
      {
         pc->con=0;
         con->pollIndex=-1;
      }
   }
}


BA_API void
SoDisp_activateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(!SoDispCon_recEvActive(con));
   if( ! SoDispCon_sendEvActive(con) )
      DoubleList_insertLast(&o->conList, &con->dispatcherLink);
   SoDispCon_setRecEvActive(con);
}


void
SoDisp_deactivateRec(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_recEvActive(con));
   SoDisp_clearPollInterest(o, con, BA_POLL_READ);
   SoDispCon_setRecEvInactive(con);
   if( ! SoDispCon_sendEvActive(con) )
      DoubleLink_unlink(&con->dispatcherLink);
}


#if BA_HAVE_SOCKET_WRITE_EVENTS
void
SoDisp_activateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_isValid(con));
   baAssert(!SoDispCon_sendEvActive(con));
   if( ! SoDispCon_recEvActive(con) )
      DoubleList_insertLast(&o->conList, &con->dispatcherLink);
   SoDispCon_setSendEvActive(con);
}


void
SoDisp_deactivateSend(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_sendEvActive(con));
   SoDisp_clearPollInterest(o, con, POLLOUT);
   SoDispCon_setSendEvInactive(con);
   if( ! SoDispCon_recEvActive(con) )
      DoubleLink_unlink(&con->dispatcherLink);
}
#endif


void
SoDisp_removeConnection(SoDisp* o, SoDispCon* con)
{
   baAssert(SoDispCon_dispatcherHasCon(con));
   baAssert(!SoDispCon_recEvActive(con));
   baAssert(!SoDispCon_sendEvActive(con));
   SoDisp_clearPollInterest(o, con, BA_POLL_READ | POLLOUT);
   SoDispCon_clearDispatcherHasCon(con);
}


/* Allocate both arrays before replacing either old allocation. This is
   called only by the dispatcher thread while it holds the mutex. */
static int
SoDisp_growPollSet(SoDisp* o)
{
   int capacity;
   size_t size;
   struct pollfd* fds;
   SoDispPollCon* cons;
   if(o->pollCapacity > INT_MAX/2)
      goto L_failed;
   capacity = o->pollCapacity ? o->pollCapacity * 2 : 32;
   if((size_t)capacity > (size_t)-1 / sizeof(SoDispPollCon) ||
      (size_t)capacity > (size_t)-1 / sizeof(struct pollfd) ||
      (nfds_t)capacity != (size_t)capacity)
      goto L_failed;
   size=(size_t)capacity * sizeof(SoDispPollCon);
   cons=(SoDispPollCon*)baMalloc(size);
   if( ! cons )
      goto L_failed;
   fds=(struct pollfd*)baMalloc((size_t)capacity * sizeof(struct pollfd));
   if( ! fds )
   {
      baFree(cons);
      goto L_failed;
   }
   if(o->pollCount)
   {
      memcpy(fds, o->pollSet, (size_t)o->pollCount * sizeof(struct pollfd));
      memcpy(cons, o->pollCon, (size_t)o->pollCount * sizeof(SoDispPollCon));
   }
   baFree(o->pollSet);
   baFree(o->pollCon);
   o->pollSet=fds;
   o->pollCon=cons;
   o->pollCapacity=capacity;
   return 0;

L_failed:
   baFatalE(FE_MALLOC, 0);
   return -1;
}


static void
SoDisp_clearPollSet(SoDisp* o)
{
   int i;
   for(i=0; i < o->pollCount; i++)
   {
      if(o->pollCon[i].con)
         o->pollCon[i].con->pollIndex=-1;
   }
   o->pollCount=0;
}


void
SoDisp_run(SoDisp* o, S32 timeout)
{
   int descriptors;
   SoDisp_mutexSet(o);
   o->doExit=FALSE;
   do
   {
      DoubleListEnumerator iter;
      DoubleLink* l;
      int i;
      int delay = timeout < 0 ? o->pollDelay : timeout;
      DoubleListEnumerator_constructor(&iter, &o->conList);
      for(l=DoubleListEnumerator_getElement(&iter); l;
          l=DoubleListEnumerator_nextElement(&iter))
      {
         SoDispCon* con=link2Con(l);
         struct pollfd* fd;
         if(o->pollCount == o->pollCapacity && SoDisp_growPollSet(o))
            goto L_exit;
         i=o->pollCount++;
         fd=o->pollSet + i;
         fd->fd=con->httpSocket.hndl;
         fd->events = SoDispCon_recEvActive(con) ? BA_POLL_READ : 0;
         if(SoDispCon_sendEvActive(con))
            fd->events |= POLLOUT;
         fd->revents=0;
         o->pollCon[i].con=con;
         o->pollCon[i].events=fd->events;
         con->pollIndex=i;
         if( ! SoDispCon_isValid(con) )
            delay=0; /* Report invalidated sockets even under continuous I/O. */
      }

      SoDisp_mutexRelease(o);
      descriptors=SoDisp_pollWait(o->pollSet, (nfds_t)o->pollCount, delay);
      SoDisp_mutexSet(o);
      if(descriptors >= 0)
      {
         for(i=0; i < o->pollCount && !o->doExit; i++)
         {
            SoDispPollCon* pc=o->pollCon + i;
            SoDispCon* con=pc->con;
            short events=o->pollSet[i].revents;
            if( ! con )
               continue;
            if( ! SoDispCon_isValid(con) )
            {
               events |= POLLNVAL;
               if(descriptors == 0)
                  descriptors=1;
            }
            if((pc->events & POLLOUT) &&
               (events & (POLLOUT | BA_POLL_ERROR)))
               SoDispCon_dispSendEvent(con);

            /* A callback may close, move, free, or disable this connection.
               The snapshot entry is cleared before its storage is reused. */
            con=pc->con;
            if(con && (pc->events & BA_POLL_READ) &&
               (events & (BA_POLL_READ | BA_POLL_ERROR)))
            {
               /* HUP can accompany unread data. Let recv drain that data
                  and observe EOF instead of closing the socket here. */
               SoDispCon_setDispHasRecData(con);
               SoDispCon_dispRecEvent(con);
            }
         }
      }
      SoDisp_clearPollSet(o);
      if(descriptors == 0 && o->pollDelay != o->defaultPollDelay)
      {
         if(o->pollDelay == 0)
            o->pollDelay=o->defaultPollDelay;
         else
         {
            o->pollDelay += o->pollDelay/15;
            if(o->pollDelay > o->defaultPollDelay)
               o->pollDelay=o->defaultPollDelay;
         }
      }
      else if(descriptors < 0)
      {
         SoDisp_mutexRelease(o);
         Thread_sleep(10); /* Avoid spinning on resource exhaustion. */
         SoDisp_mutexSet(o);
      }
   } while((timeout < 0 || descriptors > 0) && !o->doExit);

L_exit:
   SoDisp_clearPollSet(o);
   SoDisp_mutexRelease(o);
}
