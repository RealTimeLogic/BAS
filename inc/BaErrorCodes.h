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
 *   $Id: BaErrorCodes.h 4915 2021-12-01 18:26:55Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2003-2021
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
 */
#ifndef __BaErrorCodes_h
#define __BaErrorCodes_h



typedef enum {
   FE_MALLOC = HTTP_E_BASE, /* error code 2 = size needed */
   FE_ASSERT,               /* error code 2 not used */
   FE_NO_SERV_CON,          /* error code 2 not used */
   FE_SOCKET,               /* error code 2 = errno */
   FE_GETHOSTBYNAME,        /* error code 2 = errno */
   FE_GETHOSTBYNAME2,       /* error code 2 = errno */
   FE_BIND,                 /* error code 2 = errno */
   FE_LISTEN,               /* error code 2 = errno */
   FE_IOCTL,                /* error code 2 = errno */
   FE_SETSOCKOPT,           /* error code 2 = errno */
   FE_ACCEPT,               /* error code 2 = errno */
   FE_NO_IPV6_SUPPORT,      /* error code 2 not used */
   FE_INVALID_CSPREADER,    /* error code 2 not used */
   FE_CANNOT_READ,          /* error code 2 is the offset position */
   FE_MAGIC_NO,             /* error code 2 not used */
   FE_THREAD_LIB,           /* See threadlib for error code 2 */
   FE_SSL_ERROR,                 /* error code 2 not used */
   FE_HTTPCON_INVALID_DISPEV,    /* error code 2 not used */
   FE_BLUA_PANIC,                /* Lua panic: error 2 is code from exit() */
   FE_EXIT,
   FE_INCORRECT_USE,
   FE_TYPE_SIZE_ERROR, /* One of U8 to U32 are of incorrect size */
   FE_WRONG_ENDIAN, /* Incorrect B_LITTLE_ENDIAN/B_BIG_ENDIAN macro */

   FE_USER_ERROR_1 = HTTP_E_BASE+100,
   FE_USER_ERROR_2,
   FE_USER_ERROR_3,
   FE_USER_ERROR_4,
   FE_USER_ERROR_5,
   FE_USER_ERROR_6,
   FE_USER_ERROR_7,
   FE_USER_ERROR_8,
   FE_USER_ERROR_9,
   FE_USER_ERROR_10
} BaFatalErrorCodes;


typedef enum {
   E_NO_ERROR=0,
   E_INVALID_SOCKET_CON=-1000,
   E_GETHOSTBYNAME,
   E_BIND,
   E_SOCKET_CLOSED,
   E_SOCKET_WRITE_FAILED,
   E_SOCKET_READ_FAILED,
   E_TIMEOUT,
   E_MALLOC,
   E_ALREADY_INSERTED,
   E_TOO_MUCH_DATA,
   E_PAGE_NOT_FOUND, /* for HttpResponse include or forward */
   E_IS_COMMITTED,
   E_INVALID_PARAM,
   E_MIXING_WRITE_SEND,
   E_TOO_MANY_INCLUDES,
   E_TOO_MANY_FORWARDS,
   E_INCLUDE_OP_NOT_VALID,

   E_CANNOT_RESOLVE,
   E_CANNOT_CONNECT,
   E_INVALID_URL,
   E_INVALID_RESPONSE,

   E_INCORRECT_USE,  /* The API is not used correctly */

   E_TLS_NOT_ENABLED = -400,
   E_SHARK_ALERT_RECV, /* Call SharkSslCon_getAlertDescription */
   E_TLS_CRYPTOERR,
   E_TLS_HANDSHAKE,
   E_NOT_TRUSTED, /* SSL certificate not trusted or domain mismatch */
   E_TLS_CLOSE_NOTIFY, /* Peer is closing the connection */

   E_PROXY_AUTH = -500, /* Authentication required or wrong credentials */
   E_PROXY_GENERAL, /* general SOCKS server failure */
   E_PROXY_NOT_ALLOWED, /* connection not allowed by ruleset */
   E_PROXY_NETWORK, /* Network unreachable */
   E_PROXY_HOST, /* Host unreachable */
   E_PROXY_REFUSED, /* Connection refused */
   E_PROXY_TTL, /* TTL expired */
   E_PROXY_COMMAND_NOT_SUP, /* Command not supported */
   E_PROXY_ADDRESS_NOT_SUP, /* Address type not supported */
   E_PROXY_NOT_COMPATIBLE, /* Not a supported SOCKS version */
   E_PROXY_READY,/*Set HttpClient, not an error: socket in state proxy ready*/
   E_PROXY_UNKNOWN, /* Unkown socks err */

   E_SYS_SHUTDOWN = -600 /* Used by Mako Server (and others) when program exits */
} BaErrorCodes;



#define baFatalE(ecode1, ecode2) baFatalEf(ecode1, ecode2, __FILE__, __LINE__)
#ifdef __cplusplus
extern "C" {
#endif
BA_API void baFatalEf(BaFatalErrorCodes ecode1, unsigned int ecode2,
                 const char* file, int line);

BA_API const char* baErr2Str(int ecode);

BA_API int baErr2HttpCode(int ecode);
#ifdef __cplusplus
}
#endif



#endif /*  __BaErrorCodes_h */
