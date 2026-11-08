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
 *   $Id: BaConfig.h 6807 2026-11-08 14:20:27Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2023 - 2026
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
 * Shared BAS/BWS configuration defaults and dependency checks.
 * TargConfig.h loads BaConfigOpts.h before port defaults, then includes
 * this header before the configured interfaces are consumed.
 */

/** \file BaConfig.h
 * \brief Shared BAS/BWS build defaults and dependency validation.
 *
 * Configure your product in BaConfigOpts.h or shared compiler definitions.
 * Include TargConfig.h so application overrides and port defaults are loaded
 * in the correct order. The complete guide is maintained in doc/en/C/md/BaConfig.md.
 *
 * \copydoc BaConfigGuide
 */

#ifndef _TargConfig_h
#error Include TargConfig.h to load the port and BAS configuration
#endif

#ifndef _BaConfig_h
#define _BaConfig_h
#include <BaConfigDefs.h>

/* Internal include bookkeeping for BAS/BWS and their Mako/Xedge hosts.
 * Ports load overrides early; also support direct configuration consumers.
 * BA_INTERNAL_CONFIG_OPTS_LOADED is not an application feature switch.
 */
#ifndef BA_INTERNAL_CONFIG_OPTS_LOADED
#define BA_INTERNAL_CONFIG_OPTS_LOADED 1
#include <BaConfigOpts.h>
#endif

#include <BaConfigTypes.h>

/* Port facts and application overrides precede final byte-order validation. */
#include <BaConfigEndian.h>

/* BA_ALLOCATOR_DEFAULT keeps the port's normal allocation hooks. */
/** \def BA_ALLOCATOR
 * Select the port default, dlmalloc, or MQX allocation hooks.
 *
 * Applies to: BAS/BWS port hooks; Mako/Xedge inherit them and have dlmalloc
 * host integration.
 *
 * See \ref baconfig_port for values, defaults, and requirements.
 */
#ifndef BA_ALLOCATOR
#define BA_ALLOCATOR BA_ALLOCATOR_DEFAULT
#endif
#if BA_ALLOCATOR != BA_ALLOCATOR_DEFAULT && BA_ALLOCATOR != BA_ALLOCATOR_DLMALLOC && BA_ALLOCATOR != BA_ALLOCATOR_MQX
#error BA_ALLOCATOR must select DEFAULT, DLMALLOC or MQX
#endif
/** \def BA_THREAD_STACK_SIZE
 * Default stack request for supplied hosts; a positive byte count.
 *
 * Applies to: Mako/Xedge and supplied examples; BAS/BWS constructors take
 * explicit sizes.
 *
 * See \ref baconfig_port for values, defaults, and requirements.
 */
#ifndef BA_THREAD_STACK_SIZE
#define BA_THREAD_STACK_SIZE 24000
#endif
#if BA_THREAD_STACK_SIZE <= 0
#error BA_THREAD_STACK_SIZE must be positive
#endif
/** \def BA_ENABLE_THREAD_NAMES
 * FreeRTOS only: 1 uses source filenames; 0 uses "BAS" (BAS/BWS, Mako, and Xedge).
 */
#ifndef BA_ENABLE_THREAD_NAMES
#define BA_ENABLE_THREAD_NAMES 0
#endif
#if BA_ENABLE_THREAD_NAMES != 0 && BA_ENABLE_THREAD_NAMES != 1
#error BA_ENABLE_THREAD_NAMES must be 0 or 1
#endif
/** \def BA_TIMER_SOURCE
 * Select internal sleep timing or a port-provided timer trigger.
 *
 * Applies to: Native BaTimer in BAS or BWS; inherited by Mako/Xedge when the
 * timer is linked.
 *
 * See \ref baconfig_port for values, defaults, and requirements.
 */
#ifndef BA_TIMER_SOURCE
#define BA_TIMER_SOURCE BA_TIMER_SOURCE_INTERNAL
#endif
#if BA_TIMER_SOURCE != BA_TIMER_SOURCE_INTERNAL && BA_TIMER_SOURCE != BA_TIMER_SOURCE_EXTERNAL
#error BA_TIMER_SOURCE must select INTERNAL or EXTERNAL
#endif

/* Optional native BWS source selection. BAS retains the implementations
 * required by its Lua bindings. These switches are not a promise to remove
 * the corresponding components from BAS or from every generated package.
 */
