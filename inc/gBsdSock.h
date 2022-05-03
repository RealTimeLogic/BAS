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
 *                            HEADER
 *
 *   $Id: gBsdSock.h 5141 2022-05-03 18:45:53Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2004 - 2022
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
 *               http://realtimelogic.com
 ****************************************************************************
 *
 *
 *              Do not directly include this file!!!
 *  gBsdSock -> Generic BSD socket functions.
 */
#ifndef _gBsdSock_h
#define _gBsdSock_h

#ifdef __cplusplus
extern "C" {
#endif

struct HttpSockaddr;

#ifndef basocklen_t
#define basocklen_t socklen_t
#endif

#ifndef SocketHandle
#define SocketHandle int
#endif

#ifndef socketAccept
#define socketAccept accept
#endif

#ifndef socketBind
#define socketBind bind
#endif

#ifndef socketClose
#define socketClose close
#endif

#ifndef socketShutdown
#define socketShutdown shutdown
#endif

#ifndef socketErrno
#define socketErrno(s) errno
#endif

/* for setting blocking/non-blocking sockets */
#ifndef socketIoctl
#define socketIoctl ioctl
#endif

#ifndef socketGetPeerName
#define socketGetPeerName getpeername
#endif

#ifndef socketGetSockName
#define socketGetSockName getsockname
#endif

#ifndef socketListen
#define socketListen listen
#endif

#ifndef socketRecv
#define socketRecv recv
#endif

#ifndef socketSelect
#define socketSelect select
#endif

#ifndef socketSend
#define socketSend send
#endif

#ifndef socketSendto
#define socketSendto sendto
#endif

#ifndef socketRecvfrom
#define socketRecvfrom recvfrom
#endif

#ifndef socketGetsockopt
#define socketGetsockopt getsockopt
#endif

#ifndef socketSetsockopt
#define socketSetsockopt setsockopt
#endif

#ifndef socketSocket
#define socketSocket socket
#endif

#ifndef socketInetAddr
#define socketInetAddr inet_addr
#endif

#ifndef socketInetPton
#define socketInetPton inet_pton
#endif

#ifndef socketGethostbyname
#define socketGethostbyname gethostbyname
#endif

#ifndef socketInetNtoa
#define socketInetNtoa inet_ntoa
#endif

#ifndef socketWouldBlock
#define socketWouldBlock(s) (EWOULDBLOCK == socketErrno(s))
#endif

#ifndef socketInProgress
#ifdef EINPROGRESS
#define socketInProgress(s) (EINPROGRESS == socketErrno(s))
#else
#define socketInProgress(s) 0
#endif
#endif

#ifndef socketConnect
#define socketConnect connect
#endif

#ifndef BaSockAddrStorage
#define BaSockAddrStorage struct sockaddr_storage
#endif



/* FD_CLOEXEC if applicable for platform */
#ifndef HttpSocket_setcloexec
#define HttpSocket_setcloexec(x)
#endif
#ifndef HttpSocket_clearcloexec
#define HttpSocket_clearcloexec(x)
#endif
#ifndef basocketpair
#define basocketpair(x)
#endif

typedef struct {
      SocketHandle hndl;
} HttpSocket;


#ifndef HttpSocket_accept
#define HttpSocket_accept(o, conSock, status) do { \
   (conSock)->hndl=socketAccept((o)->hndl, NULL, NULL); \
   *(status) = (HttpSocket_isValid(conSock)) ? 0 : -1; \
   HttpSocket_setcloexec(o); \
}while(0)
#endif


