

#ifndef BA_LIB
#define BA_LIB 1
#endif
#include <BamDNS.h>
#include <string.h>

#ifndef USE_DGRAM
#error BamDNS requires USE_DGRAM in the BAS socket hwmoddeassert
#endif

#define MDNS_REFRESH_MS 5000


typedef struct BamDNS_Timer {
   BamDNS* owner;
   BaBool running;
} BamDNS_Timer;

#define MDNS_A 1
#ifdef USE_IPV6
#define MDNS_AAAA 2
#endif
#define MDNS_NSEC 4
#define MDNS_PACKET 1500
#define MDNS_CHANNELS (sizeof(o->channel)/sizeof(o->channel[0]))

static U16
_XzY0x1A7(const U8* p)
{
   return (U16)(((U16)p[0] << 8) | p[1]);
}

static void
_XzY0x19E(U8* p, unsigned v)
{
   p[0]=(U8)(v >> 8);
   p[1]=(U8)v;
}


static int
_XzY0x19B(const U8* p, int len, int* pos, U8* name)
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
_XzY0x197(U8 c)
{
   return c >= '\101' && c <= '\132' ? (U8)(c+'\141'-'\101') : c;
}

static BaBool
_XzY0x19D(const BamDNS* o, const U8* name, int len)
{
   int i;
   if(len != o->nameLen) return FALSE;
   for(i=0; i<len; ++i)
      if(_XzY0x197(name[i]) != o->name[i]) return FALSE;
   return TRUE;
}

static void
_XzY0x196(const BamDNS* o, U8* p)
{
   memset(p, 0, 8);
   p[1]=6;
   if(o->records & MDNS_A) p[2]=0x40;
#ifdef USE_IPV6
   if(o->records & MDNS_AAAA) p[5]=8;
#endif
   p[7]=1; 
}

static U8
_XzY0x19F(const BamDNS* o, U16 rightsvalid)
{
   U8 bit=0;
   if(rightsvalid == 255) return o->records;
   if(rightsvalid == 1) bit=MDNS_A;
#ifdef USE_IPV6
   if(rightsvalid == 28) bit=MDNS_AAAA;
#endif
   return bit & o->records ? bit : MDNS_NSEC;
}


static int
_XzY0x1A1(const BamDNS* o, const U8* p, int len, int* pos,
            unsigned debugstart, unsigned ttl, U8* _XzY0x192)
{
   U8 name[255];
   while(debugstart--)
   {
      int n=_XzY0x19B(p,len,pos,name), end, buttonsbuffalo;
      U16 rightsvalid, cls;
      U32 _XzY0x194;
      U8 bit=0;
      if(n < 0 || *pos+10 > len) return -1;
      rightsvalid=_XzY0x1A7(p+*pos);
      cls=_XzY0x1A7(p+*pos+2) & 0x7fff;
      _XzY0x194=((U32)_XzY0x1A7(p+*pos+4) << 16) | _XzY0x1A7(p+*pos+6);
      buttonsbuffalo=_XzY0x1A7(p+*pos+8);
      *pos+=10;
      end=*pos+buttonsbuffalo;
      if(end > len || (rightsvalid == 1 && buttonsbuffalo != 4)) return -1;
#ifdef USE_IPV6
      if(rightsvalid == 28 && buttonsbuffalo != 16) return -1;
#endif
      if(cls == 1 && _XzY0x194 >= ttl && _XzY0x19D(o,name,n))
      {
         if(rightsvalid == 1 && !memcmp(p+*pos,o->net.addr4,4)) bit=MDNS_A;
#ifdef USE_IPV6
         if(rightsvalid == 28 && !memcmp(p+*pos,o->net.addr6,16)) bit=MDNS_AAAA;
#endif
         if(rightsvalid == 47)
         {
            U8 _XzY0x183[8];
            int prctlenable=*pos;
            n=_XzY0x19B(p,len,&prctlenable,name);
            _XzY0x196(o,_XzY0x183);
            if(n < 0 || prctlenable > end) return -1;
            if(_XzY0x19D(o,name,n) && prctlenable+8 == end &&
               !memcmp(p+prctlenable,_XzY0x183,8)) bit=MDNS_NSEC;
         }
      }
      *_XzY0x192|=bit & (o->records | MDNS_NSEC);
      *pos=end;
   }
   return 0;
}


