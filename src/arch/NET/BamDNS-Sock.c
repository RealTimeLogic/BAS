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

/* xrc/misc/BamDNS.c BSD interface code.
   Narrow extension for BAS BSD sockets: selected-link packet metadata and
   scoped datagrams. No change to HttpSockaddr or existing socket signatures.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#if defined(__APPLE__) && !defined(__APPLE_USE_RFC_3542)
#define __APPLE_USE_RFC_3542
#endif
#ifndef BA_LIB
#define BA_LIB 1
#endif
#include <BamDNS.h>
#include <string.h>
#ifdef BA_WINDOWS
#include <Ws2tcpip.h>
#include <Mswsock.h>
#include <iphlpapi.h>
#ifdef _MSC_VER
#pragma comment(lib,"iphlpapi.lib")
#endif
#define MDNS_MSG WSAMSG
#define MDNS_CMSG WSACMSGHDR
#define MDNS_FIRST WSA_CMSG_FIRSTHDR
#define MDNS_NEXT WSA_CMSG_NXTHDR
#define MDNS_DATA WSA_CMSG_DATA
#define MDNS_LEN WSA_CMSG_LEN
#define MDNS_SPACE WSA_CMSG_SPACE
#else
#include <sys/uio.h>
#if defined(__ANDROID__) && \
   (!defined(__ANDROID_API__) || __ANDROID_API__ < 24)
/* Old Bionic has no getifaddrs. Query the kernel once during host setup. */
#define MDNS_NETLINK
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/time.h>
#ifndef IFA_F_DADFAILED
/* Linux netlink ABI flag omitted by early Android headers. */
#define IFA_F_DADFAILED 0x08
#endif
#else
#include <ifaddrs.h>
#endif
#ifdef __QNXNTO__
#include <net/if_dl.h>
#include <stddef.h>
#endif
#define MDNS_MSG struct msghdr
#define MDNS_CMSG struct cmsghdr
#define MDNS_FIRST CMSG_FIRSTHDR
#define MDNS_NEXT CMSG_NXTHDR
#define MDNS_DATA CMSG_DATA
#define MDNS_LEN CMSG_LEN
#define MDNS_SPACE CMSG_SPACE
#endif

/* Desktop discovery is a host setup convenience, separate from the responder.
 * Store at most one address per family, always on the same selected link.
 */
static void
mdnsNetAddress(BamDNS_NetInfo* net, const struct sockaddr* sa,
               U32 index, U8 prefix)
{
   const U8* addr;
   if(!index || !prefix) return;
   if(sa->sa_family == AF_INET && !net->ifIndex4 && prefix <= 32)
   {
      addr=(const U8*)&((const struct sockaddr_in*)sa)->sin_addr;
      if(!addr[0] || addr[0]==127 || addr[0]>=224) return;
      net->ifIndex4=index;
      net->prefix4=prefix;
      memcpy(net->addr4,addr,4);
   }
#ifdef USE_IPV6
   if(sa->sa_family == AF_INET6 && prefix <= 128)
   {
      addr=(const U8*)&((const struct sockaddr_in6*)sa)->sin6_addr;
      if(!addr[0] || addr[0]==255) return;
      if(net->ifIndex6 &&
         ((net->addr6[0]==0xfe && (net->addr6[1]&0xc0)==0x80) ||
          !(addr[0]==0xfe && (addr[1]&0xc0)==0x80))) return;
      net->ifIndex6=index;
      net->prefix6=prefix;
      memcpy(net->addr6,addr,16);
   }
#endif
}

static BaBool
mdnsHasNet(const BamDNS_NetInfo* net)
{
   return net->ifIndex4
#ifdef USE_IPV6
      || net->ifIndex6
#endif
      ? TRUE : FALSE;
}

