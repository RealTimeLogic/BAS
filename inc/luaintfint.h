/*
** 
** Lua interface functions.
** See Copyright Notice and documentation in luaintf.c
*/

#ifndef _luaintfint_h
#define _luaintfint_h

#define LUAINTF_VER 4

/* If using: https://realtimelogic.info/amalgamator/ */
#ifdef USE_AMALGAMATED_BAS
#include <barracuda.h>
#else
#include "lualib.h"
#include "lauxlib.h"
#endif

#ifndef ThreadLib_hpp
struct ThreadMutex;
#endif

LUA_API int lua_vgc(lua_State *L, int what, va_list argp);

struct BaTimer;

typedef lua_State *(*Tlua_newstate) (lua_Alloc f, void *ud);
typedef void (*Tlua_close) (lua_State *L);
typedef lua_State *(*Tlua_newthread) (lua_State *L);
typedef lua_CFunction (*Tlua_atpanic) (lua_State *L, lua_CFunction panicf);
typedef lua_Number (*Tlua_version) (lua_State *L);
typedef int (*Tlua_absindex) (lua_State *L, int idx);
typedef int (*Tlua_gettop) (lua_State *L);
typedef void (*Tlua_settop) (lua_State *L, int idx);
typedef void (*Tlua_pushvalue) (lua_State *L, int idx);
typedef void (*Tlua_rotate) (lua_State *L, int idx, int n);
typedef void (*Tlua_copy) (lua_State *L, int fromidx, int toidx);
typedef int (*Tlua_checkstack) (lua_State *L, int n);
typedef void (*Tlua_xmove) (lua_State *from, lua_State *to, int n);
typedef int (*Tlua_isnumber) (lua_State *L, int idx);
typedef int (*Tlua_isstring) (lua_State *L, int idx);
typedef int (*Tlua_iscfunction) (lua_State *L, int idx);
typedef int (*Tlua_isinteger) (lua_State *L, int idx);
typedef int (*Tlua_isuserdata) (lua_State *L, int idx);
typedef int (*Tlua_type) (lua_State *L, int idx);
typedef const char *(*Tlua_typename) (lua_State *L, int tp);
typedef lua_Number (*Tlua_tonumberx) (lua_State *L, int idx, int *isnum);
typedef lua_Integer (*Tlua_tointegerx) (lua_State *L, int idx, int *isnum);
typedef int (*Tlua_toboolean) (lua_State *L, int idx);
typedef const char *(*Tlua_tolstring) (lua_State *L, int idx, size_t *len);
typedef lua_Unsigned (*Tlua_rawlen) (lua_State *L, int idx);
typedef lua_CFunction (*Tlua_tocfunction) (lua_State *L, int idx);
typedef void *(*Tlua_touserdata) (lua_State *L, int idx);
typedef lua_State *(*Tlua_tothread) (lua_State *L, int idx);
typedef const void *(*Tlua_topointer) (lua_State *L, int idx);
typedef void (*Tlua_arith) (lua_State *L, int op);
typedef int (*Tlua_rawequal) (lua_State *L, int idx1, int idx2);
typedef int (*Tlua_compare) (lua_State *L, int idx1, int idx2, int op);
typedef void (*Tlua_pushnil) (lua_State *L);
typedef void (*Tlua_pushnumber) (lua_State *L, lua_Number n);
typedef void (*Tlua_pushinteger) (lua_State *L, lua_Integer n);
typedef const char *(*Tlua_pushlstring) (lua_State *L, const char *s, size_t len);
typedef const char *(*Tlua_pushstring) (lua_State *L, const char *s);
typedef const char *(*Tlua_pushvfstring) (lua_State *L, const char *fmt,va_list argp);
typedef void (*Tlua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
typedef void (*Tlua_pushboolean) (lua_State *L, int b);
typedef void (*Tlua_pushlightuserdata) (lua_State *L, void *p);
typedef int (*Tlua_pushthread) (lua_State *L);
typedef int (*Tlua_getglobal) (lua_State *L, const char *name);
typedef int (*Tlua_gettable) (lua_State *L, int idx);
typedef int (*Tlua_getfield) (lua_State *L, int idx, const char *k);
typedef int (*Tlua_geti) (lua_State *L, int idx, lua_Integer n);
typedef int (*Tlua_rawget) (lua_State *L, int idx);
typedef int (*Tlua_rawgeti) (lua_State *L, int idx, lua_Integer n);
typedef int (*Tlua_rawgetp) (lua_State *L, int idx, const void *p);
typedef void (*Tlua_createtable) (lua_State *L, int narr, int nrec);
typedef void *(*Tlua_newuserdata) (lua_State *L, size_t sz, int nuvalue);
typedef int (*Tlua_getmetatable) (lua_State *L, int objindex);
typedef int (*Tlua_getuservalue) (lua_State *L, int idx, int nuvalue);
typedef void (*Tlua_setglobal) (lua_State *L, const char *name);
typedef void (*Tlua_settable) (lua_State *L, int idx);
typedef void (*Tlua_setfield) (lua_State *L, int idx, const char *k);
typedef void (*Tlua_seti) (lua_State *L, int idx, lua_Integer n);
typedef void (*Tlua_rawset) (lua_State *L, int idx);
typedef void (*Tlua_rawseti) (lua_State *L, int idx, lua_Integer n);
typedef void (*Tlua_rawsetp) (lua_State *L, int idx, const void *p);
typedef int (*Tlua_setmetatable) (lua_State *L, int objindex);
typedef int (*Tlua_setuservalue) (lua_State *L, int idx, int nuvalue);
typedef void (*Tlua_callk) (lua_State *L, int nargs, int nresults,lua_KContext ctx, lua_KFunction k);
typedef int (*Tlua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc,lua_KContext ctx, lua_KFunction k);
typedef int (*Tlua_load) (lua_State *L, lua_Reader reader, void *dt,const char *chunkname, const char *mode);
typedef int (*Tlua_dump) (lua_State *L, lua_Writer writer, void *data, int strip);
typedef int (*Tlua_yieldk) (lua_State *L, int nresults, lua_KContext ctx,lua_KFunction k);
typedef int (*Tlua_resume) (lua_State *L, lua_State *from, int narg, int* nresults);
typedef int (*Tlua_status) (lua_State *L);
typedef int (*Tlua_isyieldable) (lua_State *L);
typedef int (*Tlua_vgc) (lua_State *L, int what, va_list argp);
typedef int (*Tlua_error) (lua_State *L);
typedef int (*Tlua_next) (lua_State *L, int idx);
typedef void (*Tlua_concat) (lua_State *L, int n);
typedef void (*Tlua_len) (lua_State *L, int idx);
typedef size_t (*Tlua_stringtonumber) (lua_State *L, const char *s);
typedef lua_Alloc (*Tlua_getallocf) (lua_State *L, void **ud);
typedef void (*Tlua_setallocf) (lua_State *L, lua_Alloc f, void *ud);
typedef int (*Tlua_getstack) (lua_State *L, int level, lua_Debug *ar);
typedef int (*Tlua_getinfo) (lua_State *L, const char *what, lua_Debug *ar);
typedef const char *(*Tlua_getlocal) (lua_State *L, const lua_Debug *ar, int n);
typedef const char *(*Tlua_setlocal) (lua_State *L, const lua_Debug *ar, int n);
typedef const char *(*Tlua_getupvalue) (lua_State *L, int funcindex, int n);
typedef const char *(*Tlua_setupvalue) (lua_State *L, int funcindex, int n);
typedef void *(*Tlua_upvalueid) (lua_State *L, int fidx, int n);
typedef void (*Tlua_upvaluejoin) (lua_State *L, int fidx1, int n1,int fidx2, int n2);
typedef void (*Tlua_sethook) (lua_State *L, lua_Hook func, int mask, int count);
typedef lua_Hook (*Tlua_gethook) (lua_State *L);
typedef int (*Tlua_gethookmask) (lua_State *L);
typedef int (*Tlua_gethookcount) (lua_State *L);
typedef void (*TluaL_checkversion_) (lua_State *L, lua_Number ver, size_t sz);
typedef int (*TluaL_getmetafield) (lua_State *L, int obj, const char *e);
typedef int (*TluaL_callmeta) (lua_State *L, int obj, const char *e);
typedef const char *(*TluaL_tolstring) (lua_State *L, int idx, size_t *len);
typedef int (*TluaL_argerror) (lua_State *L, int arg, const char *extramsg);
typedef const char *(*TluaL_checklstring) (lua_State *L, int arg,size_t *l);
typedef const char *(*TluaL_optlstring) (lua_State *L, int arg,const char *def, size_t *l);
typedef lua_Number (*TluaL_checknumber) (lua_State *L, int arg);
typedef lua_Number (*TluaL_optnumber) (lua_State *L, int arg, lua_Number def);
typedef lua_Integer (*TluaL_checkinteger) (lua_State *L, int arg);
typedef lua_Integer (*TluaL_optinteger) (lua_State *L, int arg,lua_Integer def);
typedef void (*TluaL_checkstack) (lua_State *L, int sz, const char *msg);
typedef void (*TluaL_checktype) (lua_State *L, int arg, int t);
typedef void (*TluaL_checkany) (lua_State *L, int arg);
typedef int (*TluaL_newmetatable) (lua_State *L, const char *tname);
typedef void (*TluaL_setmetatable) (lua_State *L, const char *tname);
typedef void *(*TluaL_testudata) (lua_State *L, int ud, const char *tname);
typedef void *(*TluaL_checkudata) (lua_State *L, int ud, const char *tname);
typedef void (*TluaL_where) (lua_State *L, int lvl);
typedef int (*TluaL_checkoption) (lua_State *L, int arg, const char *def,const char *const lst[]);
typedef int (*TluaL_fileresult) (lua_State *L, int stat, const char *fname);
typedef int (*TluaL_execresult) (lua_State *L, int stat);
typedef int (*TluaL_ref) (lua_State *L, int t);
typedef void (*TluaL_unref) (lua_State *L, int t, int ref);
typedef int (*TluaL_loadfilex) (lua_State *L, const char *filename,const char *mode);
typedef int (*TluaL_loadbufferx) (lua_State *L, const char *buff, size_t sz,const char *name, const char *mode);
typedef int (*TluaL_loadstring) (lua_State *L, const char *s);
typedef lua_State *(*TluaL_newstate) (void);
typedef lua_Integer (*TluaL_len) (lua_State *L, int idx);
typedef const char *(*TluaL_gsub) (lua_State *L, const char *s, const char *p,const char *r);
typedef void (*TluaL_setfuncs) (lua_State *L, const luaL_Reg *l, int nup);
typedef int (*TluaL_getsubtable) (lua_State *L, int idx, const char *fname);
typedef void (*TluaL_traceback) (lua_State *L, lua_State *L1,const char *msg, int level);
typedef void (*TluaL_requiref) (lua_State *L, const char *modname,lua_CFunction openf, int glb);
typedef void (*TluaL_buffinit) (lua_State *L, luaL_Buffer *B);
typedef char *(*TluaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
typedef void (*TluaL_addlstring) (luaL_Buffer *B, const char *s, size_t l);
typedef void (*TluaL_addstring) (luaL_Buffer *B, const char *s);
typedef void (*TluaL_addvalue) (luaL_Buffer *B);
typedef void (*TluaL_pushresult) (luaL_Buffer *B);
typedef void (*TluaL_pushresultsize) (luaL_Buffer *B, size_t sz);
typedef char *(*TluaL_buffinitsize) (lua_State *L, luaL_Buffer *B, size_t sz);

typedef struct ThreadMutex* (*TgetDispMutex)(lua_State* L);
typedef void (*TThreadMutex_release)(struct ThreadMutex* o);
typedef void (*TThreadMutex_set)(struct ThreadMutex* o);
typedef struct BaTimer* (*TgetTimer)(lua_State* L);

typedef struct {
   int version;
   Tlua_newstate lua_newstateFp;
   Tlua_close lua_closeFp;
   Tlua_newthread lua_newthreadFp;
   Tlua_atpanic lua_atpanicFp;
   Tlua_version lua_versionFp;
   Tlua_absindex lua_absindexFp;
   Tlua_gettop lua_gettopFp;
   Tlua_settop lua_settopFp;
   Tlua_pushvalue lua_pushvalueFp;
   Tlua_rotate lua_rotateFp;
   Tlua_copy lua_copyFp;
   Tlua_checkstack lua_checkstackFp;
   Tlua_xmove lua_xmoveFp;
   Tlua_isnumber lua_isnumberFp;
   Tlua_isstring lua_isstringFp;
   Tlua_iscfunction lua_iscfunctionFp;
   Tlua_isinteger lua_isintegerFp;
   Tlua_isuserdata lua_isuserdataFp;
   Tlua_type lua_typeFp;
   Tlua_typename lua_typenameFp;
   Tlua_tonumberx lua_tonumberxFp;
   Tlua_tointegerx lua_tointegerxFp;
   Tlua_toboolean lua_tobooleanFp;
   Tlua_tolstring lua_tolstringFp;
   Tlua_rawlen lua_rawlenFp;
   Tlua_tocfunction lua_tocfunctionFp;
   Tlua_touserdata lua_touserdataFp;
   Tlua_tothread lua_tothreadFp;
   Tlua_topointer lua_topointerFp;
   Tlua_arith lua_arithFp;
   Tlua_rawequal lua_rawequalFp;
   Tlua_compare lua_compareFp;
   Tlua_pushnil lua_pushnilFp;
   Tlua_pushnumber lua_pushnumberFp;
   Tlua_pushinteger lua_pushintegerFp;
   Tlua_pushlstring lua_pushlstringFp;
   Tlua_pushstring lua_pushstringFp;
   Tlua_pushvfstring lua_pushvfstringFp;
   Tlua_pushcclosure lua_pushcclosureFp;
   Tlua_pushboolean lua_pushbooleanFp;
   Tlua_pushlightuserdata lua_pushlightuserdataFp;
   Tlua_pushthread lua_pushthreadFp;
   Tlua_getglobal lua_getglobalFp;
   Tlua_gettable lua_gettableFp;
   Tlua_getfield lua_getfieldFp;
   Tlua_geti lua_getiFp;
   Tlua_rawget lua_rawgetFp;
   Tlua_rawgeti lua_rawgetiFp;
   Tlua_rawgetp lua_rawgetpFp;
   Tlua_createtable lua_createtableFp;
   Tlua_newuserdata lua_newuserdataFp;
   Tlua_getmetatable lua_getmetatableFp;
   Tlua_getuservalue lua_getuservalueFp;
   Tlua_setglobal lua_setglobalFp;
   Tlua_settable lua_settableFp;
   Tlua_setfield lua_setfieldFp;
   Tlua_seti lua_setiFp;
   Tlua_rawset lua_rawsetFp;
   Tlua_rawseti lua_rawsetiFp;
   Tlua_rawsetp lua_rawsetpFp;
   Tlua_setmetatable lua_setmetatableFp;
   Tlua_setuservalue lua_setuservalueFp;
   Tlua_callk lua_callkFp;
   Tlua_pcallk lua_pcallkFp;
   Tlua_load lua_loadFp;
   Tlua_dump lua_dumpFp;
   Tlua_yieldk lua_yieldkFp;
   Tlua_resume lua_resumeFp;
   Tlua_status lua_statusFp;
   Tlua_isyieldable lua_isyieldableFp;
   Tlua_vgc lua_vgcFp;
   Tlua_error lua_errorFp;
   Tlua_next lua_nextFp;
   Tlua_concat lua_concatFp;
   Tlua_len lua_lenFp;
   Tlua_stringtonumber lua_stringtonumberFp;
   Tlua_getallocf lua_getallocfFp;
   Tlua_setallocf lua_setallocfFp;
   Tlua_getstack lua_getstackFp;
   Tlua_getinfo lua_getinfoFp;
   Tlua_getlocal lua_getlocalFp;
   Tlua_setlocal lua_setlocalFp;
   Tlua_getupvalue lua_getupvalueFp;
   Tlua_setupvalue lua_setupvalueFp;
   Tlua_upvalueid lua_upvalueidFp;
   Tlua_upvaluejoin lua_upvaluejoinFp;
   Tlua_sethook lua_sethookFp;
   Tlua_gethook lua_gethookFp;
   Tlua_gethookmask lua_gethookmaskFp;
   Tlua_gethookcount lua_gethookcountFp;
   TluaL_checkversion_ luaL_checkversion_Fp;
   TluaL_getmetafield luaL_getmetafieldFp;
   TluaL_callmeta luaL_callmetaFp;
   TluaL_tolstring luaL_tolstringFp;
   TluaL_argerror luaL_argerrorFp;
   TluaL_checklstring luaL_checklstringFp;
   TluaL_optlstring luaL_optlstringFp;
   TluaL_checknumber luaL_checknumberFp;
   TluaL_optnumber luaL_optnumberFp;
   TluaL_checkinteger luaL_checkintegerFp;
   TluaL_optinteger luaL_optintegerFp;
   TluaL_checkstack luaL_checkstackFp;
   TluaL_checktype luaL_checktypeFp;
   TluaL_checkany luaL_checkanyFp;
   TluaL_newmetatable luaL_newmetatableFp;
   TluaL_setmetatable luaL_setmetatableFp;
   TluaL_testudata luaL_testudataFp;
   TluaL_checkudata luaL_checkudataFp;
   TluaL_where luaL_whereFp;
   TluaL_checkoption luaL_checkoptionFp;
   TluaL_fileresult luaL_fileresultFp;
   TluaL_execresult luaL_execresultFp;
   TluaL_ref luaL_refFp;
   TluaL_unref luaL_unrefFp;
   TluaL_loadfilex luaL_loadfilexFp;
   TluaL_loadbufferx luaL_loadbufferxFp;
   TluaL_loadstring luaL_loadstringFp;
   TluaL_newstate luaL_newstateFp;
   TluaL_len luaL_lenFp;
   TluaL_gsub luaL_gsubFp;
   TluaL_setfuncs luaL_setfuncsFp;
   TluaL_getsubtable luaL_getsubtableFp;
   TluaL_traceback luaL_tracebackFp;
   TluaL_requiref luaL_requirefFp;
   TluaL_buffinit luaL_buffinitFp;
   TluaL_prepbuffsize luaL_prepbuffsizeFp;
   TluaL_addlstring luaL_addlstringFp;
   TluaL_addstring luaL_addstringFp;
   TluaL_addvalue luaL_addvalueFp;
   TluaL_pushresult luaL_pushresultFp;
   TluaL_pushresultsize luaL_pushresultsizeFp;
   TluaL_buffinitsize luaL_buffinitsizeFp;

   TgetDispMutex getDispMutexFp;
   TThreadMutex_release ThreadMutex_releaseFp;
   TThreadMutex_set ThreadMutex_setFp;
   TgetTimer getTimerFp;
} LuaFuncs;


#ifdef loadlib_c
#include <balua.h>

static BaLua_param* getBaParem(lua_State* L)
{
  BaLua_param* p;
  balua_pushbatab(L);
  lua_rawgeti(L, -1, BA_VMPTR);
  p = (BaLua_param*)lua_touserdata(L, -1);
  lua_pop(L,2);
  return p;
}
static struct ThreadMutex* getDispMutex(lua_State* L)
{
   BaLua_param* p = getBaParem(L);
   return p ? p->mutex : 0;
}
static struct BaTimer* getTimer(lua_State* L)
{
   BaLua_param* p = getBaParem(L);
   return p ? p->timer : 0;
}
static void ThreadMutex_releaseWrapper(struct ThreadMutex* o)
{
   if(o) ThreadMutex_release(o);
}
static void ThreadMutex_setWrapper(struct ThreadMutex* o)
{
   if(o) ThreadMutex_set(o);
}


static const LuaFuncs luaFuncs={
   LUAINTF_VER,
   lua_newstate,
   lua_close,
   lua_newthread,
   lua_atpanic,
   lua_version,
   lua_absindex,
   lua_gettop,
   lua_settop,
   lua_pushvalue,
   lua_rotate,
   lua_copy,
   lua_checkstack,
   lua_xmove,
   lua_isnumber,
   lua_isstring,
   lua_iscfunction,
   lua_isinteger,
   lua_isuserdata,
   lua_type,
   lua_typename,
   lua_tonumberx,
   lua_tointegerx,
   lua_toboolean,
   lua_tolstring,
   lua_rawlen,
   lua_tocfunction,
   lua_touserdata,
   lua_tothread,
   lua_topointer,
   lua_arith,
   lua_rawequal,
   lua_compare,
   lua_pushnil,
   lua_pushnumber,
   lua_pushinteger,
   lua_pushlstring,
   lua_pushstring,
   lua_pushvfstring,
   lua_pushcclosure,
   lua_pushboolean,
   lua_pushlightuserdata,
   lua_pushthread,
   lua_getglobal,
   lua_gettable,
   lua_getfield,
   lua_geti,
   lua_rawget,
   lua_rawgeti,
   lua_rawgetp,
   lua_createtable,
   lua_newuserdatauv,
   lua_getmetatable,
   lua_getiuservalue,
   lua_setglobal,
   lua_settable,
   lua_setfield,
   lua_seti,
   lua_rawset,
   lua_rawseti,
   lua_rawsetp,
   lua_setmetatable,
   lua_setiuservalue,
   lua_callk,
   lua_pcallk,
   lua_load,
   lua_dump,
   lua_yieldk,
   lua_resume,
   lua_status,
   lua_isyieldable,
   lua_vgc,
   lua_error,
   lua_next,
   lua_concat,
   lua_len,
   lua_stringtonumber,
   lua_getallocf,
   lua_setallocf,
   lua_getstack,
   lua_getinfo,
   lua_getlocal,
   lua_setlocal,
   lua_getupvalue,
   lua_setupvalue,
   lua_upvalueid,
   lua_upvaluejoin,
   lua_sethook,
   lua_gethook,
   lua_gethookmask,
   lua_gethookcount,
   luaL_checkversion_,
   luaL_getmetafield,
   luaL_callmeta,
   luaL_tolstring,
   luaL_argerror,
   luaL_checklstring,
   luaL_optlstring,
   luaL_checknumber,
   luaL_optnumber,
   luaL_checkinteger,
   luaL_optinteger,
   luaL_checkstack,
   luaL_checktype,
   luaL_checkany,
   luaL_newmetatable,
   luaL_setmetatable,
   luaL_testudata,
   luaL_checkudata,
   luaL_where,
   luaL_checkoption,
   luaL_fileresult,
   luaL_execresult,
   luaL_ref,
   luaL_unref,
   luaL_loadfilex,
   luaL_loadbufferx,
   luaL_loadstring,
   luaL_newstate,
   luaL_len,
   luaL_gsub,
   luaL_setfuncs,
   luaL_getsubtable,
   luaL_traceback,
   luaL_requiref,
   luaL_buffinit,
   luaL_prepbuffsize,
   luaL_addlstring,
   luaL_addstring,
   luaL_addvalue,
   luaL_pushresult,
   luaL_pushresultsize,
   luaL_buffinitsize,

   getDispMutex,
   ThreadMutex_releaseWrapper,
   ThreadMutex_setWrapper,
   getTimer
};
#endif

#endif