/** \def BA_ENABLE_AUTHENTICATION
 * Select the full-SDK BWS authenticator sources.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_AUTHENTICATION
#define BA_ENABLE_AUTHENTICATION 1
#endif
#if (BA_ENABLE_AUTHENTICATION != 0) && (BA_ENABLE_AUTHENTICATION != 1)
#error BA_ENABLE_AUTHENTICATION must be 0 or 1
#endif

/** \def BA_ENABLE_JSON_USER_DATABASE
 * Select the BWS JSON user container; requires authentication and JSON.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_JSON_USER_DATABASE
#define BA_ENABLE_JSON_USER_DATABASE 0
#endif
#if (BA_ENABLE_JSON_USER_DATABASE != 0) && (BA_ENABLE_JSON_USER_DATABASE != 1)
#error BA_ENABLE_JSON_USER_DATABASE must be 0 or 1
#endif

/** \def BA_ENABLE_HTTP_THREAD_POOL
 * Select the BWS HTTP command thread pool; configure its size at runtime.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_HTTP_THREAD_POOL
#define BA_ENABLE_HTTP_THREAD_POOL 0
#endif
#if (BA_ENABLE_HTTP_THREAD_POOL != 0) && (BA_ENABLE_HTTP_THREAD_POOL != 1)
#error BA_ENABLE_HTTP_THREAD_POOL must be 0 or 1
#endif

/** \def BA_ENABLE_HTTP_UPLOAD
 * Select BWS upload and supporting asynchronous HTTP sources.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_HTTP_UPLOAD
#define BA_ENABLE_HTTP_UPLOAD 0
#endif
#if (BA_ENABLE_HTTP_UPLOAD != 0) && (BA_ENABLE_HTTP_UPLOAD != 1)
#error BA_ENABLE_HTTP_UPLOAD must be 0 or 1
#endif

/** \def BA_ENABLE_WEBDAV
 * Select BWS WebDAV sources; requires HTTP upload.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_WEBDAV
#define BA_ENABLE_WEBDAV 0
#endif
#if (BA_ENABLE_WEBDAV != 0) && (BA_ENABLE_WEBDAV != 1)
#error BA_ENABLE_WEBDAV must be 0 or 1
#endif

/** \def BA_ENABLE_ZIP_IO
 * Select BWS ZIP-backed I/O sources, independently of decompression.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_ZIP_IO
#define BA_ENABLE_ZIP_IO 0
#endif
#if (BA_ENABLE_ZIP_IO != 0) && (BA_ENABLE_ZIP_IO != 1)
#error BA_ENABLE_ZIP_IO must be 0 or 1
#endif

/** \def BA_ENABLE_JSON
 * Select the native JSON sources in the full-SDK BWS wrapper.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_JSON
#define BA_ENABLE_JSON 0
#endif
#if (BA_ENABLE_JSON != 0) && (BA_ENABLE_JSON != 1)
#error BA_ENABLE_JSON must be 0 or 1
#endif

/** \def BA_ENABLE_WEBSOCKET
 * Select the WebSocket server implementation in the full-SDK BWS wrapper.
 *
 * Applies to: Full-SDK BWS source selection only; not a BAS, Mako, or Xedge
 * feature switch.
 *
 * See \ref baconfig_native for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_WEBSOCKET
#define BA_ENABLE_WEBSOCKET 1
#endif
#if (BA_ENABLE_WEBSOCKET != 0) && (BA_ENABLE_WEBSOCKET != 1)
#error BA_ENABLE_WEBSOCKET must be 0 or 1
#endif

/** \def BA_ENABLE_MDNS
 * Enable the mDNS responder core in builds that select its sources.
 *
 * Applies to: Optional native responder usable with BAS or BWS. Mako and
 * standalone Xedge require BA_ENABLE_MDNS_LUA for their Lua registration.
 *
 * See \ref baconfig_mdns for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_MDNS
#define BA_ENABLE_MDNS 0
#endif
#if (BA_ENABLE_MDNS != 0) && (BA_ENABLE_MDNS != 1)
#error BA_ENABLE_MDNS must be 0 or 1
#endif

/** \def BA_ENABLE_MDNS_LUA
 * Enable mDNS Lua integration; requires the core and BAS Lua support.
 *
 * Applies to: BAS Lua binding and Mako/Xedge registration; not native BWS.
 *
 * See \ref baconfig_mdns for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_MDNS_LUA
#define BA_ENABLE_MDNS_LUA 0
#endif
#if (BA_ENABLE_MDNS_LUA != 0) && (BA_ENABLE_MDNS_LUA != 1)
#error BA_ENABLE_MDNS_LUA must be 0 or 1
#endif

#if BA_ENABLE_WEBDAV && !BA_ENABLE_HTTP_UPLOAD
#error BA_ENABLE_WEBDAV requires BA_ENABLE_HTTP_UPLOAD=1
#endif
#if BA_ENABLE_JSON_USER_DATABASE && (!BA_ENABLE_AUTHENTICATION || !BA_ENABLE_JSON)
#error BA_ENABLE_JSON_USER_DATABASE requires authentication and JSON
#endif
#if BA_ENABLE_MDNS_LUA && !BA_ENABLE_MDNS
#error BA_ENABLE_MDNS_LUA requires BA_ENABLE_MDNS=1
#endif

/** \def MAKO_ENABLE_LUA_MODULE_INTERFACE
 * Enable the Mako module-interface loader path; keep host and SDK consistent.
 *
 * Applies to: Mako integration plus the BAS Lua loader; not BWS or supplied
 * Xedge startup.
 *
 * See \ref baconfig_mako for values, defaults, and requirements.
 */
