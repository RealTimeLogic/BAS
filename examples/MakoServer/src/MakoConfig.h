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
 *   $Id: mako.h 6615 2026-10-25 18:37:54Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2012 - 2026
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
/* Host options. Loaded after MakoExt1.ch and before MakoExt2.ch uses them.
 * Library options (including the Lua module interface and syslog) belong in
 * BaConfigOpts.h or definitions applied to every translation unit.
 */
#ifndef _MakoConfig_h
#define _MakoConfig_h
#include <TargConfig.h>

/* Host policy shared by Mako and Xedge. Keep provider roots independent
 * of the legacy softTPM key headers and callbacks.
 */
#ifndef BA_ENABLE_SOFTTPM_KEY_MATERIAL
#define BA_ENABLE_SOFTTPM_KEY_MATERIAL (!SHARKSSL_PSA_CONSUMER)
#endif
#if (BA_ENABLE_SOFTTPM_KEY_MATERIAL != 0) && (BA_ENABLE_SOFTTPM_KEY_MATERIAL != 1)
#error BA_ENABLE_SOFTTPM_KEY_MATERIAL must be 0 or 1
#endif


#if SHARKSSL_PSA_CONSUMER && BA_ENABLE_SOFTTPM_KEY_MATERIAL
#error PSA hosts require BA_ENABLE_SOFTTPM_KEY_MATERIAL=0
#endif

#ifndef BA_ENABLE_SHARKTRUST
#define BA_ENABLE_SHARKTRUST 1
#endif
#if (BA_ENABLE_SHARKTRUST != 0) && (BA_ENABLE_SHARKTRUST != 1)
#error BA_ENABLE_SHARKTRUST must be 0 or 1
#endif

#ifndef BA_ENABLE_ZIP_PASSWORD
#define BA_ENABLE_ZIP_PASSWORD 0
#endif
#if (BA_ENABLE_ZIP_PASSWORD != 0) && (BA_ENABLE_ZIP_PASSWORD != 1)
#error BA_ENABLE_ZIP_PASSWORD must be 0 or 1
#endif

#ifndef BA_ENABLE_ZIP_SIGNATURE
#define BA_ENABLE_ZIP_SIGNATURE 0
#endif
#if (BA_ENABLE_ZIP_SIGNATURE != 0) && (BA_ENABLE_ZIP_SIGNATURE != 1)
#error BA_ENABLE_ZIP_SIGNATURE must be 0 or 1
#endif

#ifndef BA_REQUIRE_ZIP_PASSWORD
#define BA_REQUIRE_ZIP_PASSWORD 0
#endif
#if (BA_REQUIRE_ZIP_PASSWORD != 0) && (BA_REQUIRE_ZIP_PASSWORD != 1)
#error BA_REQUIRE_ZIP_PASSWORD must be 0 or 1
#endif


#if BA_REQUIRE_ZIP_PASSWORD && !BA_ENABLE_ZIP_PASSWORD
#error BA_REQUIRE_ZIP_PASSWORD requires BA_ENABLE_ZIP_PASSWORD=1
#endif

#ifndef MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK
#define MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK 1
#endif
#if (MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK != 0) && (MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK != 1)
#error MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK must be 0 or 1
#endif

#ifndef MAKO_USE_CUSTOM_PLATFORM
#define MAKO_USE_CUSTOM_PLATFORM 0
#endif
#if (MAKO_USE_CUSTOM_PLATFORM != 0) && (MAKO_USE_CUSTOM_PLATFORM != 1)
#error MAKO_USE_CUSTOM_PLATFORM must be 0 or 1
#endif


#define MAKO_SQL_NONE 0
#define MAKO_SQL_SQLITE 1
#define MAKO_SQL_ITTIA 2
#ifndef MAKO_SQL_BACKEND
#define MAKO_SQL_BACKEND MAKO_SQL_SQLITE
#endif
#if MAKO_SQL_BACKEND != MAKO_SQL_NONE && MAKO_SQL_BACKEND != MAKO_SQL_SQLITE && MAKO_SQL_BACKEND != MAKO_SQL_ITTIA
#error MAKO_SQL_BACKEND must select NONE, SQLITE or ITTIA
#endif

/* Optional function-name token: void function(lua_State* L).
 * Define MAKO_OPEN_BINDINGS and supply its declaration/implementation in
 * host-owned code. It runs at the regular binding-registration point, including
 * PSA builds; it is independent of makoOpenAUX and key provisioning.
 */

#ifndef MAKO_SOFTTPM_KEY_HEADER
#define MAKO_SOFTTPM_KEY_HEADER "EncryptionKey.h"
#endif

#endif /* _MakoConfig_h */