#ifndef HttpSocket_bind
#define HttpSocket_bindIp4(o, sockAddr, port, status) do { \
   struct sockaddr_in in; \
   memset(&in, 0, sizeof(struct sockaddr_in)); \
   in.sin_family = AF_INET; \
   in.sin_port = baHtons(port); \
   memcpy(&in.sin_addr.s_addr,(sockAddr)->addr, 4); \
   *(status) = socketBind((o)->hndl, (struct sockaddr *)&in, \
                          sizeof(struct sockaddr_in)); \
} while(0)
#ifdef USE_IPV6
#define HttpSocket_bindIp6(o, sockAddr, port, status) do { \
      struct sockaddr_in6 in6; \
      memset(&in6, 0, sizeof(struct sockaddr_in6)); \
      in6.sin6_family = AF_INET6; \
      in6.sin6_port = baHtons(port); \
      memcpy(&in6.sin6_addr, (sockAddr)->addr, 16); \
      *(status) = socketBind((o)->hndl, (struct sockaddr*)&in6, \
                       sizeof(struct sockaddr_in6)); \
} while(0)
#else
#define HttpSocket_bindIp6(o, sockAddr, port, status) baAssert(0)
#endif
#define HttpSocket_bind(o, sockAddr, port, status) do { \
   if((sockAddr)->isIp6) \
      HttpSocket_bindIp6(o, sockAddr, port, status); \
   else \
      HttpSocket_bindIp4(o, sockAddr, port, status); \
}while(0)
#endif

#ifndef HttpSocket_close
#define HttpSocket_close(o) do {\
   int status;\
   HttpSocket_setNonblocking(o,&status);\
   (void)status; \
   socketClose((o)->hndl);\
   HttpSocket_invalidate(o);\
 } while(0)
#endif


/* Perform a hard (abortive) close */
#ifndef HttpSocket_hardClose
#define HttpSocket_hardClose(o) do {\
   struct linger l;\
   l.l_onoff = 1; l.l_linger = 0;\
   socketSetsockopt((o)->hndl,SOL_SOCKET,SO_LINGER,     \
                    (char*)&l,sizeof(struct linger));   \
   socketClose((o)->hndl);\
   HttpSocket_invalidate(o);\
 } while(0)
#endif


/* SHUT_RDWR or 2 Further sends and receives are disallowed */
#ifndef HttpSocket_shutdown
#define HttpSocket_shutdown(o) do {\
   int status;\
   HttpSocket_setBlocking(o,&status);\
   (void)status; \
   socketShutdown((o)->hndl,2);\
   socketClose((o)->hndl);\
   HttpSocket_invalidate(o);\
 } while(0)
#endif


#ifndef HttpSocket_constructor
#define HttpSocket_constructor(o) do { \
   memset(o, 0, sizeof(HttpSocket)); \
   HttpSocket_invalidate(o); \
}while(0)
#endif

#ifndef HttpSocket_errno
#define HttpSocket_errno(o, status, ecode) (void)status,*(ecode) = socketErrno((o)->hndl)
#endif

#ifndef HttpSocket_getId
#define HttpSocket_getId(o) (int)(o)->hndl
#endif

#ifndef HttpSocket_getPeerName
#ifdef USE_IPV6
#define HttpSocket_getPeerName(o, addrMA, port, isIp6MA, status) do {\
   BaSockAddrStorage a;\
    basocklen_t aLen = sizeof(a);\
   if( !(*(status) = socketGetPeerName((o)->hndl, (struct sockaddr *)&a, &aLen)) )\
   {\
      if(isIp6MA){\
         memcpy(&(addrMA)->addr, &((struct sockaddr_in6*)&a)->sin6_addr, 16);\
         if(port) *(port)=baHtons(((struct sockaddr_in6*)&a)->sin6_port);\
	  }\
      else {\
         *((U32*)(addrMA)->addr) = ((struct sockaddr_in*)&a)->sin_addr.s_addr;\
         if(port) *(port)=baHtons(((struct sockaddr_in*)&a)->sin_port);\
	  }\
      (addrMA)->isIp6=isIp6MA; \
   }\
} while(0)
#else
#define HttpSocket_getPeerName(o, sockAddr, port, isIp6MA, status) do {   \
   struct sockaddr_in in; basocklen_t size=sizeof(struct sockaddr_in);\
   (void)isIp6MA;                                                         \
   *(status) = socketGetPeerName((o)->hndl, (struct sockaddr *)&in, &size); \
   memcpy((sockAddr)->addr, &in.sin_addr.s_addr, 4);\
   if(port) *(port)=baHtons(in.sin_port);\
   (sockAddr)->isIp6=FALSE; \
} while(0)
#endif
#endif