#ifndef MAKO_ENABLE_LUA_MODULE_INTERFACE
#define MAKO_ENABLE_LUA_MODULE_INTERFACE 0
#endif
#if (MAKO_ENABLE_LUA_MODULE_INTERFACE != 0) && (MAKO_ENABLE_LUA_MODULE_INTERFACE != 1)
#error MAKO_ENABLE_LUA_MODULE_INTERFACE must be 0 or 1
#endif

/** \def BA_ENABLE_SYSLOG
 * Enable the Lua trace logger's operating-system syslog backend.
 *
 * Applies to: BAS Lua trace logger, inherited by Mako/Xedge; not native BWS or
 * Mako daemon logging.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_SYSLOG
#define BA_ENABLE_SYSLOG 0
#endif
#if (BA_ENABLE_SYSLOG != 0) && (BA_ENABLE_SYSLOG != 1)
#error BA_ENABLE_SYSLOG must be 0 or 1
#endif

/* Lua runtime capabilities; port defaults precede this header. */
/** \def BA_HAVE_LOG2
 * Allow Lua to use the C log2 function where its runtime policy permits.
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_HAVE_LOG2
#define BA_HAVE_LOG2 1
#endif
#if BA_HAVE_LOG2 != 0 && BA_HAVE_LOG2 != 1
#error BA_HAVE_LOG2 must be 0 or 1
#endif

/** \def BA_USE_C_RUNTIME_TIME
 * Use C-runtime calendar conversion/formatting in Lua instead of the BAS
 * UTC-only fallback. Current Unix time always comes from baGetUnixTime().
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_USE_C_RUNTIME_TIME
#define BA_USE_C_RUNTIME_TIME 1
#endif
#if BA_USE_C_RUNTIME_TIME != 0 && BA_USE_C_RUNTIME_TIME != 1
#error BA_USE_C_RUNTIME_TIME must be 0 or 1
#endif

/** \def BA_HAVE_STDIO
 * Allow the Lua package loader to probe native files through C stdio.
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_HAVE_STDIO
#define BA_HAVE_STDIO 0
#endif
#if BA_HAVE_STDIO != 0 && BA_HAVE_STDIO != 1
#error BA_HAVE_STDIO must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_STDIO
 * Include Lua FILE/loadfile support; the OS mode also controls the io opener.
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_STDIO
#define BA_ENABLE_LUA_STDIO 1
#endif
#if BA_ENABLE_LUA_STDIO != 0 && BA_ENABLE_LUA_STDIO != 1
#error BA_ENABLE_LUA_STDIO must be 0 or 1
#endif

/** \def BA_LUA_USE_DLOPEN
 * Allow automatic Linux/macOS/iOS Lua loading options; not a global loader ban.
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_LUA_USE_DLOPEN
#define BA_LUA_USE_DLOPEN 1
#endif
#if BA_LUA_USE_DLOPEN != 0 && BA_LUA_USE_DLOPEN != 1
#error BA_LUA_USE_DLOPEN must be 0 or 1
#endif

/** \def BA_LUA_OS_LIBRARY_MODE
 * Select the full or minimal Lua OS-library surface.
 *
 * Applies to: BAS Lua runtime/configuration, inherited by Mako/Xedge; no
 * native BWS consumer.
 *
 * See \ref baconfig_lua_runtime for values, defaults, and requirements.
 */
#ifndef BA_LUA_OS_LIBRARY_MODE
#define BA_LUA_OS_LIBRARY_MODE BA_LUA_OS_LIBRARY_FULL
#endif
#if BA_LUA_OS_LIBRARY_MODE != BA_LUA_OS_LIBRARY_FULL && BA_LUA_OS_LIBRARY_MODE != BA_LUA_OS_LIBRARY_MINIMAL
#error Invalid BA_LUA_OS_LIBRARY_MODE
#endif

