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
 *   $Id: balua.h 5134 2022-04-27 22:40:57Z wini $
 *
 *   COPYRIGHT:  Real Time Logic LLC, 2008 - 2022
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

 
#ifndef _balua_h
#define _balua_h

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "SharkSSL.h"

#ifdef __cplusplus
}
#endif 

#include "AuthenticatedUser.h"
#include "ThreadLib.h"

/** @defgroup LSP The Lua Server Pages (LSP) Plugin.
    @ingroup Plugins

   balua.h is header file that provides the *public* symbols for creating,
   destroying and configuring an LSP VM.

   Creating and initializing A Barracuda Lua VM consists of calling
   balua_create() and calling balua_loadconfig().

   See the C startup source code for the two following examples for
   more information on how to initialize the LSP plugin:

   \li <a href="../../../../../examples/lspappmgr/readme.html">
   LSP Application Manager</a>
   \li <a href="../../../../../examples/wfs/readme.html">
   Web File Server</a>
   @{
*/

#ifdef __cplusplus
extern "C" {
#endif

#define BALUA_VERSION 20
#define BA_ENV_IX lua_upvalueindex(1)
#define BA_TABLE "_BA_"
#define BA_WKTABLE "_BAWK"
/** Creates the Barracuda Lua VM */
#define balua_create(p) _balua_create(p, BALUA_VERSION)

#define balua_optboolean(L,narg,def) luaL_opt(L, balua_checkboolean, narg, def)
#define baluaENV_getmetatable(L,mtId) lua_rawgeti(L, BA_ENV_IX, mtId)
#define baluaENV_checkudata(L,ud,mtid) _baluaENV_isudtype(L,ud,mtid,TRUE)
#define baluaENV_isudtype(L,ud,mtid) _baluaENV_isudtype(L,ud,mtid,FALSE)
#define balua_pushbatab(L) lua_getfield(L,LUA_REGISTRYINDEX,BA_TABLE)
#define balua_pushwktab(L) lua_getfield(L,LUA_REGISTRYINDEX,BA_WKTABLE)
#define balua_newlib(L,l) \
  (luaL_newlibtable(L,l), balua_pushbatab(L), luaL_setfuncs(L,l,1))
#define baluaENV_getmutex(L) baluaENV_getparam(L)->mutex
/** Get the SoDisp mutex */
#define balua_getmutex(L) balua_getparam(L)->mutex
#define GET_BAMUTEX ThreadMutex* m = baluaENV_getmutex(L)
/** Release mutex 'm' */
#define balua_releasemutex(m) if(m) ThreadMutex_release(m)
/** Set mutex 'm' */
#define balua_setmutex(m) if(m) ThreadMutex_set(m)

#ifdef NDEBUG
#define balua_check(x,y) x
#else
#define balua_check(x,y) baAssert(y == x)
#endif


#define dmpstk1(L,x) \
   fprintf(stderr,"%d: %s %p\n", __LINE__, lua_typename(L,lua_type(L,(x))), \
           lua_topointer(L,x))
#define dmpstk2(L,x) {                          \
   int ix=lua_absindex(L,x);                                         \
   fprintf(stderr,"%4d: %2d %s\n", __LINE__, ix, luaL_tolstring(L,x,0)); \
   lua_pop(L,1);                                                        \
}while(0)
#define dmptop(L) \
   fprintf(stderr,"%d: top %d\n",__LINE__,lua_gettop(L))
#define dmpTab(L, ix) do {                                        \
      fprintf(stderr,"%d: TAB %d %p\n",\
               __LINE__, lua_absindex(L,ix), lua_topointer(L,ix)); \
   lua_pushnil(L);                                              \
   while(lua_next(L, ix < 0 ? ix-1: ix) != 0)                   \
   {                                                             \
      fprintf(stderr,"\t%s = %s\n",                                     \
              luaL_tolstring(L,-3,0),                                   \
              luaL_tolstring(L,-1,0));                                  \
      lua_pop(L,3);                                                     \
   }                                                                    \
} while(0)


struct BaTimer;
struct LoginTracker;