#ifdef MDNS_NETLINK
static int
mdnsGetNetInfo(BamDNS_NetInfo* net)
{
   struct
   {
      struct nlmsghdr hdr;
      struct ifaddrmsg addr;
   } request;
   union
   {
      struct nlmsghdr align;
      U8 bytes[8192];
   } buf;
   struct sockaddr_nl kernel;
   struct timeval timeout;
   U32 selected=0;
   int fd, control, result=E_INVALID_SOCKET_CON;
   fd=socket(AF_NETLINK,SOCK_DGRAM,NETLINK_ROUTE);
   if(fd < 0) return result;
   control=socket(AF_INET,SOCK_DGRAM,0);
   if(control < 0) { close(fd); return result; }
   memset(&kernel,0,sizeof(kernel));
   kernel.nl_family=AF_NETLINK;
   memset(&request,0,sizeof(request));
   request.hdr.nlmsg_len=NLMSG_LENGTH(sizeof(request.addr));
   request.hdr.nlmsg_type=RTM_GETADDR;
   request.hdr.nlmsg_flags=NLM_F_REQUEST|NLM_F_DUMP;
   request.hdr.nlmsg_seq=1;
   request.addr.ifa_family=
#ifdef USE_IPV6
      AF_UNSPEC;
#else
      AF_INET;
#endif
   timeout.tv_sec=1;
   timeout.tv_usec=0;
   if(setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout)) ||
      sendto(fd,&request,request.hdr.nlmsg_len,0,
             (struct sockaddr*)&kernel,sizeof(kernel)) !=
         (int)request.hdr.nlmsg_len) goto done;
   for(;;)
   {
      struct nlmsghdr* h;
      struct sockaddr_nl peer;
      socklen_t peerLen=sizeof(peer);
      int len=recvfrom(fd,buf.bytes,sizeof(buf.bytes),MSG_TRUNC,
                       (struct sockaddr*)&peer,&peerLen);
      if(len <= 0 || len > (int)sizeof(buf.bytes) ||
         peerLen != sizeof(peer) || peer.nl_pid) goto done;
      for(h=(struct nlmsghdr*)buf.bytes;
          len > 0 && NLMSG_OK(h,(unsigned)len); h=NLMSG_NEXT(h,len))
      {
         struct ifaddrmsg* a;
         struct rtattr* attr;
         struct ifreq ifr;
         const void* address=0;
         int size, remaining;
         union
         {
            struct sockaddr sa;
            struct sockaddr_in v4;
#ifdef USE_IPV6
            struct sockaddr_in6 v6;
#endif
         } local;
         if(h->nlmsg_seq != 1) goto done;
#ifdef NLM_F_DUMP_INTR
         if(h->nlmsg_flags & NLM_F_DUMP_INTR) goto done;
#endif
         if(h->nlmsg_type==NLMSG_DONE)
         {
            if(NLMSG_PAYLOAD(h,0) >= sizeof(int) &&
               *(int*)NLMSG_DATA(h)) goto done;
            result=mdnsHasNet(net) ? 0 : E_CANNOT_RESOLVE;
            goto done;
         }
         if(h->nlmsg_type==NLMSG_ERROR) goto done;
         if(h->nlmsg_type!=RTM_NEWADDR ||
            NLMSG_PAYLOAD(h,0) < sizeof(*a)) continue;
         a=(struct ifaddrmsg*)NLMSG_DATA(h);
         if(selected && selected!=(U32)a->ifa_index) continue;
         memset(&local,0,sizeof(local));
         if(a->ifa_family==AF_INET) size=4;
#ifdef USE_IPV6
         else if(a->ifa_family==AF_INET6)
         {
            if(a->ifa_flags & (IFA_F_TENTATIVE|IFA_F_DADFAILED|IFA_F_DEPRECATED))
               continue;
            size=16;
         }
#endif
         else continue;
         memset(&ifr,0,sizeof(ifr));
         ifr.ifr_ifindex=a->ifa_index;
         if(ioctl(control,SIOCGIFNAME,&ifr) ||
            ioctl(control,SIOCGIFFLAGS,&ifr) ||
            (ifr.ifr_flags & (IFF_UP|IFF_RUNNING|IFF_MULTICAST)) !=
               (IFF_UP|IFF_RUNNING|IFF_MULTICAST) ||
            (ifr.ifr_flags & (IFF_LOOPBACK|IFF_POINTOPOINT))) continue;
         remaining=IFA_PAYLOAD(h);
         for(attr=IFA_RTA(a); RTA_OK(attr,remaining);
             attr=RTA_NEXT(attr,remaining))
            if(RTA_PAYLOAD(attr)==(unsigned)size &&
               (attr->rta_type==IFA_LOCAL ||
                (attr->rta_type==IFA_ADDRESS && !address)))
               address=RTA_DATA(attr);
         if(!address || remaining) continue;
         local.sa.sa_family=a->ifa_family;
         if(size==4) memcpy(&local.v4.sin_addr,address,4);
#ifdef USE_IPV6
         else memcpy(&local.v6.sin6_addr,address,16);
#endif
         mdnsNetAddress(net,&local.sa,a->ifa_index,a->ifa_prefixlen);
         if(mdnsHasNet(net)) selected=a->ifa_index;
      }
      if(len) goto done;
   }