/* Shared server, diagnostics, serialization and Lua integration switches. */
/** \def BA_ENABLE_SERVER
 * Select server integration contracts rather than standalone/client contracts.
 *
 * Applies to: BAS/BWS native accept handling and BAS Lua integration;
 * the supplied Mako and standalone Xedge startup code requires value 1.
 *
 * See \ref baconfig_services for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_SERVER
#define BA_ENABLE_SERVER 1
#endif
#if (BA_ENABLE_SERVER != 0) && (BA_ENABLE_SERVER != 1)
#error BA_ENABLE_SERVER must be 0 or 1
#endif

/** \def BA_ENABLE_HTTP_TRACE
 * Enable HttpTrace diagnostics; unrelated to the HTTP TRACE request method.
 *
 * Applies to: Shared BAS/BWS native components; inherited by Mako/Xedge.
 *
 * See \ref baconfig_services for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_HTTP_TRACE
#define BA_ENABLE_HTTP_TRACE 1
#endif
#if (BA_ENABLE_HTTP_TRACE != 0) && (BA_ENABLE_HTTP_TRACE != 1)
#error BA_ENABLE_HTTP_TRACE must be 0 or 1
#endif

/** \def BA_ENABLE_JENCODER_JVAL
 * Enable JEncoder's JVal-tree interface and J format.
 *
 * Applies to: Native encoder in BAS/BWS when linked; inherited by Mako/Xedge.
 *
 * See \ref baconfig_services for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_JENCODER_JVAL
#define BA_ENABLE_JENCODER_JVAL 1
#endif
#if (BA_ENABLE_JENCODER_JVAL != 0) && (BA_ENABLE_JENCODER_JVAL != 1)
#error BA_ENABLE_JENCODER_JVAL must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_DEBUG_LIBRARY
 * Include the standard Lua debug library in the embedded runtime.
 *
 * Applies to: BAS Lua library selection, inherited by Mako/Xedge; not native
 * BWS.
 *
 * See \ref baconfig_lua for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_DEBUG_LIBRARY
#define BA_ENABLE_LUA_DEBUG_LIBRARY 1
#endif
#if (BA_ENABLE_LUA_DEBUG_LIBRARY != 0) && (BA_ENABLE_LUA_DEBUG_LIBRARY != 1)
#error BA_ENABLE_LUA_DEBUG_LIBRARY must be 0 or 1
#endif

/** \def BA_ENABLE_SHARKSSL
 * Select SharkSSL integration, including TLS and associated crypto helpers.
 *
 * Applies to: Shared BAS/BWS native components; inherited by Mako/Xedge.
 * Keep value 1 for the supplied Mako and standalone Xedge startup code.
 *
 * See \ref baconfig_services for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_SHARKSSL
#define BA_ENABLE_SHARKSSL 1
#endif
#if (BA_ENABLE_SHARKSSL != 0) && (BA_ENABLE_SHARKSSL != 1)
#error BA_ENABLE_SHARKSSL must be 0 or 1
#endif

/** \def BA_ENABLE_ZLIB
 * Enable zlib support for ZIP inflation and HTTP compression.
 *
 * Applies to: Shared BAS/BWS native components; inherited by Mako/Xedge.
 *
 * See \ref baconfig_services for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_ZLIB
#define BA_ENABLE_ZLIB 1
#endif
#if (BA_ENABLE_ZLIB != 0) && (BA_ENABLE_ZLIB != 1)
#error BA_ENABLE_ZLIB must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_SOCKET_COMPAT
 * Register BAS's LuaSocket compatibility surface alongside ba.socket.
 *
 * Applies to: BAS Lua socket binding, installed by Mako/Xedge; not native BWS.
 *
 * See \ref baconfig_lua for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_SOCKET_COMPAT
#define BA_ENABLE_LUA_SOCKET_COMPAT 1
#endif
#if (BA_ENABLE_LUA_SOCKET_COMPAT != 0) && (BA_ENABLE_LUA_SOCKET_COMPAT != 1)
#error BA_ENABLE_LUA_SOCKET_COMPAT must be 0 or 1
#endif

/* Set BA_ENABLE_EMBEDDED_LUA=0 to link the Lua core separately.
 * BAS Lua bindings remain selected independently. */
/** \def BA_ENABLE_EMBEDDED_LUA
 * Include the Lua core in BAS; zero requires a separately linked compatible core.
 *
 * Applies to: BAS wrapper source selection, including Mako/Xedge BAS builds;
 * not native BWS.
 *
 * See \ref baconfig_lua for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_EMBEDDED_LUA
#define BA_ENABLE_EMBEDDED_LUA 1
#endif
#if (BA_ENABLE_EMBEDDED_LUA != 0) && (BA_ENABLE_EMBEDDED_LUA != 1)
#error BA_ENABLE_EMBEDDED_LUA must be 0 or 1
#endif

/* Port-selected dispatcher and listener policy. */
/** \def BA_USE_POLL
 * Select the POSIX poll dispatcher in a supporting build and socket port.
 * Defaults to 0; the Mako QNX makefile defaults to 1. Use the poll HttpCfg.h
 * for every translation unit and rebuild when changing dispatchers.
 */