/** The startup and runtime parameters for a Barracuda Server Lua VM.
*/
typedef struct
{
      lua_State* L; /**< The lua universe */
      HttpServer* server; /**< Pointer to server for this vm */
      struct BaTimer* timer; /**< Timer bindings activated if not NULL. */
      IoIntf* vmio; /**< The required VM (Lua) IO */
      ThreadMutex* mutex; /**<The mutex used by the server's SoDisp */
      struct LoginTracker* tracker; /**< The optional tracker */
      int errHndRef; /**< Internal: The ba.seterrh(func) ref */
} BaLua_param;


typedef struct
{
      IoIntf*  i;
      int destroy;
} baio_t;


#ifdef BA_LEAK_CHECK
BA_API void* baLMallocL(lua_State* L, size_t size,const char* file, int line);
#define baLMalloc(L,size) baLMallocL(L,size,__FILE__,__LINE__)
#else
/** Same as baMalloc, but does an emergency GC if baMalloc returns NULL. */
BA_API void* baLMalloc(lua_State* L, size_t size);
#endif

/** 
    Creates the Barracuda Lua VM; Note: use macro balua_create(BaLua_param).
*/
BA_API lua_State* _balua_create(const BaLua_param* p, int version);

/** 
    Install Lua bindings for the default login tracker. See one of the two following C code examples for how to use this function: MakoMain.c or LspAppMgr.c
    \param noOfLoginTrackerNodes tracker size
    \param maxNumberOfLogins how many login attempts before user (IP address) is banned
    \param banTime how long in seconds to ban an IP address
    \sa #LoginTracker #LoginTrackerIntf
*/
BA_API int balua_usertracker_create(
   lua_State* L, U32 noOfLoginTrackerNodes, U32 maxNumberOfLogins,
   BaTime banTime);


BA_API lua_State* balua_getmainthread(lua_State* L);
BA_API int balua_typeerror(lua_State *L, int narg, const char *tname);
BA_API int balua_checkboolean(lua_State *L, int narg);
BA_API int baluaENV_newmetatable(lua_State *L, int mtid, int inheritmtid);
BA_API void baluaENV_register(
   lua_State *L, int mtid, int inheritmtid, const luaL_Reg* lib);
BA_API void* baluaENV_newuserdata(lua_State *L, int mtid, size_t size);
BA_API void* _baluaENV_isudtype(lua_State* L, int udIx, int mtid, int check);
BA_API HttpCommand* baluaENV_checkcmd(lua_State* L, int ix);
BA_API BaLua_param* balua_getparam(lua_State* L);
BA_API BaLua_param* baluaENV_getparam(lua_State* L);
BA_API int balua_errorhandler(lua_State* L);
BA_API void balua_manageerr(
   lua_State* L,const char* ewhere,const char* emsg,HttpCommand* cmd);
BA_API void balua_resumeerr(lua_State* L,const char* ewhere);
BA_API int balua_loadfile(
	lua_State *L, const char *filename, struct IoIntf* io, int envix);

#ifdef _DOXYGEN
/** Load a Lua script and run the script at startup. You may call this
    function multiple times during startup.
    \param L the Lua state.
    \param io the I/O interface to use when loading script 'filename'.
    \param filename the path+name of the Lua script. The name defaults
    to .config if this parameter is NULL.
 */
BA_API int balua_loadconfig(
   lua_State* L, struct IoIntf* io, const char* filename);
#else
#define balua_loadconfig(L, io, filename) balua_loadconfigExt(L, io, filename, 0)
#endif

/** Load a Lua script and run the script at startup. You may call this
    function multiple times during startup.
    This function is extends balua_loadconfig and enables you to get return
    values from the script.
    \param L the Lua state.
    \param io the I/O interface to use when loading script 'filename'.
    \param filename the path+name of the Lua script. The name defaults
    to .config if this parameter is NULL.
    \param nresults value passed to lua_pcall
 */
BA_API int balua_loadconfigExt(
   lua_State* L, struct IoIntf* io, const char* filename, int nresults);


BA_API IoIntf* baluaENV_checkIoIntf(lua_State *L, int udIx);

