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
 *                              HEADER
 *
 *   $Id: HttpCfg.h 6731 2026-11-03 23:51:39Z wini $
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
 *  Posix -> poll implementation
 */
#ifndef _HttpConfig_h
#include "../Posix/HttpCfg.h"
#include <poll.h>

#undef DISPATCHER_DATA
#undef CONNECTION_DISPATCHER_OBJ

struct SoDispCon;

/* Separate from pollfd: only the dispatcher thread touches the poll array
   during a wait. Connection removal can invalidate this table under the
   dispatcher mutex without changing memory being used by poll(). */
typedef struct
{
   struct SoDispCon* con;
   short events;
} SoDispPollCon;

#define DISPATCHER_DATA \
  DoubleList conList; \
  struct pollfd* pollSet; \
  SoDispPollCon* pollCon; \
  int pollCapacity; \
  int pollCount; \
  int defaultPollDelay; \
  int pollDelay

/* Index in the current wait snapshot, or -1 when not in that snapshot. */
#define CONNECTION_DISPATCHER_OBJ \
  DoubleLink dispatcherLink; \
  int pollIndex;

#ifdef __cplusplus
extern "C" {
#endif
#define SoDisp_destructor SoDisp_pollDestructor
struct SoDisp;
BA_API void SoDisp_pollDestructor(struct SoDisp* o);

/* Override the generic select-based connect wait, including high fds. */
#define BaAddrinfo_connect BaAddrinfo_platConnect
BA_API int BaAddrinfo_platConnect(BaAddrinfo* addr, HttpSocket* s, U32 timeout);
#ifdef __cplusplus
}
#endif

#endif