#ifndef BA_USE_POLL
#define BA_USE_POLL 0
#endif
#if (BA_USE_POLL != 0) && (BA_USE_POLL != 1)
#error BA_USE_POLL must be 0 or 1
#endif

/** \def BA_USE_EPOLL
 * Select the epoll dispatcher in a supporting build and socket port.
 *
 * Applies to: Shared full-SDK BAS/BWS port wrapper; Mako/Xedge inherit the
 * selected dispatcher.
 *
 * See \ref baconfig_port for values, defaults, and requirements.
 */
#ifndef BA_USE_EPOLL
#define BA_USE_EPOLL 0
#endif
#if (BA_USE_EPOLL != 0) && (BA_USE_EPOLL != 1)
#error BA_USE_EPOLL must be 0 or 1
#endif
#if BA_USE_POLL && BA_USE_EPOLL
#error Select only one of BA_USE_POLL and BA_USE_EPOLL
#endif

/** \def BA_ENABLE_SOCKET_REUSE_ADDRESS
 * Request address reuse where the selected socket port implements it.
 *
 * Applies to: Shared BAS/BWS BSD-style socket port; inherited by Mako/Xedge.
 *
 * See \ref baconfig_port for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_SOCKET_REUSE_ADDRESS
#define BA_ENABLE_SOCKET_REUSE_ADDRESS 0
#endif
#if (BA_ENABLE_SOCKET_REUSE_ADDRESS != 0) && (BA_ENABLE_SOCKET_REUSE_ADDRESS != 1)
#error BA_ENABLE_SOCKET_REUSE_ADDRESS must be 0 or 1
#endif

/* Optional modules. Ports may select defaults before including this header.
 * Set the same integer 0/1 values for BAS and all application sources. */
/* Include the AUX API by default:
 * https://realtimelogic.com/ba/doc/?url=auxlua.html
 */