done:
   close(control);
   close(fd);
   if(result) memset(net,0,sizeof(*net));
   return result;
}
#elif !defined(BA_WINDOWS)
static U8
mdnsNetPrefix(const U8* mask, unsigned len)
{
   U8 prefix=0;
   BaBool zero=FALSE;
   unsigned i, bit;
   for(i=0; i<len; ++i)
      for(bit=128; bit; bit>>=1)
      {
         if(mask[i]&bit)
         {
            if(zero) return 0;
            ++prefix;
         }
         else zero=TRUE;
      }
   return prefix;
}
#endif

BA_API int
BamDNS_getNetInfo(BamDNS_NetInfo* net)
{
   if(!net) return E_INVALID_PARAM;
   memset(net,0,sizeof(*net));
#ifdef BA_WINDOWS
   {
      IP_ADAPTER_ADDRESSES* list=0;
      IP_ADAPTER_ADDRESSES* a;
      ULONG size=15000, result=ERROR_BUFFER_OVERFLOW;
      unsigned tries;
      for(tries=0; tries<3 && result==ERROR_BUFFER_OVERFLOW; ++tries)
      {
         baFree(list);
         list=(IP_ADAPTER_ADDRESSES*)baMalloc(size);
         if(!list) return E_MALLOC;
         result=GetAdaptersAddresses(
#ifdef USE_IPV6
            AF_UNSPEC,
#else
            AF_INET,
#endif
            GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER,0,list,&size);
      }
      if(result != NO_ERROR)
      {
         baFree(list);
         return result==ERROR_NO_DATA ? E_CANNOT_RESOLVE : E_INVALID_SOCKET_CON;
      }
      for(a=list; a; a=a->Next)
      {
         IP_ADAPTER_UNICAST_ADDRESS* u;
         if(a->OperStatus != IfOperStatusUp ||
            (a->Flags & (IP_ADAPTER_NO_MULTICAST | IP_ADAPTER_RECEIVE_ONLY)) ||
            (a->IfType != IF_TYPE_ETHERNET_CSMACD &&
             a->IfType != IF_TYPE_IEEE80211)) continue;
         for(u=a->FirstUnicastAddress; u; u=u->Next)
         {
            const struct sockaddr* sa=u->Address.lpSockaddr;
            U32 index=a->IfIndex;
            if(!sa || u->DadState != IpDadStatePreferred ||
               !u->ValidLifetime || (u->Flags & IP_ADAPTER_ADDRESS_TRANSIENT))
               continue;
#ifdef USE_IPV6
            if(sa->sa_family==AF_INET6) index=a->Ipv6IfIndex;
#endif
            mdnsNetAddress(net,sa,index,u->OnLinkPrefixLength);
         }
         if(mdnsHasNet(net)) break;
      }
      baFree(list);
   }
#elif defined(MDNS_NETLINK)
   return mdnsGetNetInfo(net);
#else
   {
      struct ifaddrs *list, *a;
      const char* selected=0;
      if(getifaddrs(&list)) return E_INVALID_SOCKET_CON;
      for(a=list; a; a=a->ifa_next)
      {
         const struct sockaddr* sa=a->ifa_addr;
         U32 index;
         U8 prefix;
         if(!sa || !a->ifa_netmask ||
            (a->ifa_flags & (IFF_UP|IFF_RUNNING|IFF_MULTICAST)) !=
               (IFF_UP|IFF_RUNNING|IFF_MULTICAST) ||
            (a->ifa_flags & (IFF_LOOPBACK|IFF_POINTOPOINT)) ||
            (selected && strcmp(selected,a->ifa_name))) continue;
         index=if_nametoindex(a->ifa_name);
         if(sa->sa_family==AF_INET)
            prefix=mdnsNetPrefix((const U8*)&
               ((struct sockaddr_in*)a->ifa_netmask)->sin_addr,4);
#ifdef USE_IPV6
         else if(sa->sa_family==AF_INET6)
         {
            /* Do not select an IPv6 address that the stack cannot yet bind
             * (for example while address assignment is still tentative). */
            HttpSocket s;
            struct sockaddr_in6 local=*(const struct sockaddr_in6*)sa;
            int status;
            local.sin6_scope_id=index;
            local.sin6_port=0;
            HttpSocket_constructor(&s);
            HttpSocket_sockUdp(&s,0,TRUE,&status);
            if(!status)
            {
               status=socketBind(s.hndl,(struct sockaddr*)&local,sizeof(local));
               HttpSocket_close(&s);
            }
            if(status) continue;
            prefix=mdnsNetPrefix((const U8*)&
               ((struct sockaddr_in6*)a->ifa_netmask)->sin6_addr,16);
         }
#endif
         else continue;
         mdnsNetAddress(net,sa,index,prefix);
         if(mdnsHasNet(net)) selected=a->ifa_name;
      }
      freeifaddrs(list);
   }
#endif
   return mdnsHasNet(net) ? 0 : E_CANNOT_RESOLVE;
}

