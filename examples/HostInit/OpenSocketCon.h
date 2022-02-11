/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Embedded Web-Server 
 ****************************************************************************
 *            HEADER
 *
 *   $Id: OpenSocketCon.h 4914 2021-12-01 18:24:30Z wini $
 *
 *   COPYRIGHT:  Real Time Logic
 *               https://realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************


This header file contains the openSocketCon function, which is used by
all Barracuda examples when opening the server listen port.

The default port is 80 for HTTP and 443 for HTTPS. The openSocketCon
attempts to open a socket connection on port 80 if built as a release
build. If port 80 cannot be opened or if the code is compiled in debug
mode, port 9357 is opened.

Many of the examples assume the server is listening on port 9357, thus
some links may not work if the server is not listening on port 9357.

The openSocketCon is designed as C code, thus it can be used by C and
C++ code.
*/


#include <HttpTrace.h>
#include <HttpServCon.h>
#include <HttpSharkSslServCon.h>
#include <BaErrorCodes.h>
#include "localhost_RSA_2048.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef NO_SHARKSSL
SharkSsl sharkSsl;
HttpSharkSslServCon httpSharkSslServCon;
#ifdef USE_IPV6
HttpSharkSslServCon httpSharkSslServCon6;
#endif
#endif
HttpServCon httpServCon;
#ifdef USE_IPV6
HttpServCon httpServCon6;
#endif
#ifdef __cplusplus
}
#endif




static void
openSocketCon(HttpServer* server, SoDisp* disp)
{
   short port;

   /* Try the default server listen port.
      If unable to open, try ports in the range 9357 to 9370
   */
#if defined(HTTP_PORT) || !defined(BARRACUDA_DEBUG)
#ifdef HTTP_PORT
   port = HTTP_PORT;
#else
   port = 80; /* HTTP default port */
#endif
   HttpServCon_constructor(&httpServCon, server, disp, port, FALSE, 0, 0);
   if( !HttpServCon_isValid(&httpServCon) )
#endif
   {
      for(port = 9357 ; port < 9370; port++)
      {
         if( ! HttpServCon_setPort(&httpServCon, port, FALSE, 0) )
            break;
      }
   }
   if( !HttpServCon_isValid(&httpServCon) )
      baFatalE(FE_USER_ERROR_1, 0);
#ifdef USE_IPV6
   HttpServCon_constructor(&httpServCon6, server, disp, port, TRUE, 0, 0);
#endif

   HttpTrace_printf(0,"HTTP: Server listening on IPv4 port %d",port);
#ifdef USE_IPV6
   if( HttpServCon_isValid(&httpServCon6) )
      HttpTrace_printf(0," and IPv6 port %d",port);
#endif
   HttpTrace_printf(0,".\n");

#ifndef NO_SHARKSSL
   {
      /* Try the default server listen port.
         If unable to open, try ports in the range 9443 to 9460
      */
#if defined(HTTPS_PORT) || !defined(BARRACUDA_DEBUG)
#ifdef HTTPS_PORT
   port = HTTPS_PORT;
#else
      port=443;
#endif
      SharkSsl_constructor(&sharkSsl,
                           SharkSsl_Server,
                           16,
                           4048,
                           4048);
      SharkSsl_addCertificate(&sharkSsl, sharkSslRSACert2048_localhost);
      HttpSharkSslServCon_constructor(
         &httpSharkSslServCon,
         &sharkSsl,
         server,
         disp,
         port,                      /* U16 port=443 */
         FALSE,              /* BaBool setIP6=FALSE */
         0,           /* const void* interfaceName) */
         0);                   /* userDefinedAccept */
      if( !HttpServCon_isValid((HttpServCon*)&httpSharkSslServCon) )
#endif
      {
         for(port = 9443 ; port < 9460; port++)
         {
            if( ! HttpSharkSslServCon_setPort(&httpSharkSslServCon, port, FALSE, 0) )
               break;
         }
      }
      if( !HttpServCon_isValid((HttpServCon*)&httpSharkSslServCon) )
         baFatalE(FE_USER_ERROR_2, 0);

#ifdef USE_IPV6
      HttpSharkSslServCon_constructor(
         &httpSharkSslServCon6,
         &sharkSsl,
         server,
         disp,
         port,                      /* U16 port=443 */
         TRUE,               /* BaBool setIP6=FALSE */
         0,           /* const void* interfaceName) */
         0);                   /* userDefinedAccept */
#endif


      HttpTrace_printf(0,"HTTPS: SharkSSL listening on IPv4 port %d", port);
#ifdef USE_IPV6
   if( HttpServCon_isValid(&httpSharkSslServCon6) )
      HttpTrace_printf(0," and IPv6 port %d",port);
#endif
   HttpTrace_printf(0,".\n");
   }
#endif /* ifdef SharkSSL */
}