/** \def BA_ENABLE_AUX_API
 * Select the grouped BAS auxiliary source modules.
 *
 * Applies to: BAS wrapper source selection; supplied Mako and standalone
 * Xedge startup requires these APIs. Native BWS can use separately compiled
 * native components from this group without this BAS selector.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_AUX_API
#define BA_ENABLE_AUX_API 1
#endif
#if (BA_ENABLE_AUX_API != 0) && (BA_ENABLE_AUX_API != 1)
#error BA_ENABLE_AUX_API must be 0 or 1
#endif

/** \def BA_ENABLE_TLS_TOOLS
 * Enable SharkSSL binary certificate/CA-list data export in Lua.
 *
 * Applies to: BAS SharkSSL Lua binding, inherited by Mako/Xedge; not native
 * BWS.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_TLS_TOOLS
#define BA_ENABLE_TLS_TOOLS 0
#endif
#if (BA_ENABLE_TLS_TOOLS != 0) && (BA_ENABLE_TLS_TOOLS != 1)
#error BA_ENABLE_TLS_TOOLS must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_CBOR
 * Register CBOR during Mako or standalone Xedge startup; requires LPeg.
 *
 * Applies to: Mako Server and standalone Xedge. Both use this option;
 * BAS and BWS libraries do not register the module.
 *
 * Supply the module implementation and Lua resources. Custom BAS applications
 * must link and register the module themselves.
 * See \ref baconfig_host_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_CBOR
#define BA_ENABLE_LUA_CBOR 0
#endif
#if (BA_ENABLE_LUA_CBOR != 0) && (BA_ENABLE_LUA_CBOR != 1)
#error BA_ENABLE_LUA_CBOR must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_DEBUGGER
 * Select remote-debugger integration, distinct from Lua's debug library.
 *
 * Applies to: BAS debugger sources/hooks and Mako/Xedge startup.
 * The shared lwIP port also checks SO_REUSE; BWS gains no Lua debugger.
 *
 * See \ref baconfig_lua for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_DEBUGGER
#define BA_ENABLE_LUA_DEBUGGER 0
#endif
#if (BA_ENABLE_LUA_DEBUGGER != 0) && (BA_ENABLE_LUA_DEBUGGER != 1)
#error BA_ENABLE_LUA_DEBUGGER must be 0 or 1
#endif

/** \def BA_ENABLE_EMBEDDED_CA
 * Make the embedded CA list available to the SharkSSL Lua binding.
 *
 * Applies to: BAS SharkSSL Lua binding; inherited by Mako and required by
 * standalone Xedge. Not BWS.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_EMBEDDED_CA
#define BA_ENABLE_EMBEDDED_CA 0
#endif
#if (BA_ENABLE_EMBEDDED_CA != 0) && (BA_ENABLE_EMBEDDED_CA != 1)
#error BA_ENABLE_EMBEDDED_CA must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_FORKPTY
 * Include the supported native fork/pseudoterminal integration.
 *
 * Applies to: BAS source selection and Mako registration; no supplied Xedge
 * registration or BWS consumer.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_FORKPTY
#define BA_ENABLE_LUA_FORKPTY 0
#endif
#if (BA_ENABLE_LUA_FORKPTY != 0) && (BA_ENABLE_LUA_FORKPTY != 1)
#error BA_ENABLE_LUA_FORKPTY must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_LPEG
 * Register LPeg during Mako or standalone Xedge startup.
 *
 * Applies to: Mako Server and standalone Xedge. Both use this option;
 * BAS and BWS libraries do not register the module.
 *
 * Supply the LPeg implementation. Custom BAS applications must link and
 * register LPeg themselves.
 * See \ref baconfig_host_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_LPEG
#define BA_ENABLE_LUA_LPEG 0
#endif
#if (BA_ENABLE_LUA_LPEG != 0) && (BA_ENABLE_LUA_LPEG != 1)
#error BA_ENABLE_LUA_LPEG must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_PIPE
 * Include pipe facilities in the BAS Lua socket binding on supported ports.
 *
 * Applies to: BAS Lua socket binding, installed by Mako/Xedge; not native BWS.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_PIPE
#define BA_ENABLE_LUA_PIPE 0
#endif
#if (BA_ENABLE_LUA_PIPE != 0) && (BA_ENABLE_LUA_PIPE != 1)
#error BA_ENABLE_LUA_PIPE must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_OPCUA
 * Include OPC UA native sources in the BAS auxiliary group.
 *
 * Applies to: BAS source selection and Mako/Xedge registration; not the native
 * BWS wrapper.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_OPCUA
#define BA_ENABLE_LUA_OPCUA 0
#endif
#if (BA_ENABLE_LUA_OPCUA != 0) && (BA_ENABLE_LUA_OPCUA != 1)
#error BA_ENABLE_LUA_OPCUA must be 0 or 1
#endif

/** \def BA_ENABLE_LUA_PROTOBUF
 * Register Protocol Buffers during Mako or standalone Xedge startup.
 *
 * Applies to: Mako Server and standalone Xedge. Both use this option;
 * BAS and BWS libraries do not register the module.
 *
 * Supply the module implementation. Custom BAS applications must link and
 * register it themselves.
 * See \ref baconfig_host_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_LUA_PROTOBUF
#define BA_ENABLE_LUA_PROTOBUF 0
#endif
#if (BA_ENABLE_LUA_PROTOBUF != 0) && (BA_ENABLE_LUA_PROTOBUF != 1)
#error BA_ENABLE_LUA_PROTOBUF must be 0 or 1
#endif

/** \def BA_ENABLE_REVERSE_PROXY
 * Include the HTTP reverse-proxy implementation.
 *
 * Applies to: BAS source selection and Mako registration; no supplied Xedge
 * registration or BWS selector.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_REVERSE_PROXY
#define BA_ENABLE_REVERSE_PROXY 0
#endif
#if (BA_ENABLE_REVERSE_PROXY != 0) && (BA_ENABLE_REVERSE_PROXY != 1)
#error BA_ENABLE_REVERSE_PROXY must be 0 or 1
#endif

/** \def BA_ENABLE_REVERSE_CONNECTION
 * Include Lua reverse connections within the auxiliary SharkSSL group.
 *
 * Applies to: BAS source selection and Mako/Xedge registration; not the native
 * BWS wrapper.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_REVERSE_CONNECTION
#define BA_ENABLE_REVERSE_CONNECTION 0
#endif
#if (BA_ENABLE_REVERSE_CONNECTION != 0) && (BA_ENABLE_REVERSE_CONNECTION != 1)
#error BA_ENABLE_REVERSE_CONNECTION must be 0 or 1
#endif

/** \def BA_ENABLE_UBJSON
 * Include UBJSON native and Lua sources within the auxiliary group.
 *
 * Applies to: BAS source selection and Mako/Xedge registration; not the native
 * BWS wrapper.
 *
 * See \ref baconfig_modules for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_UBJSON
#define BA_ENABLE_UBJSON 0
#endif
#if (BA_ENABLE_UBJSON != 0) && (BA_ENABLE_UBJSON != 1)
#error BA_ENABLE_UBJSON must be 0 or 1
#endif

/** \def BA_ENABLE_TPM
 * Set to 1 when using Lua TPM support to configure its crypto dependencies.
 *
 * Applies to: BAS Lua TPM configurations, including Mako and standalone Xedge.
 * Set consistently in BAS and the application. BaConfig.h supplies required
 * crypto defaults and checks for conflicting settings. TPM initialization is
 * configured separately; value 0 does not disable TPM support. Native BWS has
 * no Lua TPM API.
 *
 * See \ref baconfig_crypto for values, defaults, and requirements.
 */