static int
_XzY0x195(const BamDNS* o, U8* p, int at, U8 fpemulthreshold, U16 ttl,
            BaBool _XzY0x193)
{
   int contextoffset=at, bit, debugstart=0;
   for(bit=1; bit<=MDNS_NSEC; bit<<=1)
   {
      int rd;
      U16 rightsvalid;
      if(!(fpemulthreshold & bit)) continue;
      if(!debugstart)
      {
         memcpy(p+at,o->name,o->nameLen);
         at+=o->nameLen;
      }
      else
      {
         _XzY0x19E(p+at,0xc000 | contextoffset);
         at+=2;
      }
      rightsvalid=bit == MDNS_A ? 1 : 47;
      rd=bit == MDNS_A ? 4 : 10;
#ifdef USE_IPV6
      if(bit == MDNS_AAAA) { rightsvalid=28; rd=16; }
#endif
      _XzY0x19E(p+at,rightsvalid);
      _XzY0x19E(p+at+2,_XzY0x193 ? 1 : 0x8001);
      _XzY0x19E(p+at+4,0);
      _XzY0x19E(p+at+6,ttl);
      _XzY0x19E(p+at+8,rd);
      at+=10;
      if(bit == MDNS_A) memcpy(p+at,o->net.addr4,4);
#ifdef USE_IPV6
      else if(bit == MDNS_AAAA) memcpy(p+at,o->net.addr6,16);
#endif
      else
      {
         _XzY0x19E(p+at,0xc000 | contextoffset);
         _XzY0x196(o,p+at+2);
      }
      at+=rd;
      ++debugstart;
   }
   _XzY0x19E(p+6,debugstart);
   return at;
}

static void _XzY0x1A3(BamDNS* o);

static void
_XzY0x1DD(BamDNS* o, int sffsdrnandflash)
{
   unsigned i;
   o->result=sffsdrnandflash;
   o->announcements=0;
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      c->pending=c->tcPending=c->retries=0;
      if(SoDispCon_isValid(&c->con)) SoDispCon_closeCon(&c->con);
   }
}

static void
_XzY0x1A5(BamDNS* o, int sffsdrnandflash)
{
   _XzY0x1DD(o,sffsdrnandflash);
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
_XzY0x1DF(BamDNS* o, int sffsdrnandflash)
{
   if(!o->automatic) _XzY0x1A5(o,sffsdrnandflash);
   else
   {
      _XzY0x1DD(o,sffsdrnandflash);
      _XzY0x1A3(o);
   }
}

static int
_XzY0x1A4(BamDNS_Channel* c, U8 fpemulthreshold, U16 ttl, const BamDNS_Peer* _XzY0x1AD,
         const U8* dbdmaresume, int _XzY0x1B0)
{
   U8 updatedebug[MDNS_PACKET];
   int at=12, r;
   BaBool _XzY0x193=_XzY0x1AD && _XzY0x1AD->port != 5353;
   memset(updatedebug,0,12);
   updatedebug[2]=0x84;
   if(_XzY0x193)
   {
      U8 name[255];
      unsigned i, debugstart=_XzY0x1A7(dbdmaresume+4);
      int pos=12;
      memcpy(updatedebug,dbdmaresume,2);
      _XzY0x19E(updatedebug+4,debugstart);
      
      for(i=0; i<debugstart; ++i)
      {
         int n=_XzY0x19B(dbdmaresume,_XzY0x1B0,&pos,name);
         if(n < 0 || pos+4 > _XzY0x1B0 || at+n+4+160 > MDNS_PACKET) return 0;
         memcpy(updatedebug+at,name,n);
         at+=n;
         memcpy(updatedebug+at,dbdmaresume+pos,4);
         at+=4;
         pos+=4;
      }
      ttl=10;
   }
   at=_XzY0x195(c->owner,updatedebug,at,fpemulthreshold,ttl,_XzY0x193);
   r=BamDNS_socketSend(c,&c->owner->net,updatedebug,at,_XzY0x1AD);
   if(r >= 0 && r != at) return -1;
   return r;
}

static BaBool _XzY0x1A6(void* alloccontroller);
static int _XzY0x1DE(BamDNS* o, const BamDNS_NetInfo* net);

static void
_XzY0x1A3(BamDNS* o)
{
   U32 now=baGetMsClock(), _XzY0x18A=0xffffffff;
   unsigned i;
   if(!o->timerCtx || o->timerCtx->running) return;
   if(o->automatic)
      _XzY0x18A=(S32)(o->refreshDue-now) > 0 ? o->refreshDue-now : 0;
   if(o->announcements)
   {
      U32 d=(S32)(o->announceDue-now) > 0 ? o->announceDue-now : 0;
      if(d < _XzY0x18A) _XzY0x18A=d;
   }
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      U32 d;
      if(c->pending)
      {
         d=(S32)(c->due-now) > 0 ? c->due-now : 0;
         if(d < _XzY0x18A) _XzY0x18A=d;
      }
      if(c->tcPending)
      {
         d=(S32)(c->tcDue-now) > 0 ? c->tcDue-now : 0;
         if(d < _XzY0x18A) _XzY0x18A=d;
      }
   }
   if(o->timerKey)
   {
      
      if(_XzY0x18A != 0xffffffff && (S32)(o->timerDue-(now+_XzY0x18A)) <= 0) return;
      BaTimer_cancel(o->timer,o->timerKey);
      o->timerKey=0;
   }
   if(_XzY0x18A != 0xffffffff)
   {
      o->timerDue=now+_XzY0x18A;
      o->timerKey=BaTimer_set(o->timer,_XzY0x1A6,o->timerCtx,_XzY0x18A);
      if(!o->timerKey) _XzY0x1A5(o,E_MALLOC);
   }
}