/** Register an IoIntf instance with the Lua VM. You may install any
    number of I/O interfaces, but each I/O interface must be
    registered with a unique name.

    \param L the Lua state.
    \param name the name makes it possible to fetch the I/O using the
    Lua function ba.openio(name).
    \param newio the IoIntf instance, such as DiskIo, ZipIo, NetIo, or
    your own implementation.
 */
BA_API IoIntf* balua_iointf(
   lua_State* L, const char* name, struct IoIntf* newio);
BA_API struct IoIntf** balua_createiointf(lua_State* L);
BA_API HttpDir* baluaENV_toDir(lua_State* L, int ix);
BA_API HttpDir* baluaENV_createDir(lua_State* L,int mtId,size_t dirSize);
BA_API const char* balua_getStringField(
   lua_State *L, int ix, const char *k, const char *def);
BA_API lua_Integer balua_getIntField(
   lua_State *L, int ix, const char *k, lua_Integer defVal);
BA_API BaBool balua_getBoolField(
   lua_State *L, int ix, const char *k, BaBool defVal);
BA_API int balua_getTabField(lua_State *L, int ix, const char *k);
BA_API const char* balua_checkStringField(lua_State *L, int ix, const char *k);
BA_API lua_Integer balua_checkIntField(lua_State *L, int ix, const char *k);


/* Internally used by the xrc code */
typedef struct
{
   int (*pushCertificate)(lua_State *L, SoDispCon* con);
   int (*pushCipher)(lua_State *L, SoDispCon* con);
   void (*unlockSharkSSL)(lua_State *L,struct SharkSsl* super);
   struct SharkSsl* (*lockSharkSSL)(
      lua_State *L,int tabIx,SharkSsl_Role role,SharkSsl* lockedShark);
} LSharkSSLFuncs;
 /* Auto set if using xrc/lua/lsharkssl.c */
extern const LSharkSSLFuncs* lSharkSSLFuncs;


enum BaUserDataTypes {
   BA_VMPTR=1,
   BA_IOINTFTAB,
   BA_DIRTAB,
   BA_CMDTAB,
   BA_JSONPARSER,
   BA_TDIR,
   BA_TDIR_RSRDR,
   BA_TDIR_DAV,
   BA_TRESINTF,
   BA_TIOINTF,
   BA_TDIRITER,
   BA_THTTPCMD,
   BA_TCOOKIE,
   BA_TSESSION,
   BA_TSETRESPONSE,
   BA_TAUTHORIZERINTF,
   BA_TLUA_AUTHORIZER,
   BA_TUSERINTF,
   BA_TLUA_USER,
   BA_TJAUTHORIZER,
   BA_TJUSER,
   BA_TAUTHENTICATORINTF,
   BA_TAUTHENTICATOR,
   BA_TTIMER,
   BA_TUPLOAD,
   BA_TASYNCRESP,
   BA_TDIR_REDIRECTOR,
   BA_TDIR_TRACELOGGER,
   BA_DBGMON_NEWIO_CB /* ldbgmon.c */
};


/*

LHttpDir and Lua userdata memory layout:
+-----------+
| LHttpDir  |
+-----------+
| HttpDir   | <- Any type that inherits from HttpDir
+-----------+
*/
typedef struct LHttpDir
{
      lua_State *LM;
      HttpDir_Service orgServiceFunc;
      struct LHttpDir* parent;
      int parentRef;
      int funcRef; /* Reference to Lua service function, if any: In registry */
      int authenticatorRef; /* authenticator if any: In ENV */
      int authorizerRef; /* authorizer if any: In ENV */
      int utabRef; /* Table associated with udata saved in BA_WKTABLE */
} LHttpDir;


BA_API int LHttpResRdr_loadLsp(
   lua_State* L, IoIntf* io, const char* pathname, IoStat* st);
BA_API void balua_ubjson(lua_State* L);
BA_API void luaopen_ba_redirector(lua_State *L);
BA_API void ba_ldbgmon(
   lua_State* L, void (*exitCb)(void*,BaBool), void* exitCbData);
BA_API void balua_revcon(lua_State* L);
BA_API void balua_mallinfo(lua_State* L);

#ifdef __cplusplus
}
#endif


/** @} */ /* LSP Plugin */

#endif