#ifndef HttpSocket_getSockName
#ifdef USE_IPV6
#define HttpSocket_getSockName(o, addrMA, port, isIp6MA, status) do {\
   BaSockAddrStorage a;\
    basocklen_t aLen = sizeof(a);\
   if( !(*(status) = socketGetSockName((o)->hndl,(struct sockaddr*)&a,&aLen)) )\
   {\
      if(isIp6MA){\
         memcpy(&(addrMA)->addr, &((struct sockaddr_in6*)&a)->sin6_addr, 16);\
         if(port) *(port)=baHtons(((struct sockaddr_in6*)&a)->sin6_port);\
	  }\
      else {\
         *((U32*)(addrMA)->addr) = ((struct sockaddr_in*)&a)->sin_addr.s_addr;\
         if(port) *(port)=baHtons(((struct sockaddr_in*)&a)->sin_port);\
	  }\
      (addrMA)->isIp6=isIp6MA; \
   }\
} while(0)
#else
#define HttpSocket_getSockName(o, sockAddr, port, isIp6MA, status) do {   \
   struct sockaddr_in in; basocklen_t size=sizeof(struct sockaddr_in);\
   (void)isIp6MA;                                                         \
   *(status) = socketGetSockName((o)->hndl, (struct sockaddr *)&in, &size); \
   memcpy((sockAddr)->addr, &in.sin_addr.s_addr, 4);\
   if(port) *(port)=baHtons(in.sin_port);\
   (sockAddr)->isIp6=FALSE; \
} while(0)
#endif
#endif

#ifndef HttpSocket_invalidate
#define HttpSocket_invalidate(o) (o)->hndl = -1
#endif

#ifndef HttpSocket_isValid
#define HttpSocket_isValid(o) ((o)->hndl >= 0)
#endif

#ifndef HttpSocket_listen
#define HttpSocket_listen(o, sockaddrNotUsed, queueSize, status) do { \
   *(status)=socketListen((o)->hndl, queueSize); \
   HttpSocket_setcloexec(o); \
}while(0)
#endif

#ifndef HttpSocket_move
#define HttpSocket_move(o, newS) do{ \
  (newS)->hndl=(o)->hndl;HttpSocket_invalidate(o);}while(0)
#endif

#ifndef HttpSocket_recv
#define HttpSocket_recv(o, data, len, retLen) do { \
      *(retLen)=(int)socketRecv((o)->hndl,data,len,0);  \
  if(*(retLen) <= 0) { \
     if(*(retLen) < 0) { \
        int wb; \
        HttpSocket_wouldBlock(o, &wb); \
        if(wb) \
           *(retLen)=0;  /* No data */ \
     } \
     else *(retLen) = -1; /* graceful disconnect */ \
  } \
} while(0)
#endif

#ifndef HttpSocket_send
#define HttpSocket_send(o, m, isTerminated, data, len, retLen) do { \
  if(m && ThreadMutex_isOwner(m)) { \
    ThreadMutex_release(m); \
    *(retLen)=(int)socketSend((o)->hndl,data,len,0);    \
    ThreadMutex_set(m); \
  } \
  else {                                        \
    *(retLen)=(int)socketSend((o)->hndl,data,len,0); \
  }                                             \
  if(*(retLen) < 0) { \
     int wb; \
     HttpSocket_wouldBlock(o, &wb); \
     if(wb) \
        *(retLen)=0;  /* non blocking, no data sent */ \
  } \
} while(0)
#endif

