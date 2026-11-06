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

#ifndef BA_LIB
#define BA_LIB 1
#endif
#include <BamDNS.h>

#if BA_ENABLE_MDNS
#include <string.h>

#if !BA_ENABLE_UDP
#error BamDNS requires BA_ENABLE_UDP in the BAS socket port
#endif

#define MDNS_REFRESH_MS 5000

/* Timer-owned context survives close/GC while enumeration releases the mutex. */
typedef struct BamDNS_Timer {
   BamDNS* owner;
   BaBool running;
} BamDNS_Timer;

#define MDNS_A 1
#if BA_ENABLE_IPV6
#define MDNS_AAAA 2
#endif
#define MDNS_NSEC 4
#define MDNS_PACKET 1500
#define MDNS_CHANNELS (sizeof(o->channel)/sizeof(o->channel[0]))

static U16
mdnsU16(const U8* p)
{
   return (U16)(((U16)p[0] << 8) | p[1]);
}

static void
mdnsPut16(U8* p, unsigned v)
{
   p[0]=(U8)(v >> 8);
   p[1]=(U8)v;
}

/* Expand a name without recursion. Only backward compression pointers are
 * legal. Also cap traversal, expanded size, and every packet access.
 */
static int
mdnsName(const U8* p, int len, int* pos, U8* name)
{
   int at=*pos, end=0, used=0, steps=0;
   while(at < len && ++steps <= 128)
   {
      int n=p[at++];
      if((n & 0xc0) == 0xc0)
      {
         int to;
         if(at >= len)
            return -1;
         to=((n & 63) << 8) | p[at++];
         if(to < 12 || to >= at-2)
            return -1;
         if(!end) end=at;
         at=to;
      }
      else
      {
         if(n > 63 || at+n > len || used+n+1 > 255)
            return -1;
         if(name)
         {
            name[used]=(U8)n;
            memcpy(name+used+1, p+at, n);
         }
         used+=n+1;
         at+=n;
         if(!n)
         {
            *pos=end ? end : at;
            return used;
         }
      }
   }
   return -1;
}

static U8
mdnsLower(U8 c)
{
   return c >= 'A' && c <= 'Z' ? (U8)(c+'a'-'A') : c;
}

static BaBool
mdnsOwnName(const BamDNS* o, const U8* name, int len)
{
   int i;
   if(len != o->nameLen) return FALSE;
   for(i=0; i<len; ++i)
      if(mdnsLower(name[i]) != o->name[i]) return FALSE;
   return TRUE;
}

static void
mdnsBitmap(const BamDNS* o, U8* p)
{
   memset(p, 0, 8);
   p[1]=6;
   if(o->records & MDNS_A) p[2]=0x40;
#if BA_ENABLE_IPV6
   if(o->records & MDNS_AAAA) p[5]=8;
#endif
   p[7]=1; /* NSEC itself, type 47. */
}

static U8
mdnsQuestion(const BamDNS* o, U16 type)
{
   U8 bit=0;
   if(type == 255) return o->records;
   if(type == 1) bit=MDNS_A;
#if BA_ENABLE_IPV6
   if(type == 28) bit=MDNS_AAAA;
#endif
   return bit & o->records ? bit : MDNS_NSEC;
}

/* Validate every RR, including ignored sections, before acting on a packet.
 * Only exact copies of our records can suppress answers; never track conflicts.
 */
