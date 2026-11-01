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
/* Standalone Xedge host options; include before barracuda.h. */
#ifndef _XedgeConfig_h
#define _XedgeConfig_h
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

#ifndef XEDGE_ENABLE_DISK_IO
#define XEDGE_ENABLE_DISK_IO 1
#endif
#if (XEDGE_ENABLE_DISK_IO != 0) && (XEDGE_ENABLE_DISK_IO != 1)
#error XEDGE_ENABLE_DISK_IO must be 0 or 1
#endif

#ifndef XEDGE_ENABLE_AUX
#define XEDGE_ENABLE_AUX 1
#endif
#if (XEDGE_ENABLE_AUX != 0) && (XEDGE_ENABLE_AUX != 1)
#error XEDGE_ENABLE_AUX must be 0 or 1
#endif

#ifndef XEDGE_ENABLE_LUA_LINE_TRACE
#define XEDGE_ENABLE_LUA_LINE_TRACE 0
#endif
#if (XEDGE_ENABLE_LUA_LINE_TRACE != 0) && (XEDGE_ENABLE_LUA_LINE_TRACE != 1)
#error XEDGE_ENABLE_LUA_LINE_TRACE must be 0 or 1
#endif


#ifndef XEDGE_USE_DEFAULT_DISK_IO_INIT
#if defined(_WIN32) || defined(BA_POSIX)
#define XEDGE_USE_DEFAULT_DISK_IO_INIT 1
#else
#define XEDGE_USE_DEFAULT_DISK_IO_INIT 0
#endif
#endif
#if XEDGE_USE_DEFAULT_DISK_IO_INIT != 0 && XEDGE_USE_DEFAULT_DISK_IO_INIT != 1
#error XEDGE_USE_DEFAULT_DISK_IO_INIT must be 0 or 1
#endif
#if XEDGE_ENABLE_DISK_IO && XEDGE_USE_DEFAULT_DISK_IO_INIT && !defined(_WIN32) && !defined(BA_POSIX)
#error Supply xedgeInitDiskIo and set XEDGE_USE_DEFAULT_DISK_IO_INIT=0 on this target
#endif

/* Separate HTTP/Lua pools, or one fixed shared Lua/HTTP pool. */
#define XEDGE_THREAD_POOL_SEPARATE 0
#define XEDGE_THREAD_POOL_SHARED_FIXED 1
#ifndef XEDGE_THREAD_POOL_MODE
#define XEDGE_THREAD_POOL_MODE XEDGE_THREAD_POOL_SEPARATE
#endif
#if XEDGE_THREAD_POOL_MODE != XEDGE_THREAD_POOL_SEPARATE && XEDGE_THREAD_POOL_MODE != XEDGE_THREAD_POOL_SHARED_FIXED
#error Invalid XEDGE_THREAD_POOL_MODE
#endif
#ifndef XEDGE_THREAD_COUNT
#define XEDGE_THREAD_COUNT 2
#endif
#if XEDGE_THREAD_COUNT < 1 || XEDGE_THREAD_COUNT > 65535
#error XEDGE_THREAD_COUNT must be between 1 and 65535
#endif

/* An explicit resource directory selects disk-backed VM resources. This is
 * independent of XEDGE_ENABLE_DISK_IO, which controls the application disk IO.
 */
#ifdef XEDGE_RESOURCE_DIRECTORY
#ifndef BAIO_DISK
#define BAIO_DISK
#endif
#else
#define XEDGE_RESOURCE_DIRECTORY "../XedgeResources"
#endif

#ifndef XEDGE_SOFTTPM_KEY_HEADER
#define XEDGE_SOFTTPM_KEY_HEADER "EncryptionKey.h"
#endif

#endif /* _XedgeConfig_h */
