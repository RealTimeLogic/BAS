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
 *   $Id: HttpCfg.h 4915 2021-12-01 18:26:55Z wini $
 *   COPYRIGHT:  Real Time Logic, 2009 - 2021
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
 * Platform: ThreadX.
 */

#ifndef _HttpConfig_h
#define _HttpConfig_h

/* No support for asynchronous response messages in NetX.
*/
#define NO_ASYNCH_RESP

#include <TargConfig.h>
#include <NetConv.h>
#include <DoubleList.h>
#include <nx_api.h>


#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
 *  Forward decl.
 ***********************************************************************/

struct SoDisp;
struct HttpSockaddr;
struct ThreadMutex;

/***********************************************************************
 *  HttpSockaddr
 ***********************************************************************/

typedef struct HttpSockaddr
{
      NX_IP* ip;
      U8 addr[16]; /* Must be long aligned */
      BaBool isIp6;
} HttpSockaddr;


void HttpSockaddr_gethostbyname(HttpSockaddr*, const char*, BaBool, int*);


/***********************************************************************
 *  HttpSocket
 ***********************************************************************/

typedef struct HttpSocket
{
      DoubleLink super; /*Used when in SoDisp queue. Inserted by NX callback*/
      NX_TCP_SOCKET* nxSock;
      NX_PACKET* nxFirstPacket;
      NX_PACKET* nxPacket;
      UCHAR* nxPktOffs;
} HttpSocket;


#define HttpSocket_constructor(o) memset(o, 0, sizeof(HttpSocket))

#define HttpSocket_invalidate(o) HttpSocket_nxClose(o, TRUE)
#define HttpSocket_shutdown(o) HttpSocket_nxClose(o, TRUE)
#define HttpSocket_hardClose(o) HttpSocket_nxClose(o, TRUE)
#define HttpSocket_close(o) HttpSocket_nxClose(o, FALSE)

#define HttpSocket_errno(o, status, ecode) (*(ecode) = status)

#define HttpSocket_setBlocking(o, status) *(status)=0

#define HttpSocket_setNonblocking(o, status) *(status)=0

#define HttpSocket_wouldBlock(o, status) TRUE

#define HttpSocket_isValid(o) ((o)->nxSock ? TRUE : FALSE)

#define HttpSocket_setTCPNoDelay(o, enable, status) \
  (*(status)=0)

#define HttpSocket_soReuseaddr(o, status) \
  (*(status)=0)

BA_API int HttpSocket_create(HttpSocket* o,
                             const char* bindIntfName,
                             U16 port,
                             BaBool dgram,
                             BaBool ipv6);
void HttpSocket_sockStream(HttpSocket*,const char*,BaBool,int*);
void HttpSocket_bind(HttpSocket*,HttpSockaddr*,UINT,int*);
void HttpSocket_listen(HttpSocket*,HttpSockaddr*,int,int*);
void HttpSocket_connect(HttpSocket*,HttpSockaddr*,UINT,int*);
void HttpSocket_accept(HttpSocket*,HttpSocket*,int*);
void HttpSocket_nxClose(HttpSocket* o, int hardClose);
U32 HttpSocket_getId(HttpSocket*);
void HttpSocket_getPeerName(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*);
void HttpSocket_getSockName(HttpSocket*,HttpSockaddr*,U16*,BaBool,int*);
void HttpSocket_move(HttpSocket*,HttpSocket* newS);
void HttpSocket_send(HttpSocket*, struct ThreadMutex* m,
                     BaBool* isTerminated, const void*,int,int*);
void HttpSockaddr_inetAddr(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);

int HttpSocket_makeClientCon(
   HttpSocket* o,struct HttpSockaddr* addr,
   const char* interfaceName,U16 port, BaBool nonBlocking);

void HttpSockaddr_addr2String(
   HttpSockaddr* o, char* buf, int bufLen, int* status);

/***********************************************************************
 *  SoDispCon
 ***********************************************************************/

/*
   Object declared in SoDispCon and used by SoDisp.
   Not used by the ThreadX port.
*/
#define CONNECTION_DISPATCHER_OBJ

struct SoDispCon; /* forward decl */

#define SoDispCon_newConnectionIsReady(x)

#ifdef FEATURE_NX_IPV6
int SoDispCon_isIP6NetX(struct SoDispCon*);
#define SoDispCon_isIP6(o) SoDispCon_isIP6NetX(o)
#endif

/***********************************************************************
 *  SoDisp
 ***********************************************************************/


#define DISPATCHER_DATA \
  DoubleList soDispQueue;\
  TX_SEMAPHORE queueSem;\
  HttpSocket* curSock

struct AllocatorIntf; /* forward decl */
void SoDisp_nxInit(NX_IP** ip, NX_PACKET_POOL* nxPacketPool,
                   struct AllocatorIntf* nxTcpSockAlloc, ULONG nxWindowSize);
void SoDisp_setDNS(void* dns);


#ifdef __cplusplus
}
#endif 

#endif