static int
mdnsRecords(const BamDNS* o, const U8* p, int len, int* pos,
            unsigned count, unsigned ttl, U8* known)
{
   U8 name[255];
   while(count--)
   {
      int n=mdnsName(p,len,pos,name), end, rlen;
      U16 type, cls;
      U32 life;
      U8 bit=0;
      if(n < 0 || *pos+10 > len) return -1;
      type=mdnsU16(p+*pos);
      cls=mdnsU16(p+*pos+2) & 0x7fff;
      life=((U32)mdnsU16(p+*pos+4) << 16) | mdnsU16(p+*pos+6);
      rlen=mdnsU16(p+*pos+8);
      *pos+=10;
      end=*pos+rlen;
      if(end > len || (type == 1 && rlen != 4)) return -1;
#if BA_ENABLE_IPV6
      if(type == 28 && rlen != 16) return -1;
#endif
      if(cls == 1 && life >= ttl && mdnsOwnName(o,name,n))
      {
         if(type == 1 && !memcmp(p+*pos,o->net.addr4,4)) bit=MDNS_A;
#if BA_ENABLE_IPV6
         if(type == 28 && !memcmp(p+*pos,o->net.addr6,16)) bit=MDNS_AAAA;
#endif
         if(type == 47)
         {
            U8 bitmap[8];
            int next=*pos;
            n=mdnsName(p,len,&next,name);
            mdnsBitmap(o,bitmap);
            if(n < 0 || next > end) return -1;
            if(mdnsOwnName(o,name,n) && next+8 == end &&
               !memcmp(p+next,bitmap,8)) bit=MDNS_NSEC;
         }
      }
      *known|=bit & (o->records | MDNS_NSEC);
      *pos=end;
   }
   return 0;
}

/* Append at most three fixed records. The caller reserves 160 bytes. */
static int
mdnsAnswers(const BamDNS* o, U8* p, int at, U8 mask, U16 ttl,
            BaBool legacy)
{
   int first=at, bit, count=0;
   for(bit=1; bit<=MDNS_NSEC; bit<<=1)
   {
      int rd;
      U16 type;
      if(!(mask & bit)) continue;
      if(!count)
      {
         memcpy(p+at,o->name,o->nameLen);
         at+=o->nameLen;
      }
      else
      {
         mdnsPut16(p+at,0xc000 | first);
         at+=2;
      }
      type=bit == MDNS_A ? 1 : 47;
      rd=bit == MDNS_A ? 4 : 10;
#if BA_ENABLE_IPV6
      if(bit == MDNS_AAAA) { type=28; rd=16; }
#endif
      mdnsPut16(p+at,type);
      mdnsPut16(p+at+2,legacy ? 1 : 0x8001);
      mdnsPut16(p+at+4,0);
      mdnsPut16(p+at+6,ttl);
      mdnsPut16(p+at+8,rd);
      at+=10;
      if(bit == MDNS_A) memcpy(p+at,o->net.addr4,4);
#if BA_ENABLE_IPV6
      else if(bit == MDNS_AAAA) memcpy(p+at,o->net.addr6,16);
#endif
      else
      {
         mdnsPut16(p+at,0xc000 | first);
         mdnsBitmap(o,p+at+2);
      }
      at+=rd;
      ++count;
   }
   mdnsPut16(p+6,count);
   return at;
}

static void mdnsSchedule(BamDNS* o);

static void
mdnsDisconnect(BamDNS* o, int status)
{
   unsigned i;
   o->result=status;
   o->announcements=0;
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      c->pending=c->tcPending=c->retries=0;
      if(SoDispCon_isValid(&c->con)) SoDispCon_closeCon(&c->con);
   }
}

static void
mdnsStop(BamDNS* o, int status)
{
   mdnsDisconnect(o,status);
   if(o->timerKey)
   {
      BaTimer_cancel(o->timer,o->timerKey);
      o->timerKey=0;
   }
   if(o->timerCtx)
   {
      o->timerCtx->owner=0;
      if(!o->timerCtx->running) baFree(o->timerCtx);
      o->timerCtx=0;
   }
}

static void
mdnsSuspend(BamDNS* o, int status)
{
   if(!o->automatic) mdnsStop(o,status);
   else
   {
      mdnsDisconnect(o,status);
      mdnsSchedule(o);
   }
}

static int
mdnsSend(BamDNS_Channel* c, U8 mask, U16 ttl, const BamDNS_Peer* peer,
         const U8* query, int qlen)
{
   U8 packet[MDNS_PACKET];
   int at=12, r;
   BaBool legacy=peer && peer->port != 5353;
   memset(packet,0,12);
   packet[2]=0x84;
   if(legacy)
   {
      U8 name[255];
      unsigned i, count=mdnsU16(query+4);
      int pos=12;
      memcpy(packet,query,2);
      mdnsPut16(packet+4,count);
      /* Re-encode questions: copying pointers could refer to omitted RRs. */
      for(i=0; i<count; ++i)
      {
         int n=mdnsName(query,qlen,&pos,name);
         if(n < 0 || pos+4 > qlen || at+n+4+160 > MDNS_PACKET) return 0;
         memcpy(packet+at,name,n);
         at+=n;
         memcpy(packet+at,query+pos,4);
         at+=4;
         pos+=4;
      }
      ttl=10;
   }
   at=mdnsAnswers(c->owner,packet,at,mask,ttl,legacy);
   r=BamDNS_socketSend(c,&c->owner->net,packet,at,peer);
   if(r >= 0 && r != at) return -1;
   return r;
}