#ifdef USE_ADDRINFO
#ifndef HttpSocket_sendto
#define HttpSocket_sendto(o, data, len, addr, addrlen, retLen) do { \
    *(retLen)=(int)socketSendto((o)->hndl,data,len,0, addr, addrlen); \
} while(0)
#endif
#else
#ifndef HttpSocket_sendto
#define HttpSocket_sendtoV4(o, data, len, addrMA, port, status) \
   struct sockaddr_in sin;                                      \
   memset((char *)&sin, 0, sizeof(sin));                        \
   sin.sin_family = AF_INET;                                    \
   sin.sin_port = baHtons(port);                                \
   memcpy(&sin.sin_addr.s_addr, (addrMA)->addr, 4);             \
   *(status) = (int)socketSendto((o)->hndl,data,len,0,               \
   (struct sockaddr *)&sin, sizeof(sin));
#ifdef USE_IPV6
#define HttpSocket_sendtoV6(o, data, len, addrMA, port, status)         \
   struct sockaddr_in6 sin6;                                            \
   memset((char *)&sin6, 0, sizeof(sin6));                              \
   sin6.sin6_family =  (addrMA)->isIp6 ? AF_INET6 : AF_INET;            \
   sin6.sin6_port = baHtons(port);                                      \
   memcpy(&sin6.sin6_addr, (addrMA)->addr, 16);                         \
   *(status)=(int)socketSendto((o)->hndl,data,len,0,                         \
  (struct sockaddr *)&sin6,sizeof(sin6));
#define HttpSocket_sendto(o, data, len, addrMA, port, status) do {\
   if((addrMA)->isIp6) {\
      HttpSocket_sendtoV6(o, data, len, addrMA, port, status)\
   }\
   else {\
      HttpSocket_sendtoV4(o, data, len, addrMA, port, status)\
   }\
} while(0)
#else
#define HttpSocket_sendto(o, data, len, addrMA, port, status) do {      \
      HttpSocket_sendtoV4(o, data, len, addrMA, port, status) } while(0)
#endif
#endif
#endif

#ifndef baIoctlArg
#define baIoctlArg size_t
#endif

#ifndef HttpSocket_setBlocking
#define HttpSocket_setBlocking(o, status) do { \
   baIoctlArg arg=0; /*Set blocking mode */ \
  *(status)=socketIoctl((o)->hndl, FIONBIO, &arg); \
} while(0)
#endif

#ifndef HttpSocket_setNonblocking
#define HttpSocket_setNonblocking(o, status) \
do { \
   baIoctlArg arg=1; /* Set non-blocking mode */ \
  *(status)=socketIoctl((o)->hndl, FIONBIO, &arg); \
} while(0)
#endif

#ifndef HttpSocket_setTCPNoDelay
#define HttpSocket_setTCPNoDelay(o, enable, status) do { \
   int enableFlag = enable ? 1 : 0; \
   *(status) = socketSetsockopt((o)->hndl, IPPROTO_TCP, TCP_NODELAY, \
                               (char*)&enableFlag, sizeof(int)); \
}while(0)
#endif

#if !defined(HttpSocket_getKeepAlive) && defined(SO_KEEPALIVE)
#define HttpSocket_getKeepAlive(o,enablePtr,statusPtr) do {     \
      int optval;                                               \
      socklen_t optlen = sizeof(optval);                        \
      *(statusPtr) =                                            \
         socketGetsockopt((o)->hndl,SOL_SOCKET,SO_KEEPALIVE,    \
                          (char*)&optval,&optlen)<0 ?           \
         -1 : 0;                                                \
      *(enablePtr) = optval;                                    \
   } while(0)
#endif

#if !defined(HttpSocket_setKeepAlive) && defined(SO_KEEPALIVE)
#define HttpSocket_setKeepAlive(o,enable,statusPtr) do { \
   int optval=enable; \
   basocklen_t optlen = sizeof(optval); \
   *(statusPtr) = \
      socketSetsockopt((o)->hndl,SOL_SOCKET,SO_KEEPALIVE,       \
                       (char*)&optval,optlen)<0 ?               \
      -1 : 0; \
} while(0)
#endif


