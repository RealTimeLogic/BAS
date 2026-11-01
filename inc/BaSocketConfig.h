/* Copyright (C) Real Time Logic LLC, 2026.
 * Network defaults and validation after the selected HttpCfg.h.
 * Port capability definitions take precedence; applications use BaConfigOpts.h. */
#ifndef BA_SOCKET_CONFIG_H
#define BA_SOCKET_CONFIG_H

#ifndef BA_USE_GETADDRINFO
#define BA_USE_GETADDRINFO 0
#endif
#if (BA_USE_GETADDRINFO != 0) && (BA_USE_GETADDRINFO != 1)
#error BA_USE_GETADDRINFO must be 0 or 1
#endif

#ifndef BA_LWIP_USE_NETCONN_CALLBACK_DATA
#define BA_LWIP_USE_NETCONN_CALLBACK_DATA 0
#endif
#if (BA_LWIP_USE_NETCONN_CALLBACK_DATA != 0) && (BA_LWIP_USE_NETCONN_CALLBACK_DATA != 1)
#error BA_LWIP_USE_NETCONN_CALLBACK_DATA must be 0 or 1
#endif

#ifndef BA_HAVE_SODISPCON_RECVFROM
#define BA_HAVE_SODISPCON_RECVFROM 0
#endif
#if (BA_HAVE_SODISPCON_RECVFROM != 0) && (BA_HAVE_SODISPCON_RECVFROM != 1)
#error BA_HAVE_SODISPCON_RECVFROM must be 0 or 1
#endif

#ifndef BA_ENABLE_UDP
#define BA_ENABLE_UDP 0
#endif
#if (BA_ENABLE_UDP != 0) && (BA_ENABLE_UDP != 1)
#error BA_ENABLE_UDP must be 0 or 1
#endif

#ifndef BA_ENABLE_IPV6
#define BA_ENABLE_IPV6 0
#endif
#if (BA_ENABLE_IPV6 != 0) && (BA_ENABLE_IPV6 != 1)
#error BA_ENABLE_IPV6 must be 0 or 1
#endif

#ifndef BA_HAVE_SOCKET_WRITE_EVENTS
#define BA_HAVE_SOCKET_WRITE_EVENTS 1
#endif
#if (BA_HAVE_SOCKET_WRITE_EVENTS != 0) && (BA_HAVE_SOCKET_WRITE_EVENTS != 1)
#error BA_HAVE_SOCKET_WRITE_EVENTS must be 0 or 1
#endif

#ifndef BA_HAVE_IPV6_MULTICAST_MEMBERSHIP
#define BA_HAVE_IPV6_MULTICAST_MEMBERSHIP 1
#endif
#if (BA_HAVE_IPV6_MULTICAST_MEMBERSHIP != 0) && (BA_HAVE_IPV6_MULTICAST_MEMBERSHIP != 1)
#error BA_HAVE_IPV6_MULTICAST_MEMBERSHIP must be 0 or 1
#endif

#ifndef BA_HAVE_TCP_KEEPALIVE_TUNING
#define BA_HAVE_TCP_KEEPALIVE_TUNING 1
#endif
#if (BA_HAVE_TCP_KEEPALIVE_TUNING != 0) && (BA_HAVE_TCP_KEEPALIVE_TUNING != 1)
#error BA_HAVE_TCP_KEEPALIVE_TUNING must be 0 or 1
#endif

#endif