static BaBool mdnsTimer(void* data);
static int mdnsSetNetwork(BamDNS* o, const BamDNS_NetInfo* net);

static void
mdnsSchedule(BamDNS* o)
{
   U32 now=baGetMsClock(), delay=0xffffffff;
   unsigned i;
   if(!o->timerCtx || o->timerCtx->running) return;
   if(o->automatic)
      delay=(S32)(o->refreshDue-now) > 0 ? o->refreshDue-now : 0;
   if(o->announcements)
   {
      U32 d=(S32)(o->announceDue-now) > 0 ? o->announceDue-now : 0;
      if(d < delay) delay=d;
   }
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      U32 d;
      if(c->pending)
      {
         d=(S32)(c->due-now) > 0 ? c->due-now : 0;
         if(d < delay) delay=d;
      }
      if(c->tcPending)
      {
         d=(S32)(c->tcDue-now) > 0 ? c->tcDue-now : 0;
         if(d < delay) delay=d;
      }
   }
   if(o->timerKey)
   {
      /* An earlier existing wakeup can also service later queued work. */
      if(delay != 0xffffffff && (S32)(o->timerDue-(now+delay)) <= 0) return;
      BaTimer_cancel(o->timer,o->timerKey);
      o->timerKey=0;
   }
   if(delay != 0xffffffff)
   {
      o->timerDue=now+delay;
      o->timerKey=BaTimer_set(o->timer,mdnsTimer,o->timerCtx,delay);
      if(!o->timerKey) mdnsStop(o,E_MALLOC);
   }
}

static void
mdnsQueue(BamDNS_Channel* c, U8 mask, U32 delay)
{
   U32 now=baGetMsClock(), due=now+delay;
   /* Conservatively rate-limit the whole RR set to once per second. */
   if((U32)(now-c->lastSend) < 1000 &&
      (S32)(c->lastSend+1000-due) > 0) due=c->lastSend+1000;
   if(!c->pending || (S32)(due-c->due) < 0) c->due=due;
   c->pending|=mask;
}

static BaBool
mdnsTimer(void* data)
{
   BamDNS_Timer* ctx=(BamDNS_Timer*)data;
   BamDNS* o=ctx->owner;
   U32 now=baGetMsClock();
   unsigned i;
   ctx->running=TRUE;
   o->timerKey=0; /* This one-shot node is retired by BaTimer on return. */
   if(o->automatic && (S32)(now-o->refreshDue) >= 0)
   {
      BamDNS_NetInfo net;
      ThreadMutex* mutex=o->timer->mutex;
      int status;
      ThreadMutex_release(mutex);
      status=BamDNS_getNetInfo(&net);
      ThreadMutex_set(mutex);
      /* close/GC can free the responder, but not this running context. */
      o=ctx->owner;
      if(!o) { baFree(ctx); return FALSE; }
      now=baGetMsClock();
      o->refreshDue=now+MDNS_REFRESH_MS;
      if(status) mdnsSuspend(o,status);
      else if(o->result || memcmp(&o->net,&net,sizeof(net)))
         mdnsSetNetwork(o,&net);
   }
   if(o->announcements && (S32)(now-o->announceDue) >= 0)
   {
      for(i=0; i<MDNS_CHANNELS; ++i)
         if(SoDispCon_isValid(&o->channel[i].con))
            mdnsQueue(o->channel+i,o->records,0);
      --o->announcements;
      o->announceDue=now+1000;
   }
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      if(c->tcPending && (S32)(now-c->tcDue) >= 0)
      {
         mdnsQueue(c,c->tcPending,0);
         c->tcPending=0;
      }
      if(c->pending && (S32)(now-c->due) >= 0)
      {
         int r=mdnsSend(c,c->pending,120,0,0,0);
         if(r == -1)
         {
            mdnsSuspend(o,E_SOCKET_WRITE_FAILED);
            break;
         }
         if(r == -2 && ++c->retries < 3) c->due=now+20;
         else
         {
            if(r > 0) c->lastSend=now;
            c->pending=c->retries=0;
         }
      }
   }
   ctx->running=FALSE;
   if(ctx->owner) mdnsSchedule(o);
   else baFree(ctx);
   return FALSE;
}