#ifndef HttpSocket_soReuseaddr
#ifndef ENABLE_REUSE_ADDR
#define ENABLE_REUSE_ADDR 0
#endif
#if ENABLE_REUSE_ADDR
#define HttpSocket_soReuseaddr(o, status) do { \
   int enableFlag = 1; \
   *(status) = socketSetsockopt((o)->hndl, SOL_SOCKET, SO_REUSEADDR, \
                               (char*)&enableFlag, sizeof(int)); \
}while(0)
#else
#define HttpSocket_soReuseaddr(o, status)
#endif
#endif

#ifndef HttpSocket_soDontroute
#define HttpSocket_soDontroute(o, enableFlag, status) do {       \
   *(status) = socketSetsockopt((o)->hndl, SOL_SOCKET, SO_DONTROUTE, \
                               (char*)&enableFlag, sizeof(int)); \
}while(0)
#endif

#ifndef HttpSocket_soBroadcast
#define HttpSocket_soBroadcast(o, enableFlag, status) do {       \
   *(status) = socketSetsockopt((o)->hndl, SOL_SOCKET, SO_BROADCAST, \
                               (char*)&enableFlag, sizeof(int)); \
}while(0)
#endif


#ifndef HttpSocket_sockStream
#ifdef USE_IPV6
#define HttpSocket_sockStreamIp6(o, status) do { \
   (o)->hndl=socketSocket(AF_INET6, SOCK_STREAM, 0); \
   *(status) = HttpSocket_isValid(o) ? 0 : -1; \
}while(0)
#else
#define HttpSocket_sockStreamIp6(o, status) HttpSocket_sockStreamIp4(o, status)
#endif

#define HttpSocket_sockStreamIp4(o, status) do { \
  (o)->hndl=socketSocket(AF_INET, SOCK_STREAM, 0); \
   *(status) = HttpSocket_isValid(o) ? 0 : -1; \
}while(0)

#define HttpSocket_sockStream(o, interfNameNotUsed, useIp6, status) do { \
   if(useIp6) \
     HttpSocket_sockStreamIp6(o, status); \
   else \
     HttpSocket_sockStreamIp4(o, status); \
}while(0)
#endif

#ifdef USE_DGRAM

#ifndef ba_nametoindex
#define ba_nametoindex(name, resp,status) *(resp)=if_nametoindex(name)
#endif


#ifndef HttpSocket_sockUdp
#ifdef USE_IPV6
#define HttpSocket_sockUdpIp6(o, status) do { \
   (o)->hndl=socketSocket(AF_INET6, SOCK_DGRAM, 0); \
   *(status) = HttpSocket_isValid(o) ? 0 : -1; \
}while(0)
#else
#define HttpSocket_sockUdpIp6(o, status) HttpSocket_sockUdpIp4(o, status)
#endif

#define HttpSocket_sockUdpIp4(o, status) do { \
  (o)->hndl=socketSocket(AF_INET, SOCK_DGRAM, 0); \
   *(status) = HttpSocket_isValid(o) ? 0 : -1; \
}while(0)

#define HttpSocket_sockUdp(o, interfNameNotUsed, useIp6, status) do { \
   if(useIp6) \
     HttpSocket_sockUdpIp6(o, status); \
   else \
     HttpSocket_sockUdpIp4(o, status); \
}while(0)
#endif