#ifndef BA_ENABLE_TPM
#define BA_ENABLE_TPM 0
#endif
#if (BA_ENABLE_TPM != 0) && (BA_ENABLE_TPM != 1)
#error BA_ENABLE_TPM must be 0 or 1
#endif

/* Crypto scope: these SHARKSSL_* defaults are seen by both BAS and BWS
 * through TargConfig.h; Mako/Xedge inherit the BAS configuration. Their
 * effects depend on the linked SharkSSL implementation and selected provider.
 * BA_ENABLE_TPM only supplies/checks dependencies here; it does not select
 * a source module or control host TPM initialization.
 * See doc/en/C/md/BaConfig.md for the per-option defaults and scope.
 * The PSA consumer supplies these operations through its provider.
 */
#if SHARKSSL_PSA_CONSUMER
#ifndef SHARKSSL_PSA_CONSUMER_STANDALONE_AES
#define SHARKSSL_PSA_CONSUMER_STANDALONE_AES 1
#elif BA_ENABLE_TPM && (SHARKSSL_PSA_CONSUMER_STANDALONE_AES != 1)
#error BA_ENABLE_TPM requires SHARKSSL_PSA_CONSUMER_STANDALONE_AES=1 with PSA
#endif
#ifndef SHARKSSL_ENABLE_RSA_API
#define SHARKSSL_ENABLE_RSA_API 0
#endif
#ifndef SHARKSSL_ENABLE_RSA_OAEP
#define SHARKSSL_ENABLE_RSA_OAEP 0
#endif
#ifndef SHARKSSL_ENABLE_DHE_RSA
#define SHARKSSL_ENABLE_DHE_RSA 0
#endif
#ifndef SHARKSSL_ENABLE_ECDSA_API
#define SHARKSSL_ENABLE_ECDSA_API 0
#endif
#ifndef SHARKSSL_USE_RNG_FORTUNA
#define SHARKSSL_USE_RNG_FORTUNA 0
#endif
#ifndef SHARKSSL_USE_RNG_TINYMT
#define SHARKSSL_USE_RNG_TINYMT 0
#endif
#ifndef SHARKSSL_TLS_1_3_INITIAL_KEY_SHARE_X25519_ONLY
#define SHARKSSL_TLS_1_3_INITIAL_KEY_SHARE_X25519_ONLY 1
#endif
#endif

#ifndef SHARKSSL_ENABLE_ASN1_KEY_CREATION
#define SHARKSSL_ENABLE_ASN1_KEY_CREATION                1
#elif BA_ENABLE_TPM && (SHARKSSL_ENABLE_ASN1_KEY_CREATION != 1)
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_ASN1_KEY_CREATION=1
#endif
#ifndef SHARKSSL_ENABLE_RSAKEY_CREATE
#define SHARKSSL_ENABLE_RSAKEY_CREATE                    (!SHARKSSL_PSA_CONSUMER)
#endif
#ifndef SHARKSSL_ENABLE_ECCKEY_CREATE
#define SHARKSSL_ENABLE_ECCKEY_CREATE                    (!SHARKSSL_PSA_CONSUMER)
#endif
#ifndef SHARKSSL_ENABLE_CSR_CREATION
#define SHARKSSL_ENABLE_CSR_CREATION                     1
#elif BA_ENABLE_TPM && (SHARKSSL_ENABLE_CSR_CREATION != 1)
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_CSR_CREATION=1
#endif
#ifndef SHARKSSL_ENABLE_CSR_SIGNING
#define SHARKSSL_ENABLE_CSR_SIGNING                      1
#elif BA_ENABLE_TPM && (SHARKSSL_ENABLE_CSR_SIGNING != 1)
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_CSR_SIGNING=1
#endif

/* Preserve the SHA-256 derivation fallback when SHA-512 is explicitly off. */
#ifndef SHARKSSL_USE_SHA_512
#define SHARKSSL_USE_SHA_512 1
#endif

#ifndef SHARKSSL_ECC_USE_SECP256R1
#define SHARKSSL_ECC_USE_SECP256R1 1
#elif BA_ENABLE_TPM && (SHARKSSL_ECC_USE_SECP256R1 != 1)
#error BA_ENABLE_TPM requires SHARKSSL_ECC_USE_SECP256R1=1
#endif
#ifndef SHARKSSL_ECC_USE_SECP384R1
#define SHARKSSL_ECC_USE_SECP384R1 1
#elif BA_ENABLE_TPM && (SHARKSSL_ECC_USE_SECP384R1 != 1)
#error BA_ENABLE_TPM requires SHARKSSL_ECC_USE_SECP384R1=1
#endif
#ifndef SHARKSSL_ECC_USE_SECP521R1
#define SHARKSSL_ECC_USE_SECP521R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP256R1
#define SHARKSSL_ECC_USE_BRAINPOOLP256R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP384R1
#define SHARKSSL_ECC_USE_BRAINPOOLP384R1 1
#endif
#ifndef SHARKSSL_ECC_USE_BRAINPOOLP512R1
#define SHARKSSL_ECC_USE_BRAINPOOLP512R1 1
#endif