static BaBool
mdnsSamePeer(const BamDNS_Peer* a, const BamDNS_Peer* b)
{
   return a->port == b->port && a->addr.isIp6 == b->addr.isIp6 &&
      !memcmp(a->addr.addr,b->addr.addr,a->addr.isIp6 ? 16 : 4);
}

static void
mdnsReceive(SoDispCon* con)
{
   BamDNS_Channel* c=(BamDNS_Channel*)con;
   BamDNS* o=c->owner;
   BamDNS_Peer peer;
   U8 packet[MDNS_PACKET+1], name[255], qm=0, qu=0, known=0, ignored=0;
   U16 flags;
   int pos=12, len;
   unsigned i, count;
   U32 now=baGetMsClock();
   /* One datagram per event keeps dispatcher work bounded. */
   len=BamDNS_socketRecv(c,&o->net,packet,sizeof(packet),&peer);
   if(len == -1)
   {
      mdnsSuspend(o,E_SOCKET_READ_FAILED);
      return;
   }
   if(len < 12 || len > MDNS_PACKET) return;
   flags=mdnsU16(packet+2);
   if(flags & 0x780f) return; /* Only standard queries/responses, no errors. */
   if((flags & 0x8000) && peer.port != 5353) return;
   count=mdnsU16(packet+4);
   for(i=0; i<count; ++i)
   {
      int n=mdnsName(packet,len,&pos,name);
      U16 type, cls;
      if(n < 0 || pos+4 > len) return;
      type=mdnsU16(packet+pos);
      cls=mdnsU16(packet+pos+2);
      pos+=4;
      if(mdnsOwnName(o,name,n) && ((cls & 0x7fff) == 1 ||
                                   (cls & 0x7fff) == 255))
      {
         if(cls & 0x8000) qu|=mdnsQuestion(o,type);
         else qm|=mdnsQuestion(o,type);
      }
   }
   if(mdnsRecords(o,packet,len,&pos,mdnsU16(packet+6),
                  flags & 0x8000 ? 120 : 60,&known) ||
      mdnsRecords(o,packet,len,&pos,mdnsU16(packet+8),120,&ignored) ||
      mdnsRecords(o,packet,len,&pos,mdnsU16(packet+10),120,&ignored) ||
      pos != len) return;
   if(flags & 0x8000)
   {
      if(peer.multicast)
      {
         c->pending&=(U8)~known;
         c->tcPending&=(U8)~known;
      }
   }
   else if(peer.port != 5353 || !peer.multicast)
   {
      if(peer.port == 5353)
      {
         qm&=(U8)~known;
         qu&=(U8)~known;
      }
      /* Direct/legacy answers have no persistent request buffer. UDP send
       * backpressure drops this reply; the querier can retransmit its query.
       */
      if(qm | qu)
         if(mdnsSend(c,qm|qu,120,&peer,packet,len) == -1)
         {
            mdnsSuspend(o,E_SOCKET_WRITE_FAILED);
            return;
         }
   }
   else
   {
      qm&=(U8)~known;
      qu&=(U8)~known;
      if(c->tcPending && mdnsSamePeer(&peer,&c->tcPeer))
      {
         c->tcPending&=(U8)~known;
         if(flags & 0x200) c->tcDue=now+450+(now%51);
      }
      if(flags & 0x200)
      {
         if(c->tcPending && !mdnsSamePeer(&peer,&c->tcPeer))
         {
            /* One continuation slot; preserve the previous query's answers
             * conservatively when a second querier needs the slot. */
            mdnsQueue(c,c->tcPending,
                      (S32)(c->tcDue-now) > 0 ? c->tcDue-now : 0);
            c->tcPending=0;
         }
         c->tcPeer=peer;
         c->tcPending|=qm|qu;
         c->tcDue=now+450+(now%51);
      }
      else
      {
         qu&=(U8)~qm;
         if(qu && (U32)(now-c->lastSend) < 30000)
         {
            if(mdnsSend(c,qu,120,&peer,0,0) == -1)
            {
               mdnsSuspend(o,E_SOCKET_WRITE_FAILED);
               return;
            }
         }
         else qm|=qu;
         if(qm) mdnsQueue(c,qm,count > 1 ? 20+(now%101) : 0);
      }
   }
   mdnsSchedule(o);
}