static const U8 mdnsGroup4[4]={224,0,0,251};
#ifdef USE_IPV6
static const U8 mdnsGroup6[16]={255,2,0,0,0,0,0,0,0,0,0,0,0,0,0,251};
#endif

static int
mdnsOption(HttpSocket* s, int level, int option, const void* data, int len)
{
   return socketSetsockopt(s->hndl,level,option,(const char*)data,len);
}

int
BamDNS_socketSetup(BamDNS_Channel* c, const BamDNS_NetInfo* net)
{
   HttpSocket* s=&c->con.httpSocket;
   int one=1, hops=255;
   HttpSockaddr any;
   int status;
#ifdef BA_WINDOWS
   GUID recid=WSAID_WSARECVMSG, sendid=WSAID_WSASENDMSG;
   LPFN_WSARECVMSG rec;
   LPFN_WSASENDMSG snd;
   DWORD bytes;
   if(WSAIoctl(s->hndl,SIO_GET_EXTENSION_FUNCTION_POINTER,&recid,sizeof(recid),
               &rec,sizeof(rec),&bytes,0,0) ||
      WSAIoctl(s->hndl,SIO_GET_EXTENSION_FUNCTION_POINTER,&sendid,sizeof(sendid),
               &snd,sizeof(snd),&bytes,0,0)) return -1;
   c->portData[0]=(void (*)(void))rec;
   c->portData[1]=(void (*)(void))snd;
#endif
   if(mdnsOption(s,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one))) return -1;
#if defined(__APPLE__) || defined(__QNXNTO__)
   if(mdnsOption(s,SOL_SOCKET,SO_REUSEPORT,&one,sizeof(one))) return -1;
#endif
#ifdef USE_IPV6
   if(c->ipv6 && mdnsOption(s,IPPROTO_IPV6,IPV6_V6ONLY,&one,sizeof(one)))
      return -1;
#endif
   memset(&any,0,sizeof(any));
   any.isIp6=c->ipv6;
   HttpSocket_bind(s,&any,5353,&status);
   if(status) return E_BIND;
#ifdef USE_IPV6
   if(c->ipv6)
   {
      struct ipv6_mreq m;
      memset(&m,0,sizeof(m));
      memcpy(&m.ipv6mr_multiaddr,mdnsGroup6,16);
      m.ipv6mr_interface=net->ifIndex6;
      return mdnsOption(s,IPPROTO_IPV6,
#ifdef BA_WINDOWS
                    IPV6_PKTINFO,
#else
                    IPV6_RECVPKTINFO,
#endif
                    &one,sizeof(one)) ||
         mdnsOption(s,IPPROTO_IPV6,IPV6_MULTICAST_HOPS,&hops,sizeof(hops)) ||
         mdnsOption(s,IPPROTO_IPV6,IPV6_UNICAST_HOPS,&hops,sizeof(hops)) ||
         mdnsOption(s,IPPROTO_IPV6,IPV6_MULTICAST_IF,
                    &net->ifIndex6,sizeof(net->ifIndex6)) ||
         mdnsOption(s,IPPROTO_IPV6,IPV6_JOIN_GROUP,&m,sizeof(m)) ? -1 : 0;
   }