#ifndef SHARKSSL_ENABLE_RSA_OAEP
#define SHARKSSL_ENABLE_RSA_OAEP 1
#endif

#if BA_ENABLE_TPM
#if !BA_ENABLE_SHARKSSL
#error BA_ENABLE_TPM requires SharkSSL
#endif

#ifndef SHARKSSL_ENABLE_PEM_API
#define SHARKSSL_ENABLE_PEM_API 1
#elif SHARKSSL_ENABLE_PEM_API != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_PEM_API=1
#endif
#ifndef SHARKSSL_USE_ECC
#define SHARKSSL_USE_ECC 1
#elif SHARKSSL_USE_ECC != 1
#error BA_ENABLE_TPM requires SHARKSSL_USE_ECC=1
#endif
#ifndef SHARKSSL_ENABLE_ECDSA
#define SHARKSSL_ENABLE_ECDSA 1
#elif SHARKSSL_ENABLE_ECDSA != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_ECDSA=1
#endif
#ifndef SHARKSSL_ENABLE_ECDHE_ECDSA
#define SHARKSSL_ENABLE_ECDHE_ECDSA 1
#elif SHARKSSL_ENABLE_ECDHE_ECDSA != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_ECDHE_ECDSA=1
#endif
#ifndef SHARKSSL_USE_SHA_256
#define SHARKSSL_USE_SHA_256 1
#elif SHARKSSL_USE_SHA_256 != 1
#error BA_ENABLE_TPM requires SHARKSSL_USE_SHA_256=1
#endif
#ifndef SHARKSSL_USE_SHA_384
#define SHARKSSL_USE_SHA_384 1
#elif SHARKSSL_USE_SHA_384 != 1
#error BA_ENABLE_TPM requires SHARKSSL_USE_SHA_384=1
#endif
#ifndef SHARKSSL_USE_AES_128
#define SHARKSSL_USE_AES_128 1
#elif SHARKSSL_USE_AES_128 != 1
#error BA_ENABLE_TPM requires SHARKSSL_USE_AES_128=1
#endif
#ifndef SHARKSSL_USE_AES_256
#define SHARKSSL_USE_AES_256 1
#elif SHARKSSL_USE_AES_256 != 1
#error BA_ENABLE_TPM requires SHARKSSL_USE_AES_256=1
#endif
#ifndef SHARKSSL_ENABLE_AES_GCM
#define SHARKSSL_ENABLE_AES_GCM 1
#elif SHARKSSL_ENABLE_AES_GCM != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_AES_GCM=1
#endif

#if SHARKSSL_PSA_CONSUMER
#ifndef SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY
#define SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY 0
#elif SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY != 0
#error BA_ENABLE_TPM requires SHARKSSL_PSA_CONSUMER_STRICT_IDENTITY=0
#endif
#if SHARKSSL_ENABLE_RSAKEY_CREATE || SHARKSSL_ENABLE_ECCKEY_CREATE
#error BA_ENABLE_TPM with PSA requires native RSA/ECC key generation disabled
#endif
#else
#ifndef SHARKSSL_ENABLE_ECDSA_API
#define SHARKSSL_ENABLE_ECDSA_API 1
#elif SHARKSSL_ENABLE_ECDSA_API != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_ECDSA_API=1 with softTPM
#endif
#if SHARKSSL_ENABLE_ECCKEY_CREATE != 1
#error BA_ENABLE_TPM requires SHARKSSL_ENABLE_ECCKEY_CREATE=1 with softTPM
#endif
#endif
#endif /* BA_ENABLE_TPM */

/* Fixed BAS crypto requirements, also visible to native BWS/SharkSSL.
 * SHARKSSL_HMAC_API is consumed by the BAS Lua crypto binding only;
 * Mako/Xedge inherit that behavior. It is not a native BWS HMAC selector.
 */
#if SHARKSSL_PSA_CONSUMER
/* BAS supplies legacy SHA-1 and MD5 outside the PSA provider. */
#define SHARKSSL_USE_SHA1       0
#else
#define SHARKSSL_USE_SHA1       1
#define SHARKSSL_USE_MD5        1
#endif
#define SHARKSSL_ENABLE_AES_CCM 1
#define SHARKSSL_HMAC_API       1
#define SHARKSSL_ENABLE_AES_CBC 1
#define SHARKSSL_ENABLE_ENCRYPTED_PKCS8_SUPPORT 1

#endif /* _BaConfig_h */