static BaBool
mdnsNonzero(const U8* p, unsigned len)
{
   while(len--) if(*p++) return TRUE;
   return FALSE;
}

BA_API void
BamDNS_constructor(BamDNS* o, SoDisp* disp, BaTimer* timer,
                   const BamDNS_Config* cfg)
{
   unsigned i, n;
   U32 now=baGetMsClock();
   memset(o,0,sizeof(*o));
   o->result=E_INVALID_PARAM;
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      SoDispCon_constructor(&c->con,disp,mdnsReceive);
      c->owner=o;
      c->ipv6=(BaBool)i;
      c->lastSend=now-30000;
   }
   if(!disp || !timer || !SoDisp_getMutex(disp) ||
      timer->mutex != SoDisp_getMutex(disp) ||
      !cfg || !cfg->name) return;
   for(n=0; cfg->name[n] && n<64; ++n)
   {
      U8 ch=mdnsLower((U8)cfg->name[n]);
      if(!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') ||
           (ch == '-' && n))) return;
      if(n<63) o->name[n+1]=ch;
   }
   if(!n || n>63 || o->name[n] == '-') return;
   o->name[0]=(U8)n;
   memcpy(o->name+n+1,"\5local",7);
   o->nameLen=(U8)(n+8);
   o->timer=timer;
   o->automatic=cfg->netInfo == 0;
   if(o->automatic)
   {
      BamDNS_NetInfo net;
      ThreadMutex_release(timer->mutex);
      o->result=BamDNS_getNetInfo(&net);
      ThreadMutex_set(timer->mutex);
      if(o->result) return;
      o->net=net;
   }
   else o->net=*cfg->netInfo;
   o->timerCtx=(BamDNS_Timer*)baMalloc(sizeof(BamDNS_Timer));
   if(!o->timerCtx) { o->result=E_MALLOC; return; }
   o->timerCtx->owner=o;
   o->timerCtx->running=FALSE;
   o->refreshDue=now+MDNS_REFRESH_MS;
   if(mdnsSetNetwork(o,&o->net)) mdnsStop(o,o->result);
   else mdnsSchedule(o);
}

/* Replace memberships as well as address records. Old queued replies are
   discarded and the new records get the normal two startup announcements. */
static int
mdnsSetNetwork(BamDNS* o, const BamDNS_NetInfo* net)
{
   unsigned i;
   U32 now=baGetMsClock();
   SoDisp* disp=SoDispCon_getDispatcher(&o->channel[0].con);
   if(!o->result)
      for(i=0; i<MDNS_CHANNELS; ++i)
         if(SoDispCon_isValid(&o->channel[i].con))
            mdnsSend(o->channel+i,o->records,0,0,0,0);
   mdnsDisconnect(o,E_INVALID_PARAM);
   o->net=*net;
   o->records=0;
   if(mdnsNonzero(o->net.addr4,4))
   {
      if(!o->net.ifIndex4 || !o->net.prefix4 || o->net.prefix4>32 ||
         !o->net.addr4[0] || o->net.addr4[0]>=224 ||
         o->net.addr4[0]==127) return E_INVALID_PARAM;
      o->records|=MDNS_A;
   }
#if BA_ENABLE_IPV6
   if(mdnsNonzero(o->net.addr6,16))
   {
      if(!o->net.ifIndex6 || !o->net.prefix6 || o->net.prefix6>128 ||
         o->net.addr6[0]==255 || !o->net.addr6[0]) return E_INVALID_PARAM;
      o->records|=MDNS_AAAA;
   }
#endif
   if(!o->records) return E_INVALID_PARAM;
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      int status;
      if(!(o->records & (1 << i))) continue;
      HttpSocket_sockUdp(&c->con.httpSocket,0,c->ipv6,&status);
      if(status || SoDispCon_setNonblocking(&c->con))
      {
         mdnsDisconnect(o,E_INVALID_SOCKET_CON);
         return o->result;
      }
      status=BamDNS_socketSetup(c,&o->net);
      if(status)
      {
         mdnsDisconnect(o,status == E_BIND ? E_BIND : E_INVALID_SOCKET_CON);
         return o->result;
      }
      c->con.dataBits|=SoDispCon_DGramBitMask;
      if(c->ipv6) SoDispCon_setIP6(&c->con);
      SoDisp_addConnection(disp,&c->con);
      SoDisp_activateRec(disp,&c->con);
   }
   o->result=0;
   o->announcements=2;
   o->announceDue=now;
   return 0;
}