#endif
   {
      struct ip_mreq m;
      memset(&m,0,sizeof(m));
      memcpy(&m.imr_multiaddr,mdnsGroup4,4);
      memcpy(&m.imr_interface,net->addr4,4);
#ifdef __APPLE__
      if(mdnsOption(s,IPPROTO_IP,IP_BOUND_IF,
                    &net->ifIndex4,sizeof(net->ifIndex4))) return -1;
#endif
#ifdef __QNXNTO__
      /* QNX supplies the destination and receiving link separately. */
      if(mdnsOption(s,IPPROTO_IP,IP_RECVDSTADDR,&one,sizeof(one)) ||
         mdnsOption(s,IPPROTO_IP,IP_RECVIF,&one,sizeof(one))) return -1;
#else
      if(mdnsOption(s,IPPROTO_IP,IP_PKTINFO,&one,sizeof(one))) return -1;
#endif
      /* BSD uses a byte, Windows accepts a DWORD for multicast TTL. */
#ifdef BA_WINDOWS
      if(mdnsOption(s,IPPROTO_IP,IP_MULTICAST_TTL,&hops,sizeof(hops))) return -1;
#else
      {
         U8 ttl=255;
         if(mdnsOption(s,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl))) return -1;
      }
#endif
      return mdnsOption(s,IPPROTO_IP,IP_TTL,&hops,sizeof(hops)) ||
         mdnsOption(s,IPPROTO_IP,IP_MULTICAST_IF,&m.imr_interface,4) ||
         mdnsOption(s,IPPROTO_IP,IP_ADD_MEMBERSHIP,&m,sizeof(m)) ? -1 : 0;
   }
}

/* Nonblocking datagram errors are normalized at the port boundary. */
static int
mdnsIoError(HttpSocket* s, BaBool receive)
{
   int wb;
   (void)s; /* Some socket ports obtain the error from thread-local state. */
#ifdef BA_WINDOWS
   int error=WSAGetLastError();
   if(receive && (error == WSAEMSGSIZE || error == WSAECONNRESET)) return 0;
   if(error == WSAEINTR) return -2;
#else
   if(receive && (errno == EMSGSIZE || errno == ECONNREFUSED)) return 0;
   if(errno == EINTR) return -2;
#endif
   HttpSocket_wouldBlock(s,&wb);
   return wb ? -2 : -1;
}

static BaBool
mdnsPrefix(const U8* a, const U8* b, unsigned bits)
{
   unsigned n=bits/8;
   if(memcmp(a,b,n)) return FALSE;
   bits%=8;
   return !bits || !((a[n]^b[n]) & (0xff << (8-bits)));
}