#ifndef HttpSocket_recvfrom
#define HttpSocket_recvfromIp4(o,data,len,sockAddr,port,status) do { \
   struct sockaddr_in in; basocklen_t size=sizeof(struct sockaddr_in);\
   *(status)=socketRecvfrom((o)->hndl,data,len,0,(struct sockaddr*)&in,&size); \
   memcpy((sockAddr)->addr, &in.sin_addr.s_addr, 4);\
   *(port)=baHtons(in.sin_port);\
} while(0)
#ifdef USE_IPV6
#define HttpSocket_recvfromIp6(o,data,len,sockAddr,port,status) do { \
  struct sockaddr_in6 in6; basocklen_t size=sizeof(struct sockaddr_in6);\
  *(status)=socketRecvfrom((o)->hndl,data,len,0,(struct sockaddr*)&in6,&size);\
  memcpy(sockAddr, &in6.sin6_addr, 16); \
  *(port)=baHtons(in6.sin6_port);\
} while(0)
#else
#define HttpSocket_recvfromIp6(o,data,len,sockAddr,port,status) baAssert(0)
#endif
#define HttpSocket_recvfrom(o,data,len,sockAddr,port,status) do { \
   if((sockAddr)->isIp6) \
      HttpSocket_recvfromIp6(o,data,len,sockAddr,port,status); \
   else \
      HttpSocket_recvfromIp4(o,data,len,sockAddr,port,status); \
}while(0)
#endif


#ifndef HttpSocket_setmembership
BA_API int HttpSocket_setmembership(HttpSocket* o,
                                    BaBool enable,
                                    BaBool ipv6,
                                    const char* multiAddrName,
                                    const char* intfName);
#endif

#endif /* USE_DGRAM */

#ifndef HttpSocket_wouldBlock
#define HttpSocket_wouldBlock(o, status) do { \
   *(status)=socketWouldBlock((o)->hndl); \
   if(!*(status)) { \
      *(status)=socketInProgress((o)->hndl); \
   } \
} while(0)
#endif


/*
HttpSocket_connect(HttpSocket*, HttpSockaddr*, U16 port, int* status)
*/
#ifndef HttpSocket_connect
#define HttpSocket_connectV4(o, addrMA, port, status) \
   struct sockaddr_in sin; \
   memset((char *)&sin, 0, sizeof(sin)); \
   sin.sin_family = AF_INET; \
   sin.sin_port = baHtons(port); \
   memcpy(&sin.sin_addr.s_addr, (addrMA)->addr, 4);\
   *(status) = socketConnect((o)->hndl,(struct sockaddr *)&sin, sizeof(sin));
#ifdef USE_IPV6
#define HttpSocket_connectV6(o, addrMA, port, status)\
   struct sockaddr_in6 sin6; \
   memset((char *)&sin6, 0, sizeof(sin6)); \
   sin6.sin6_family =  (addrMA)->isIp6 ? AF_INET6 : AF_INET; \
   sin6.sin6_port = baHtons(port); \
   memcpy(&sin6.sin6_addr, (addrMA)->addr, 16); \
   *(status)=socketConnect((o)->hndl,(struct sockaddr *)&sin6,sizeof(sin6));
#define HttpSocket_connect(o, addrMA, port, status) do {\
   if((addrMA)->isIp6) {\
      HttpSocket_connectV6(o, addrMA, port, status)\
   }\
   else {\
      HttpSocket_connectV4(o, addrMA, port, status)\
   }\
} while(0)
#else
#define HttpSocket_connect(o, addr, port, status) do {\
  HttpSocket_connectV4(o, addr, port, status) } while(0)
#endif
#endif



/*
  Returns:
    < 0: Sock error code
      0: OK
*/
BA_API int HttpSocket_create(HttpSocket* o,
                             const char* bindIntfName,
                             U16 port,
                             BaBool dgram,
                             BaBool ipv6);

typedef struct HttpSockaddr {
      char addr[16];
      BaBool isIp6;
} HttpSockaddr;