BA_API int
BamDNS_status(const BamDNS* o)
{
   return o->result;
}

BA_API void
BamDNS_destructor(BamDNS* o)
{
   unsigned i;
   if(!o->result)
      for(i=0; i<MDNS_CHANNELS; ++i)
         if(SoDispCon_isValid(&o->channel[i].con))
            mdnsSend(o->channel+i,o->records,0,0,0,0);
   mdnsStop(o,E_SOCKET_CLOSED);
}

#if BA_ENABLE_MDNS_LUA
#include <balua.h>

#define MDNS_META "BamDNS"

static const char*
mdnsError(int status)
{
   switch(status)
   {
      case E_INVALID_PARAM: return "invalid name or network configuration";
      case E_INVALID_SOCKET_CON: return "socket or multicast setup failed";
      case E_BIND: return "cannot bind mDNS port";
      case E_MALLOC: return "allocation failed";
      case E_CANNOT_RESOLVE: return "no usable network interface";
      case E_SOCKET_READ_FAILED: return "socket read failed";
      case E_SOCKET_WRITE_FAILED: return "socket write failed";
      default: return "closed";
   }
}

static int
mdnsLuaClose(lua_State* L)
{
   BamDNS_destructor((BamDNS*)luaL_checkudata(L,1,MDNS_META));
   return 0;
}

static int
mdnsLuaStatus(lua_State* L)
{
   int status=BamDNS_status((BamDNS*)luaL_checkudata(L,1,MDNS_META));
   if(!status)
   {
      lua_pushboolean(L,TRUE);
      return 1;
   }
   lua_pushnil(L);
   lua_pushstring(L,mdnsError(status));
   return 2;
}

static int
mdnsLuaCreate(lua_State* L)
{
   BamDNS* o;
   BamDNS_Config cfg;
   BaLua_param* p=balua_getparam(L);
   size_t len;
   luaL_checktype(L,1,LUA_TSTRING);
   cfg.name=lua_tolstring(L,1,&len);
   luaL_argcheck(L,len == strlen(cfg.name),1,"name contains NUL");
   cfg.netInfo=(const BamDNS_NetInfo*)lua_touserdata(L,lua_upvalueindex(1));
   o=(BamDNS*)lua_newuserdatauv(L,sizeof(BamDNS),0);

   if(luaL_newmetatable(L, MDNS_META))
   {
      static const luaL_Reg lib[] = {
         {"status",mdnsLuaStatus},
         {"close", mdnsLuaClose},
         {"__close", mdnsLuaClose},
         {"__gc", mdnsLuaClose},
         {NULL, NULL}
      };
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");
      luaL_setfuncs(L,lib,0);
   }
   lua_setmetatable(L, -2); /* Set meta for userdata */
   BamDNS_constructor(o,p->server ? HttpServer_getDispatcher(p->server) : 0,
                      p->timer,&cfg);
   if(!o->result) return 1;
   lua_pushnil(L);
   lua_pushstring(L,mdnsError(o->result));
   return 2;
}

BA_API void
BamDNS_luaopen(lua_State* L, const BamDNS_NetInfo* netInfo)
{
   lua_getglobal(L,"ba");
   lua_pushlightuserdata(L,(void*)netInfo);
   lua_pushcclosure(L,mdnsLuaCreate,1);
   lua_setfield(L,-2,"createmdns");
   lua_pop(L,1); /* "ba" */
}


#endif

#endif /* BA_ENABLE_MDNS */