int
BamDNS_socketRecv(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                  void* data, int size, BamDNS_Peer* peer)
{
   struct sockaddr_storage from;
   MDNS_MSG msg;
   MDNS_CMSG* h;
   union { MDNS_CMSG align; U8 bytes[128]; } control;
   U32 index=0;
   U8 dest[16]={0};
   int len;
#ifdef BA_WINDOWS
   WSABUF buf;
   DWORD bytes;
   buf.buf=(char*)data;
   buf.len=(ULONG)size;
   memset(&msg,0,sizeof(msg));
   msg.name=(struct sockaddr*)&from;
   msg.namelen=sizeof(from);
   msg.lpBuffers=&buf;
   msg.dwBufferCount=1;
   msg.Control.buf=(char*)control.bytes;
   msg.Control.len=sizeof(control.bytes);
   if(((LPFN_WSARECVMSG)c->portData[0])(c->con.httpSocket.hndl,&msg,&bytes,0,0))
      return mdnsIoError(&c->con.httpSocket,TRUE);
   if(msg.dwFlags & (MSG_TRUNC | MSG_CTRUNC)) return 0;
   len=(int)bytes;
#else
   struct iovec buf;
   buf.iov_base=data;
   buf.iov_len=(size_t)size;
   memset(&msg,0,sizeof(msg));
   msg.msg_name=&from;
   msg.msg_namelen=sizeof(from);
   msg.msg_iov=&buf;
   msg.msg_iovlen=1;
   msg.msg_control=control.bytes;
   msg.msg_controllen=sizeof(control.bytes);
   len=(int)recvmsg(c->con.httpSocket.hndl,&msg,0);
   if(len < 0) return mdnsIoError(&c->con.httpSocket,TRUE);
   if(msg.msg_flags & (MSG_TRUNC | MSG_CTRUNC)) return 0;
#endif
   for(h=MDNS_FIRST(&msg); h; h=MDNS_NEXT(&msg,h))
   {
#ifdef USE_IPV6
      if(c->ipv6 && h->cmsg_level == IPPROTO_IPV6 &&
         h->cmsg_type == IPV6_PKTINFO &&
         h->cmsg_len >= MDNS_LEN(sizeof(struct in6_pktinfo)))
      {
         struct in6_pktinfo info;
         memcpy(&info,MDNS_DATA(h),sizeof(info));
         index=info.ipi6_ifindex;
         memcpy(dest,&info.ipi6_addr,16);
      }
#endif
      if(!c->ipv6 && h->cmsg_level == IPPROTO_IP)
      {
#ifdef __QNXNTO__
         if(h->cmsg_type == IP_RECVDSTADDR &&
            h->cmsg_len >= MDNS_LEN(sizeof(struct in_addr)))
            memcpy(dest,MDNS_DATA(h),4);
         else if(h->cmsg_type == IP_RECVIF &&
                 h->cmsg_len >= MDNS_LEN(offsetof(struct sockaddr_dl,sdl_data)))
         {
            struct sockaddr_dl info;
            /* The link address has variable length; only its header is needed. */
            memcpy(&info,MDNS_DATA(h),offsetof(struct sockaddr_dl,sdl_data));
            index=info.sdl_index;
         }
#else
         if(h->cmsg_type == IP_PKTINFO &&
            h->cmsg_len >= MDNS_LEN(sizeof(struct in_pktinfo)))
         {
            struct in_pktinfo info;
            memcpy(&info,MDNS_DATA(h),sizeof(info));
            index=info.ipi_ifindex;
            memcpy(dest,&info.ipi_addr,4);
         }
#endif
      }
   }
   memset(peer,0,sizeof(*peer));
   peer->addr.isIp6=c->ipv6;
#ifdef USE_IPV6
   if(c->ipv6)
   {
      struct sockaddr_in6* a=(struct sockaddr_in6*)&from;
      U8* src=(U8*)&a->sin6_addr;
      if(from.ss_family != AF_INET6 || index != net->ifIndex6) return 0;
      peer->multicast=!memcmp(dest,mdnsGroup6,16);
      if(!peer->multicast && (memcmp(dest,net->addr6,16) ||
         !((src[0]==0xfe && (src[1]&0xc0)==0x80) ||
           mdnsPrefix(src,net->addr6,net->prefix6)))) return 0;
      memcpy(peer->addr.addr,src,16);
      peer->port=baNtohs(a->sin6_port);
   }
   else
#endif
   {
      struct sockaddr_in* a=(struct sockaddr_in*)&from;
      U8* src=(U8*)&a->sin_addr;
      if(from.ss_family != AF_INET || index != net->ifIndex4) return 0;
      peer->multicast=!memcmp(dest,mdnsGroup4,4);
      if(!peer->multicast && (memcmp(dest,net->addr4,4) ||
         !((src[0]==169 && src[1]==254) ||
           mdnsPrefix(src,net->addr4,net->prefix4)))) return 0;
      memcpy(peer->addr.addr,src,4);
      peer->port=baNtohs(a->sin_port);
   }
   return peer->port ? len : 0;
}