#ifndef HttpSockaddr_gethostbynameIp6
#ifdef USE_IPV6
#define HttpSockaddr_gethostbynameIp6(o, host, status) do { \
   if(host) { \
      struct addrinfo hints, *servinfo; \
      memset(&hints, 0, sizeof hints); \
      hints.ai_family = AF_INET6; \
      hints.ai_socktype = SOCK_STREAM; \
      *(status) = getaddrinfo(host, 0, &hints, &servinfo); \
      if( ! *(status) ) { \
         memcpy((o)->addr, \
                &((struct sockaddr_in6*)servinfo->ai_addr)->sin6_addr, 16); \
         freeaddrinfo(servinfo); \
      } \
   } \
   else { \
      memset((o)->addr, 0, 16); \
      *(status)=0; \
   } \
   (o)->isIp6=TRUE; \
}while(0)
#else
#define HttpSockaddr_gethostbynameIp6(o, host, status) \
  HttpSockaddr_gethostbynameIp4(o, host, status)
#endif
#endif

#ifndef HttpSockaddr_gethostbyname
#define HttpSockaddr_gethostbynameIp4(o, host, status)  do { \
   unsigned long ipAddr; \
   (o)->isIp6=FALSE; \
   *(status)=0; \
   if(host) \
   { \
      ipAddr = socketInetAddr(host); /* is "host" an IP address ? */ \
      if(ipAddr == INADDR_NONE) /* No, not an IP address. */ \
      { /* Is "host" a hostname ? */\
         struct hostent* hostInfo = socketGethostbyname((char*)host); \
         if(!hostInfo) \
            *(status)=-1; \
         else \
            memcpy(&ipAddr,&((struct in_addr *)hostInfo->h_addr)->s_addr,4);\
      } \
   } \
   else \
      ipAddr = baHtonl(INADDR_ANY); \
   memcpy((o)->addr,&ipAddr, 4); \
}while(0)


#define HttpSockaddr_gethostbyname(o, host, useIp6, status) do { \
   if(useIp6) \
     HttpSockaddr_gethostbynameIp6(o, host, status); \
   else \
     HttpSockaddr_gethostbynameIp4(o, host, status); \
}while(0)
#endif


#ifndef HttpSockaddr_addr2String
BA_API void HttpSockaddr_addr2String(
   HttpSockaddr* o, char* buf, int bufLen, int* status);
#endif


#ifndef HttpSockaddr_inetAddr
BA_API void HttpSockaddr_inetAddr(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);
#endif


#ifdef __cplusplus
}
#endif


#ifdef USE_ADDRINFO

#ifndef BaAddrinfo
typedef struct addrinfo BaAddrinfo;
#endif

#ifndef BaAddrinfo_hintsInit
#define BaAddrinfo_hintsInit(hints, dgram, ipv6) do {           \
      memset(hints, 0, sizeof(BaAddrinfo));                     \
      (hints)->ai_socktype = dgram ? SOCK_DGRAM : SOCK_STREAM;  \
      (hints)->ai_family = ipv6 ? PF_INET6 : PF_INET;           \
   } while(0)
#endif

#ifndef BaAddrinfo_get
#define BaAddrinfo_get(node, service,hints,res,status,errinfo) do {     \
   *(status) = getaddrinfo(node, service, hints, res);                  \
   *(errinfo) = *(status) ? (char*)gai_strerror(*(status)) : 0; \
} while(0)
#endif

#ifndef BaAddrinfo_next
#define BaAddrinfo_next(iter) *((iter)) = (*(iter))->ai_next
#endif

#ifndef BaAddrinfo_free
#define BaAddrinfo_free(addr) freeaddrinfo(addr)
#endif

#ifndef BaAddrinfo_isIp6
#define BaAddrinfo_isIp6(addr) (addr)->ai_family == AF_INET6
#endif

#ifndef BaAddrinfo_connect
int BaAddrinfo_connect(BaAddrinfo* addr, HttpSocket* s, U32 timeout);
#endif

#ifndef BaAddrinfo_sendto
#define BaAddrinfo_sendto(addr, s, data, len, status)                   \
   HttpSocket_sendto(s, data, len, (addr)->ai_addr,                     \
                     (socklen_t)(addr)->ai_addrlen, status)
#endif

#endif /* USE_ADDRINFO */


#endif