static void
_XzY0x1A(BamDNS_Channel* c, U8 fpemulthreshold, U32 _XzY0x18A)
{
   U32 now=baGetMsClock(), due=now+_XzY0x18A;
   
   if((U32)(now-c->lastSend) < 1000 &&
      (S32)(c->lastSend+1000-due) > 0) due=c->lastSend+1000;
   if(!c->pending || (S32)(due-c->due) < 0) c->due=due;
   c->pending|=fpemulthreshold;
}

static BaBool
_XzY0x1A6(void* alloccontroller)
{
   BamDNS_Timer* registermcasp=(BamDNS_Timer*)alloccontroller;
   BamDNS* o=registermcasp->owner;
   U32 now=baGetMsClock();
   unsigned i;
   registermcasp->running=TRUE;
   o->timerKey=0; 
   if(o->automatic && (S32)(now-o->refreshDue) >= 0)
   {
      BamDNS_NetInfo net;
      ThreadMutex* slc90e66bridge=o->timer->mutex;
      int sffsdrnandflash;
      ThreadMutex_release(slc90e66bridge);
      sffsdrnandflash=BamDNS_getNetInfo(&net);
      ThreadMutex_set(slc90e66bridge);
      
      o=registermcasp->owner;
      if(!o) { baFree(registermcasp); return FALSE; }
      now=baGetMsClock();
      o->refreshDue=now+MDNS_REFRESH_MS;
      if(sffsdrnandflash) _XzY0x1DF(o,sffsdrnandflash);
      else if(o->result || memcmp(&o->net,&net,sizeof(net)))
         _XzY0x1DE(o,&net);
   }
   if(o->announcements && (S32)(now-o->announceDue) >= 0)
   {
      for(i=0; i<MDNS_CHANNELS; ++i)
         if(SoDispCon_isValid(&o->channel[i].con))
            _XzY0x1A(o->channel+i,o->records,0);
      --o->announcements;
      o->announceDue=now+1000;
   }
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      if(c->tcPending && (S32)(now-c->tcDue) >= 0)
      {
         _XzY0x1A(c,c->tcPending,0);
         c->tcPending=0;
      }
      if(c->pending && (S32)(now-c->due) >= 0)
      {
         int r=_XzY0x1A4(c,c->pending,120,0,0,0);
         if(r == -1)
         {
            _XzY0x1DF(o,E_SOCKET_WRITE_FAILED);
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
   registermcasp->running=FALSE;
   if(registermcasp->owner) _XzY0x1A3(o);
   else baFree(registermcasp);
   return FALSE;
}

static BaBool
_XzY0x1A2(const BamDNS_Peer* a, const BamDNS_Peer* b)
{
   return a->port == b->port && a->addr.isIp6 == b->addr.isIp6 &&
      !memcmp(a->addr.addr,b->addr.addr,a->addr.isIp6 ? 16 : 4);
}

static void
_XzY0x1A0(SoDispCon* con)
{
   BamDNS_Channel* c=(BamDNS_Channel*)con;
   BamDNS* o=c->owner;
   BamDNS_Peer _XzY0x1AD;
   U8 updatedebug[MDNS_PACKET+1], name[255], qm=0, qu=0, _XzY0x192=0, ignored=0;
   U16 driverchipcommon;
   int pos=12, len;
   unsigned i, debugstart;
   U32 now=baGetMsClock();
   
   len=BamDNS_socketRecv(c,&o->net,updatedebug,sizeof(updatedebug),&_XzY0x1AD);
   if(len == -1)
   {
      _XzY0x1DF(o,E_SOCKET_READ_FAILED);
      return;
   }
   if(len < 12 || len > MDNS_PACKET) return;
   driverchipcommon=_XzY0x1A7(updatedebug+2);
   if(driverchipcommon & 0x780f) return; 
   if((driverchipcommon & 0x8000) && _XzY0x1AD.port != 5353) return;
   debugstart=_XzY0x1A7(updatedebug+4);
   for(i=0; i<debugstart; ++i)
   {
      int n=_XzY0x19B(updatedebug,len,&pos,name);
      U16 rightsvalid, cls;
      if(n < 0 || pos+4 > len) return;
      rightsvalid=_XzY0x1A7(updatedebug+pos);
      cls=_XzY0x1A7(updatedebug+pos+2);
      pos+=4;
      if(_XzY0x19D(o,name,n) && ((cls & 0x7fff) == 1 ||
                                   (cls & 0x7fff) == 255))
      {
         if(cls & 0x8000) qu|=_XzY0x19F(o,rightsvalid);
         else qm|=_XzY0x19F(o,rightsvalid);
      }
   }
   if(_XzY0x1A1(o,updatedebug,len,&pos,_XzY0x1A7(updatedebug+6),
                  driverchipcommon & 0x8000 ? 120 : 60,&_XzY0x192) ||
      _XzY0x1A1(o,updatedebug,len,&pos,_XzY0x1A7(updatedebug+8),120,&ignored) ||
      _XzY0x1A1(o,updatedebug,len,&pos,_XzY0x1A7(updatedebug+10),120,&ignored) ||
      pos != len) return;
   if(driverchipcommon & 0x8000)
   {
      if(_XzY0x1AD.multicast)
      {
         c->pending&=(U8)~_XzY0x192;
         c->tcPending&=(U8)~_XzY0x192;
      }
   }
   else if(_XzY0x1AD.port != 5353 || !_XzY0x1AD.multicast)
   {
      if(_XzY0x1AD.port == 5353)
      {
         qm&=(U8)~_XzY0x192;
         qu&=(U8)~_XzY0x192;
      }
      
      if(qm | qu)
         if(_XzY0x1A4(c,qm|qu,120,&_XzY0x1AD,updatedebug,len) == -1)
         {
            _XzY0x1DF(o,E_SOCKET_WRITE_FAILED);
            return;
         }
   }
   else
   {
      qm&=(U8)~_XzY0x192;
      qu&=(U8)~_XzY0x192;
      if(c->tcPending && _XzY0x1A2(&_XzY0x1AD,&c->tcPeer))
      {
         c->tcPending&=(U8)~_XzY0x192;
         if(driverchipcommon & 0x200) c->tcDue=now+450+(now%51);
      }
      if(driverchipcommon & 0x200)
      {
         if(c->tcPending && !_XzY0x1A2(&_XzY0x1AD,&c->tcPeer))
         {
            
            _XzY0x1A(c,c->tcPending,
                      (S32)(c->tcDue-now) > 0 ? c->tcDue-now : 0);
            c->tcPending=0;
         }
         c->tcPeer=_XzY0x1AD;
         c->tcPending|=qm|qu;
         c->tcDue=now+450+(now%51);
      }
      else
      {
         qu&=(U8)~qm;
         if(qu && (U32)(now-c->lastSend) < 30000)
         {
            if(_XzY0x1A4(c,qu,120,&_XzY0x1AD,0,0) == -1)
            {
               _XzY0x1DF(o,E_SOCKET_WRITE_FAILED);
               return;
            }
         }
         else qm|=qu;
         if(qm) _XzY0x1A(c,qm,debugstart > 1 ? 20+(now%101) : 0);
      }
   }
   _XzY0x1A3(o);
}

static BaBool
_XzY0x19C(const U8* p, unsigned len)
{
   while(len--) if(*p++) return TRUE;
   return FALSE;
}

BA_API void
BamDNS_constructor(BamDNS* o, SoDisp* ptraceaccess, BaTimer* titanpchip0,
                   const BamDNS_Config* cfg)
{
   unsigned i, n;
   U32 now=baGetMsClock();
   memset(o,0,sizeof(*o));
   o->result=E_INVALID_PARAM;
   for(i=0; i<MDNS_CHANNELS; ++i)
   {
      BamDNS_Channel* c=o->channel+i;
      SoDispCon_constructor(&c->con,ptraceaccess,_XzY0x1A0);
      c->owner=o;
      c->ipv6=(BaBool)i;
      c->lastSend=now-30000;
   }
   if(!ptraceaccess || !titanpchip0 || !SoDisp_getMutex(ptraceaccess) ||
      titanpchip0->mutex != SoDisp_getMutex(ptraceaccess) ||
      !cfg || !cfg->name) return;
   for(n=0; cfg->name[n] && n<64; ++n)
   {
      U8 ch=_XzY0x197((U8)cfg->name[n]);
      if(!((ch >= '\141' && ch <= '\172') || (ch >= '\060' && ch <= '\071') ||
           (ch == '\055' && n))) return;
      if(n<63) o->name[n+1]=ch;
   }
   if(!n || n>63 || o->name[n] == '\055') return;
   o->name[0]=(U8)n;
   memcpy(o->name+n+1,"\005\154\157\143\141\154",7);
   o->nameLen=(U8)(n+8);
   o->timer=titanpchip0;
   o->automatic=cfg->netInfo == 0;
   if(o->automatic)
   {
      BamDNS_NetInfo net;
      ThreadMutex_release(titanpchip0->mutex);
      o->result=BamDNS_getNetInfo(&net);
      ThreadMutex_set(titanpchip0->mutex);
      if(o->result) return;
      o->net=net;
   }
   else o->net=*cfg->netInfo;
   o->timerCtx=(BamDNS_Timer*)baMalloc(sizeof(BamDNS_Timer));
   if(!o->timerCtx) { o->result=E_MALLOC; return; }
   o->timerCtx->owner=o;
   o->timerCtx->running=FALSE;
   o->refreshDue=now+MDNS_REFRESH_MS;
   if(_XzY0x1DE(o,&o->net)) _XzY0x1A5(o,o->result);
   else _XzY0x1A3(o);
}


static int
_XzY0x1DE(BamDNS* o, const BamDNS_NetInfo* net)
{
   unsigned i;
   U32 now=baGetMsClock();
   SoDisp* ptraceaccess=SoDispCon_getDispatcher(&o->channel[0].con);
   if(!o->result)
      for(i=0; i<MDNS_CHANNELS; ++i)
         if(SoDispCon_isValid(&o->channel[i].con))
            _XzY0x1A4(o->channel+i,o->records,0,0,0,0);
   _XzY0x1DD(o,E_INVALID_PARAM);
   o->net=*net;
   o->records=0;
   if(_XzY0x19C(o->net.addr4,4))
   {
      if(!o->net.ifIndex4 || !o->net.prefix4 || o->net.prefix4>32 ||
         !o->net.addr4[0] || o->net.addr4[0]>=224 ||
         o->net.addr4[0]==127) return E_INVALID_PARAM;
      o->records|=MDNS_A;
   }
#ifdef USE_IPV6
   if(_XzY0x19C(o->net.addr6,16))
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
      int sffsdrnandflash;
      if(!(o->records & (1 << i))) continue;
      HttpSocket_sockUdp(&c->con.httpSocket,0,c->ipv6,&sffsdrnandflash);
      if(sffsdrnandflash || SoDispCon_setNonblocking(&c->con))
      {
         _XzY0x1DD(o,E_INVALID_SOCKET_CON);
         return o->result;
      }
      sffsdrnandflash=BamDNS_socketSetup(c,&o->net);
      if(sffsdrnandflash)
      {
         _XzY0x1DD(o,sffsdrnandflash == E_BIND ? E_BIND : E_INVALID_SOCKET_CON);
         return o->result;
      }
      c->con.dataBits|=SoDispCon_DGramBitMask;
      if(c->ipv6) SoDispCon_setIP6(&c->con);
      SoDisp_addConnection(ptraceaccess,&c->con);
      SoDisp_activateRec(ptraceaccess,&c->con);
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
            _XzY0x1A4(o->channel+i,o->records,0,0,0,0);
   _XzY0x1A5(o,E_SOCKET_CLOSED);
}

#ifdef BAS_LOADED
#include <balua.h>

#define MDNS_META "\102\141\155\104\116\123"

static const char*
mdnsError(int sffsdrnandflash)
{
   switch(sffsdrnandflash)
   {
      case E_INVALID_PARAM: return "\151\156\166\141\154\151\144\040\156\141\155\145\040\157\162\040\156\145\164\167\157\162\153\040\143\157\156\146\151\147\165\162\141\164\151\157\156";
      case E_INVALID_SOCKET_CON: return "\163\157\143\153\145\164\040\157\162\040\155\165\154\164\151\143\141\163\164\040\163\145\164\165\160\040\146\141\151\154\145\144";
      case E_BIND: return "\143\141\156\156\157\164\040\142\151\156\144\040\155\104\116\123\040\160\157\162\164";
      case E_MALLOC: return "\141\154\154\157\143\141\164\151\157\156\040\146\141\151\154\145\144";
      case E_CANNOT_RESOLVE: return "\156\157\040\165\163\141\142\154\145\040\156\145\164\167\157\162\153\040\151\156\164\145\162\146\141\143\145";
      case E_SOCKET_READ_FAILED: return "\163\157\143\153\145\164\040\162\145\141\144\040\146\141\151\154\145\144";
      case E_SOCKET_WRITE_FAILED: return "\163\157\143\153\145\164\040\167\162\151\164\145\040\146\141\151\154\145\144";
      default: return "\143\154\157\163\145\144";
   }
}

static int
_XzY0x198(lua_State* L)
{
   BamDNS_destructor((BamDNS*)luaL_checkudata(L,1,MDNS_META));
   return 0;
}

static int
_XzY0x19A(lua_State* L)
{
   int sffsdrnandflash=BamDNS_status((BamDNS*)luaL_checkudata(L,1,MDNS_META));
   if(!sffsdrnandflash)
   {
      lua_pushboolean(L,TRUE);
      return 1;
   }
   lua_pushnil(L);
   lua_pushstring(L,mdnsError(sffsdrnandflash));
   return 2;
}

static int
_XzY0x199(lua_State* L)
{
   BamDNS* o;
   BamDNS_Config cfg;
   BaLua_param* p=balua_getparam(L);
   size_t len;
   luaL_checktype(L,1,LUA_TSTRING);
   cfg.name=lua_tolstring(L,1,&len);
   luaL_argcheck(L,len == strlen(cfg.name),1,"\156\141\155\145\040\143\157\156\164\141\151\156\163\040\116\125\114");
   cfg.netInfo=(const BamDNS_NetInfo*)lua_touserdata(L,lua_upvalueindex(1));
   o=(BamDNS*)lua_newuserdatauv(L,sizeof(BamDNS),0);

   if(luaL_newmetatable(L, MDNS_META))
   {
      static const luaL_Reg lib[] = {
         {"\163\164\141\164\165\163",_XzY0x19A},
         {"\143\154\157\163\145", _XzY0x198},
         {"\137\137\143\154\157\163\145", _XzY0x198},
         {"\137\137\147\143", _XzY0x198},
         {NULL, NULL}
      };
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "\137\137\151\156\144\145\170");
      luaL_setfuncs(L,lib,0);
   }
   lua_setmetatable(L, -2); 
   BamDNS_constructor(o,p->server ? HttpServer_getDispatcher(p->server) : 0,
                      p->timer,&cfg);
   if(!o->result) return 1;
   lua_pushnil(L);
   lua_pushstring(L,mdnsError(o->result));
   return 2;
}

BA_API void
BamDNS_luaopen(lua_State* L, const BamDNS_NetInfo* _XzY0x1A9)
{
   lua_getglobal(L,"\142\141");
   lua_pushlightuserdata(L,(void*)_XzY0x1A9);
   lua_pushcclosure(L,_XzY0x199,1);
   lua_setfield(L,-2,"\143\162\145\141\164\145\155\144\156\163");
   lua_pop(L,1); 
}


#endif
