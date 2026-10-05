/*
 * COPYRIGHT: Real Time Logic LLC, 2026
 *
 * This software is copyrighted by and is the sole property of Real
 * Time Logic LLC. All rights, title, ownership, or other interests in
 * the software remain the property of Real Time Logic LLC. This
 * software may only be used in accordance with the terms and
 * conditions stipulated in the corresponding license agreement under
 * which the software has been supplied. Any unauthorized use,
 * duplication, transmission, distribution, or disclosure of this
 * software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of Real Time Logic LLC.
 * Real Time Logic LLC reserves the right to modify this software
 * without notice. https://realtimelogic.com
 */

#ifndef _BamDNS_h
#define _BamDNS_h

#include <SoDispCon.h>
#include <BaTimer.h>

/** Host-selected network snapshot. Zero-initialize before filling.
 * Addresses are network-order bytes; an all-zero address disables its family.
 * Each enabled family needs a nonzero interface index and prefix length
 * (1..32 or 1..128). Both families must refer to the same local link.
 * The host must supply assigned, usable addresses served by its HTTP listener.
 */
typedef struct
{
   U32 ifIndex4;
   U8 addr4[4];
   U8 prefix4;
#ifdef USE_IPV6
   U32 ifIndex6;
   U8 addr6[16];
   U8 prefix6;
#endif
} BamDNS_NetInfo;

/** Inputs are borrowed only until construction returns, then fully copied. */
typedef struct
{
   const char* name; /**< Required ASCII host label, 1..63 bytes, no .local. */
   const BamDNS_NetInfo* netInfo; /**< Optional fixed snapshot; NULL selects
                                    automatic discovery and five-second refresh. */
} BamDNS_Config;

struct BamDNS;

/* Private storage and socket-port contract. Applications must not use these
 * members. No native socket structures or Lua types occur in native storage.
 */
typedef struct
{
   HttpSockaddr addr;
   U16 port;
   BaBool multicast;
} BamDNS_Peer;

typedef struct
{
   SoDispCon con; /* First: receive callback casts back to this structure. */
   struct BamDNS* owner;
   void (*portData[2])(void); /* Opaque socket-port function pointers. */
   BamDNS_Peer tcPeer;
   U32 due, tcDue, lastSend;
   U8 pending, tcPending, retries;
   BaBool ipv6;
} BamDNS_Channel;

/** Minimal one-name responder. The caller holds the dispatcher mutex for
 * every API call. disp/timer must outlive this object and share their mutex.
 * No probing or conflict handling: the host must choose a unique name.
 */
typedef struct BamDNS
{
#ifdef __cplusplus
   BamDNS(SoDisp* disp, BaTimer* timer, const BamDNS_Config* cfg);
   ~BamDNS();
   int status() const;
#endif
   /* Private implementation storage, public only to permit caller allocation. */
   BamDNS_Channel channel[
#ifdef USE_IPV6
      2
#else
      1
#endif
   ];
   BamDNS_NetInfo net;
   BaTimer* timer;
   struct BamDNS_Timer* timerCtx;
   size_t timerKey;
   U32 announceDue, timerDue, refreshDue;
   int result;
   U8 name[71]; /* Length label + 63 bytes + length/local/root. */
   U8 nameLen, records, announcements;
   BaBool automatic;
#ifdef __cplusplus
private:
   BamDNS(const BamDNS&);
   BamDNS& operator=(const BamDNS&);
#endif
} BamDNS;

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize required caller-owned storage. Check BamDNS_status afterwards.
 * No return value, including on failure. Failed construction is safe to destroy.
 * Do not construct over a live object. No allocation of the object itself.
 * With cfg->netInfo == NULL, discovery releases/reacquires the dispatcher
 * mutex; keep caller-owned inputs/storage valid until construction returns.
 * Automatic mode checks the port's current LAN selection every five seconds,
 * rebuilds sockets/announces changed addresses, and retries after network loss.
 */
BA_API void BamDNS_constructor(BamDNS* o, SoDisp* disp, BaTimer* timer,
                               const BamDNS_Config* cfg);
/** Zero = ready. Otherwise E_INVALID_PARAM, E_INVALID_SOCKET_CON, E_BIND,
 * E_MALLOC, E_CANNOT_RESOLVE, E_SOCKET_READ_FAILED, E_SOCKET_WRITE_FAILED,
 * or E_SOCKET_CLOSED. Automatic responders can recover from network/socket
 * errors; an explicitly closed responder never restarts.
 * Readiness is local setup success, not a guarantee of name uniqueness.
 */
BA_API int BamDNS_status(const BamDNS* o);
/** Best-effort goodbye, cancel timer, detach sockets. Idempotent after init. */
BA_API void BamDNS_destructor(BamDNS* o);

/** Network discovery port hook; supplied for Windows/Linux/macOS.
 * Fill required caller-owned net with the first usable, active, multicast
 * LAN interface. IPv6 prefers a link-local address. No sockets are retained.
 * Call before taking the BAS mutex; enumeration may block. Returns zero,
 * E_CANNOT_RESOLVE (no interface), E_MALLOC, E_INVALID_SOCKET_CON (OS failure),
 * or E_INVALID_PARAM (NULL net). On failure a non-NULL net is cleared.
 * Each call fills one snapshot; automatic responders call it periodically.
 * No DNS/name probing is done. Fixed-snapshot-only ports must still supply
 * this symbol, but may return E_CANNOT_RESOLVE without enumerating.
 */
BA_API int BamDNS_getNetInfo(BamDNS_NetInfo* net);

#ifdef BAS_LOADED
struct lua_State;
/** Install ba.createmdns in the existing ba table; no values left on stack.
 * Pass NULL for current discovery at construction and periodic refresh.
 * A non-NULL netInfo selects fixed-snapshot mode and is borrowed until the Lua
 * VM closes (retained constructor closures can still use it). Close fixed-mode
 * responders before refreshing that snapshot under the dispatcher mutex.
 */
BA_API void BamDNS_luaopen(struct lua_State* L,
                          const BamDNS_NetInfo* netInfo);
#endif

/* Port implementers only; supplied BSD implementation is in
 * src/arch/bsdSocket/BamDNS-Sock.c. setup configures and binds an open socket;
 * returns 0, E_BIND, or -1. recv returns byte count,
 * 0 for a discarded packet, -2 for would-block, -1 for a local error.
 * send returns byte count, -2 for would-block, -1 for a local error.
 * All operations must be nonblocking. NULL send peer selects multicast.
 */
int BamDNS_socketSetup(BamDNS_Channel* c, const BamDNS_NetInfo* net);
int BamDNS_socketRecv(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                     void* data, int size, BamDNS_Peer* peer);
int BamDNS_socketSend(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                     const void* data, int size, const BamDNS_Peer* peer);

#ifdef __cplusplus
}
inline BamDNS::BamDNS(SoDisp* disp, BaTimer* timer, const BamDNS_Config* cfg)
{ BamDNS_constructor(this, disp, timer, cfg); }
inline BamDNS::~BamDNS() { BamDNS_destructor(this); }
inline int BamDNS::status() const { return BamDNS_status(this); }
#endif

#endif
