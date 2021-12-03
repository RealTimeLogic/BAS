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
 *   $Id: HttpCfg.h 4915 2021-12-01 18:26:55Z wini $
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
 *  VxWorks
 */
#ifndef _HttpConfig_h
#define _HttpConfig_h

#if _WRS_VXWORKS_MAJOR == 5
#include <net/inet.h>
#endif

#include <vxWorks.h> 
#include <sockLib.h>
#include <selectLib.h>
#include <hostLib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <inetLib.h>


/* Include the line below if you get compile errors */
/* #include <net/inet.h> */



#include <TargConfig.h>
#include <gBsdDspO.h>
#include <NetConv.h>


/***********************************************************************
 *  The HttpSocket API
 ***********************************************************************/

#define socketConnect _socketConnect
int _socketConnect(int s,  struct sockaddr* name,  int namelen);
#ifdef sodisp_c
int _socketConnect(int s,  struct sockaddr* name,  int namelen)
{
   struct timeval timeout;
   timeout.tv_sec  = 1;
   timeout.tv_usec = 0;
   return connectWithTimeout(s,name,namelen,&timeout);
}
#endif


/* Overload the following default functions */

#if _WRS_VXWORKS_MAJOR == 5
#define basocklen_t int

#define HttpSocket_shutdown(o) do {\
   int status;\
   HttpSocket_setBlocking(o,&status);\
   (void)status; \
   socketClose((o)->hndl);\
   HttpSocket_invalidate(o);\
 } while(0)

#endif

#define socketIoctl(a,b,c) ioctl(a,b,c)


#ifdef USE_IPV6
#define UseVxWorksGethostbyname
#define HttpSockaddr_gethostbyname HttpSockaddr_gethostbynameF
#else
#define HttpSockaddr_gethostbyname(o, host, useIp6, status)  do { \
   int ipAddr; \
   (o)->isIp6=FALSE; \
   *(status)=0; \
   if(host) \
   { \
      ipAddr = socketInetAddr((char*)host); /* is "host" an IP address ? */ \
      if((unsigned)ipAddr == INADDR_NONE) /* No, not an IP address. */ \
      { /* Is "host" a hostname ? */\
         ipAddr = hostGetByName((char*)host); \
         if(ipAddr == ERROR) \
            *(status)=-1; \
      } \
   } \
   else \
      ipAddr = baHtonl(INADDR_ANY); \
   memcpy((o)->addr,&ipAddr, 4); \
}while(0)
#endif



/* Include the default HttpSocket functions */
#include <gBsdSock.h>

#ifdef UseVxWorksGethostbyname
void HttpSockaddr_gethostbynameF(
   HttpSockaddr* o, const char* host, BaBool useIp6, int* status);
#endif

#endif
