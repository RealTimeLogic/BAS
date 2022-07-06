/*
 *     ____             _________                __                _     
 *    / __ \___  ____ _/ /_  __(_)___ ___  ___  / /   ____  ____ _(_)____
 *   / /_/ / _ \/ __ `/ / / / / / __ `__ \/ _ \/ /   / __ \/ __ `/ / ___/
 *  / _, _/  __/ /_/ / / / / / / / / / / /  __/ /___/ /_/ / /_/ / / /__  
 * /_/ |_|\___/\__,_/_/ /_/ /_/_/ /_/ /_/\___/_____/\____/\__, /_/\___/  
 *                                                       /____/          
 *
 *                  Barracuda Application Server
 *
 ****************************************************************************
 *			      HEADER
 *
 *   $Id: lxrc.h 5204 2022-07-06 06:54:33Z wini $
 *
 *   COPYRIGHT:  Real Time Logic, 2022
 *               https://realtimelogic.com
 *
 *   The copyright to the program herein is the property of
 *   Real Time Logic. The program may be used or copied only
 *   with the written permission from Real Time Logic or
 *   in accordance with the terms and conditions stipulated in
 *   the agreement under which the program has been supplied.
 ****************************************************************************
 *
 *
 */
#ifndef __lxrc_h
#define __lxrc_h 

#ifndef NO_SHARKSSL
#include <SharkSSL.h>
#endif
#include <balua.h>
#include <SoDispCon.h>

#ifdef __cplusplus
extern "C" {
#endif

void balua_forkpty(lua_State* L);
void balua_http(lua_State* L);
void balua_luaio(lua_State *L);
void balua_sharkssl(lua_State *L);
void balua_socket(lua_State* L);
void balua_relsocket(lua_State* L);
void balua_crypto(lua_State *L);
void balua_tracelogger(lua_State *L);

typedef void (*balua_thread_Shutdown)(lua_State* L, ThreadMutex* dispMutex);
balua_thread_Shutdown balua_thread(lua_State* L);

#ifdef __cplusplus
}
#endif

/************************  Private functions ***************************/

int lGetStdSockOptions(lua_State* L, int tabIx, const char** intf,
                       BaBool* ipv6, BaBool* secure);
#ifndef NO_SHARKSSL
void lsharkssl_unlock(lua_State *L,SharkSsl* super);
SharkSsl* lsharkssl_lock(
   lua_State *L,int tabIx,SharkSsl_Role role,SharkSsl* lockedShark);
#endif
int pushCertificate(lua_State *L, SoDispCon* con);
int pushCiphers(lua_State *L, SoDispCon* con);
int calcTabSize(lua_State* L, int ix); /* lhttp.c */
struct HttpClientKeyVal;
char* extractTab(const char* tab, lua_State* L, int ix,
                 char* ud, struct HttpClientKeyVal** pkv); /* lhttp.c */

#define HTTPCLIENT "HTTPCLIENT"
struct HttpClient;
#define toHttpClient(L,ix) (struct HttpClient*)luaL_checkudata(L,ix,HTTPCLIENT)

#define BACERTSTORE "BACERTSTORE"
/* tocs: To SharkSslCertStore -- Certificate Authority
 */
#define toCertStore(L,ix) (SharkSslCertStore*)luaL_checkudata(L,ix,BACERTSTORE) 


#endif
