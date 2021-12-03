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
 *   $Id: HttpCfg.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2008
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
 *  Posix -> epoll implementation
 */
#ifndef _HttpConfig_h
#include "../Posix/HttpCfg.h"
#include <sys/epoll.h>


#undef DISPATCHER_DATA
#undef CONNECTION_DISPATCHER_OBJ

struct SoDispCon;

typedef struct
{
   struct SoDispCon* con;
   U32 mask;
} SoDispConBucket;

typedef SoDispConBucket SoDispConBucketTab[256];

/* Max concurrent connections: 1024*256 */
#define BA_MAX_BUCKETS 1024

#define DISPATCHER_DATA \
  struct epoll_event * events;\
  DoubleList termList;\
  SoDispConBucketTab* bucket[BA_MAX_BUCKETS];\
  SoDispConBucket* freeList;\
  SoDispConBucket* freeListTail;\
  U16 bucketIx;\
  U16 nextId;\
  int maxevents;\
  int epfd;\
  int defaultPollDelay; \
  int pollDelay


#define CONNECTION_DISPATCHER_OBJ DoubleLink dispatcherLink;U32 mask;

#define SoDisp_destructor _SoDisp_destructor
struct SoDisp;
void _SoDisp_destructor(struct SoDisp* o);
#define BaAddrinfo_connect BaAddrinfo_platConnect
int BaAddrinfo_platConnect(BaAddrinfo* addr, HttpSocket* s, U32 timeout);

#endif