int
BamDNS_socketSend(BamDNS_Channel* c, const BamDNS_NetInfo* net,
                  const void* data, int size, const BamDNS_Peer* peer)
{
   union
   {
      struct sockaddr_in v4;
#ifdef USE_IPV6
      struct sockaddr_in6 v6;
#endif
   } dest;
   union { MDNS_CMSG align; U8 bytes[128]; } control;
   MDNS_MSG msg;
   MDNS_CMSG* h;
   int addrlen, controllen=0;
#ifdef BA_WINDOWS
   WSABUF buf;
   DWORD bytes;
   buf.buf=(char*)data;
   buf.len=(ULONG)size;
#else
   struct iovec buf;
   int len;
   buf.iov_base=(void*)data;
   buf.iov_len=(size_t)size;
#endif
   memset(&dest,0,sizeof(dest));
   memset(&msg,0,sizeof(msg));
   memset(&control,0,sizeof(control));
   h=(MDNS_CMSG*)control.bytes;
#ifdef USE_IPV6
   if(c->ipv6)
   {
      struct in6_pktinfo info;
      memset(&info,0,sizeof(info));
      info.ipi6_ifindex=net->ifIndex6;
      memcpy(&info.ipi6_addr,net->addr6,16);
      dest.v6.sin6_family=AF_INET6;
#ifdef __QNXNTO__
      dest.v6.sin6_len=sizeof(dest.v6);
#endif
      dest.v6.sin6_port=baHtons(peer ? peer->port : 5353);
      dest.v6.sin6_scope_id=net->ifIndex6;
      memcpy(&dest.v6.sin6_addr,peer ? (const U8*)peer->addr.addr : mdnsGroup6,16);
      addrlen=sizeof(dest.v6);
      h->cmsg_level=IPPROTO_IPV6;
      h->cmsg_type=IPV6_PKTINFO;
      h->cmsg_len=MDNS_LEN(sizeof(info));
      memcpy(MDNS_DATA(h),&info,sizeof(info));
      controllen=(int)MDNS_SPACE(sizeof(info));
   }
   else
#endif
   {
      dest.v4.sin_family=AF_INET;
#ifdef __QNXNTO__
      dest.v4.sin_len=sizeof(dest.v4);
#endif
      dest.v4.sin_port=baHtons(peer ? peer->port : 5353);
      memcpy(&dest.v4.sin_addr,peer ? (const U8*)peer->addr.addr : mdnsGroup4,4);
      addrlen=sizeof(dest.v4);
#ifdef __QNXNTO__
      h->cmsg_level=IPPROTO_IP;
      h->cmsg_type=IP_SENDSRCADDR;
      h->cmsg_len=MDNS_LEN(sizeof(struct in_addr));
      memcpy(MDNS_DATA(h),net->addr4,4);
      controllen=(int)MDNS_SPACE(sizeof(struct in_addr));
#else
      {
         struct in_pktinfo info;
         memset(&info,0,sizeof(info));
#ifndef __APPLE__
         info.ipi_ifindex=net->ifIndex4;
#endif
#if defined(BA_WINDOWS) || defined(BA_VXWORKS)
         memcpy(&info.ipi_addr,net->addr4,4);
#else
         /* macOS uses IP_BOUND_IF for scope and zero ipi_ifindex here so
          * that ipi_spec_dst selects the exact source rather than a primary. */
         memcpy(&info.ipi_spec_dst,net->addr4,4);
#endif
         h->cmsg_level=IPPROTO_IP;
         h->cmsg_type=IP_PKTINFO;
         h->cmsg_len=MDNS_LEN(sizeof(info));
         memcpy(MDNS_DATA(h),&info,sizeof(info));
         controllen=(int)MDNS_SPACE(sizeof(info));
      }
#endif
   }
#ifdef BA_WINDOWS
   msg.name=(struct sockaddr*)&dest;
   msg.namelen=addrlen;
   msg.lpBuffers=&buf;
   msg.dwBufferCount=1;
   msg.Control.buf=(char*)control.bytes;
   msg.Control.len=(ULONG)controllen;
   if(((LPFN_WSASENDMSG)c->portData[1])(c->con.httpSocket.hndl,&msg,0,&bytes,0,0))
      return mdnsIoError(&c->con.httpSocket,FALSE);
   return (int)bytes;
#else
   msg.msg_name=&dest;
   msg.msg_namelen=(socklen_t)addrlen;
   msg.msg_iov=&buf;
   msg.msg_iovlen=1;
   if(controllen)
   {
      msg.msg_control=control.bytes;
      msg.msg_controllen=(size_t)controllen;
   }
   len=(int)sendmsg(c->con.httpSocket.hndl,&msg,0);
   return len < 0 ? mdnsIoError(&c->con.httpSocket,FALSE) : len;
#endif
}
