---
name: bas-lua-bindings
description: Design, review, and implement C or C++ Lua bindings for Barracuda App Server (BAS), Mako, Xedge, and custom BAS runtimes. Use when creating Lua-to-native bindings, native-to-Lua callbacks, BAS-aware ba.* modules, ba.create.* factories, IoIntf bindings, request/response helpers, or HttpDir-derived directory objects.
---

# BAS Lua Binding Skill

Use this skill when designing, reviewing, or implementing Lua bindings for the Barracuda App Server (BAS) or a derivative runtime using the BAS Lua environment.

If you are new to BAS: BAS is the C/C++ server engine behind Mako Server, Xedge, and similar runtimes. It exposes the `ba` Lua API, routes HTTP requests through `HttpDir` directory objects, and provides request/response objects to LSP pages and Lua callbacks.

This skill is self-contained. It is written for an agent that may have access only to BAS header files, not to BAS implementation files or a local BAS checkout. When checking declarations, use the public/runtime headers available in the target source tree.

## First Principles

BAS Lua bindings are not just generic Lua native modules using the Lua C API. A binding may run in one of three modes:

1. Generic Lua module: uses plain Lua `luaL_newmetatable`, `lua_newuserdata`, `luaL_checkudata`, and `luaL_setfuncs` without BAS upvalues. Use this for socket-like, crypto-like, parser-like, or utility-like objects that do not need BAS runtime state.
2. BAS-aware module: uses the BAS environment table as an upvalue so functions can access server state, mutex, registered metatables, VM I/O, request/response objects, and BAS helper functions. Examples: `ba.json`, `ba.io`, timers, request helpers, and many `ba.create.*` functions.
3. BAS directory object: creates or extends an `HttpDir`-derived native object that can be inserted into the BAS directory tree and exposed as a Lua userdata. Examples: `ba.create.dir`, `ba.create.resrdr`, `ba.create.dav`, and `ba.create.tracelogger`.

Pick the smallest mode that satisfies the API:

- Use a generic module when the binding does not need BAS state.
- Use a BAS-aware module when it needs `BaLua_param`, VM I/O, the server, timers, request/response userdata, or BAS typed userdata.
- Use a directory object when the binding should participate in URL dispatch, authentication/authorization, child directory insertion, LSP resource handling, or WebSocket/service callbacks.

Minimal BAS mental model:

- BAS owns one Lua state created with `balua_create`.
- The global Lua table `ba` is the main BAS API surface.
- `HttpServer` receives HTTP requests and dispatches them through a tree of `HttpDir` directory objects.
- `HttpCommand` represents one request/response transaction; request-scoped values must not be saved after the request finishes.
- `IoIntf` abstracts resources and files, including disk, ZIP, VM I/O, and embedded resources.
- The dispatcher mutex protects the Lua state. Lua C API calls must happen only while that mutex is locked.

## User Triage

When the user has little BAS binding experience, guide them before designing code. Ask only the questions needed to choose the binding category and lifetime model. If the request already answers a question, do not ask it again.

Start with these questions:

- Direction: Should Lua call native code, should native code call Lua asynchronously, or should an HTTP request reach native code?
- Runtime: Is this for a custom BAS integration, Mako, Xedge, or a loadable Lua module?
- Language: Should the native binding be implemented in C or C++?
- BAS state: Does the binding need `request`, `response`, `HttpServer`, `IoIntf`, timers, or other BAS objects?
- Lifetime: Who owns the native object, how is it closed, and can Lua garbage collection destroy it?
- Blocking behavior: Can any native call block, sleep, wait for I/O, wait for hardware/network/processes, or run for long enough to affect other requests?
- Callback behavior: Does native code need to call a Lua function later, after the original Lua call returns?
- HTTP integration: Should the binding mount into the URL tree as a directory, use auth, contain child directories, or serve/fallback to resources?
- Lua API shape: Should this be `require("mod")`, a global/module function, `ba.*`, `ba.create.*`, or a userdata object with methods?

Map answers to a binding pattern:

- Lua calls native code, no BAS state: generic Lua native module using Lua C API functions such as `luaL_newmetatable`, `lua_newuserdata`, and `luaL_checkudata` as needed.
- Lua calls native code and needs BAS state: BAS-aware module registered with the BA environment upvalue.
- Lua owns a native object: userdata object with `__gc`, usually `__close`, a checked closed state, and explicit ownership rules.
- Native code calls Lua asynchronously: `LThreadMgr` plus `ThreadJob`; anchor the Lua callback in userdata/uservalue and recover it in the job with BAS weak references when needed.
- HTTP request calls native function: BAS-aware request/response binding using `baluaENV_checkcmd`.
- URL tree integration: `HttpDir`-derived directory object created with `baluaENV_createDir` and registered as a `BA_TDIR` subtype.
- Filesystem/resource abstraction: `IoIntf` binding or a Lua-owned dynamic I/O object.
- C++ implementation: same binding categories as C, plus C ABI entry points, no exceptions crossing Lua/BAS callbacks, placement `new` for objects stored in userdata, and explicit destructor calls from `__gc`/`close`.

Use this compact decision tree:

```text
If Lua calls native code only:
  If no BAS state is needed -> generic Lua native module.
  If BAS state/request/IoIntf is needed -> BAS-aware module.
  If native object lifetime matters -> userdata with __gc/__close.
  If URL tree integration is needed -> HttpDir-derived directory object.

If native code calls Lua:
  Use LThreadMgr + ThreadJob.
  Anchor callbacks in userdata/uservalue.
  Use balua_wkRef for job lookup when the userdata is not on the stack.
  Queue through a safe context before LThreadMgr_run if the event source cannot lock the BAS mutex.

If request/response is needed:
  Register with BA environment upvalue.
  Accept request/response as BA_THTTPCMD via baluaENV_checkcmd.

If filesystem/resource abstraction is needed:
  Use IoIntf or expose a new IoIntf-backed userdata.
```

After triage, summarize the selected pattern in one or two sentences before writing code. Example: "This is a BAS-aware userdata binding: Lua calls native code, the object owns a native handle, and the read method can block, so the read path must release the BAS mutex around only the blocking native call."

## Agent Operating Procedure

Use this procedure when starting from little or no BAS context:

1. Classify the runtime: custom BAS, Mako, Xedge, or loadable Lua module.
2. Classify the implementation language: C or C++.
3. Classify the call direction: Lua-to-native, native-to-Lua callback, HTTP-to-native, or URL tree directory object.
4. Decide whether the binding needs BAS state. If yes, use BAS-aware registration with the BA environment upvalue.
5. Decide ownership and lifetime before coding: native owner, Lua userdata owner, runtime owner, or shared parent/child directory reference.
6. Identify blocking calls and callback paths before writing functions.
7. Read the relevant headers listed below and use their declarations exactly.
8. State the selected pattern and install point before generating code.
9. Generate the smallest binding that satisfies the requested API.
10. Add the runtime install call in the correct place: custom BAS startup, Mako hook, Xedge `xedgeOpenAUX`, or Lua `require` loader.
11. Verify stack balance, close/GC behavior, invalid argument behavior, and runtime startup.
12. Review the generated code against the checklist at the end of this skill, then revise before presenting it.

Stop and ask the user for clarification if the runtime, language, call direction, or ownership model is unknown and cannot be inferred from the request.

## Header Inputs

When headers are available, check these first. Use Unix-style paths in all notes and generated guidance. Treat these headers as authoritative; do not recreate BAS structs, enums, or macros from examples in this skill.

- [`inc/balua.h`](inc/balua.h): public BAS Lua API, `BaLua_param`, `BA_ENV_IX`, `balua_newlib`, mutex helpers, typed userdata helpers, `LHttpDir` layout, and exported directory/I/O helpers.
- [`inc/HttpServer.h`](inc/HttpServer.h): `HttpServer`, `HttpDir`, `HttpCommand`, `HttpRequest`, `HttpResponse`, directory insertion, service callback, authentication, authorization, request, and response APIs.
- [`inc/IoIntf.h`](inc/IoIntf.h): resource and directory iteration interfaces such as `IoIntf`, `ResIntf`, `DirIntf`, `IoStat`, and open/stat/read/write operations.
- [`inc/BaDiskIo.h`](inc/BaDiskIo.h) and other I/O headers available in the target runtime: concrete `IoIntf` implementations.
- [`inc/ThreadLib.h`](inc/ThreadLib.h) and platform thread headers: `ThreadMutex`, `Thread`, `ThreadSemaphore`, and native thread primitives.
- [`inc/BaTimer.h`](inc/BaTimer.h): timer API when designing timer-backed bindings.
- [`inc/SoDisp.h`](inc/SoDisp.h) and connection headers when designing dispatcher, socket, WebSocket, or event-driven bindings.

Do not require implementation files to use this skill. The operational details below include the important implementation patterns directly.

Generated C or C++ code must include BAS headers and use the declarations from those headers. Any BAS macro names, type names, enum names, or field summaries in this skill are orientation aids, not replacement declarations.

## BAS Lua Environment Model

The BAS environment table is stored in the Lua registry under `BA_TABLE` (`"_BA"`) and is normally available to BAS-aware C functions as upvalue index 1:

```c
#define BA_ENV_IX lua_upvalueindex(1)
#define balua_pushbatab(L) lua_getfield(L,LUA_REGISTRYINDEX,BA_TABLE)
#define balua_newlib(L,l) \
  (luaL_newlibtable(L,l), balua_pushbatab(L), luaL_setfuncs(L,l,1))
```

Do not paste these BAS macro definitions into generated code. Include [`inc/balua.h`](inc/balua.h) and use the macros supplied by the target BAS version.

`balua_create()` creates the Lua state, creates the BA table, stores a copied `BaLua_param` at `BA_VMPTR`, registers core metatables, registers the VM I/O object, and installs the global `ba` table with `luaL_requiref`.

Use `balua_getparam(L)` when the function might not have `BA_ENV_IX` as an upvalue and must fetch the BA table from the registry. Use `baluaENV_getparam(L)` when called from a BAS-aware function registered with the BA table upvalue.

The copied `BaLua_param` gives access to:

- `L`: Lua state
- `server`: BAS `HttpServer`
- `timer`: optional `BaTimer`
- `vmio`: required VM `IoIntf`
- `mutex`: dispatcher mutex
- `tracker`: optional login tracker
- zip signing/encryption configuration

Use the exact `BaLua_param` declaration from [`inc/balua.h`](inc/balua.h). Do not re-declare it in generated code. Conceptually, it is the runtime parameter object that provides access to the Lua state, server, timer, VM I/O, dispatcher mutex, optional login tracker, error handler reference, and ZIP signing/encryption configuration.

## Metatable Registry and Type IDs

BAS stores typed userdata metatables in the BA environment table by integer type ID. The important IDs from `enum BaUserDataTypes` commonly include the names below. Verify the exact enum in [`inc/balua.h`](inc/balua.h) for the target BAS version and do not re-declare these IDs in binding code:

- `BA_TDIR`: base Lua `HttpDir`
- `BA_TDIR_RSRDR`: `HttpResRdr`, inherits from `BA_TDIR`
- `BA_TDIR_DAV`: `WebDAV`, inherits from `BA_TDIR`
- `BA_TRESINTF`: resource/file handle
- `BA_TIOINTF`: I/O interface
- `BA_TDIRITER`: directory iterator
- `BA_THTTPCMD`: request/response command object
- `BA_TCOOKIE`, `BA_TSESSION`, `BA_TSETRESPONSE`
- `BA_TAUTHORIZERINTF`, `BA_TLUA_AUTHORIZER`, `BA_TJAUTHORIZER`
- `BA_TUSERINTF`, `BA_TLUA_USER`, `BA_TJUSER`
- `BA_TAUTHENTICATORINTF`, `BA_TAUTHENTICATOR`
- `BA_TTIMER`, `BA_TUPLOAD`, `BA_TASYNCRESP`
- `BA_TDIR_TRACELOGGER`: external trace logger directory type
- `BA_DBGMON_NEWIO_CB`, `BA_IOINTFPTRTAB`: debug monitor and ZIP I/O internals

Register BAS typed userdata with:

```c
baluaENV_register(L, MY_TYPE_ID, OPTIONAL_PARENT_TYPE_ID, myMethods);
```

This creates the metatable, sets `metatable.__index = metatable`, stores the BA table as upvalue 1 for all methods, and optionally sets the metatable's metatable to the inherited type metatable. `baluaENV_checkudata()` understands this inheritance chain.

Use:

```c
MyObj* o = (MyObj*)baluaENV_checkudata(L, 1, MY_TYPE_ID);
```

for BAS typed objects, not `luaL_checkudata`.

Use `luaL_newmetatable` and `luaL_checkudata` only for generic objects that are not part of the BAS typed userdata registry.

## Return and Error Conventions

Prefer BAS conventions:

- Return `true` for status code `0`.
- Return `nil, errstr, errcode` for BAS error codes via `balua_pushstatus(L, status)`.
- Use `luaL_error()` for programmer errors: wrong type, unsupported operation, invalid state, recursive/busy use.
- Use `balua_manageerr()` or `balua_errorhandler()` when errors occur in callbacks detached from direct Lua call flow.
- Preserve stack balance. For install functions, restore the original stack top unless returning a documented value.

## Boolean, Field, and Memory Helpers

Use BAS helpers where available:

- `balua_checkboolean(L, ix)` and `balua_optboolean(L, ix, def)` for strict boolean parsing.
- `balua_getStringField`, `balua_getIntField`, `balua_getBoolField`, `balua_getTabField`.
- `balua_checkStringField`, `balua_checkIntField`.
- `baLMalloc(L, size)` for allocations tied to Lua operations. It runs an emergency GC before failing.
- `balua_getuservalue(L, absIndex)` when a full userdata needs an associated Lua table for callbacks, peer objects, or state.
- `balua_wkRef(L)`, `balua_wkRefIx(L, ix)`, `balua_wkPush(L, ref)`, and `balua_wkUnref(L, ref)` for weak references to Lua values that C jobs/callbacks must recover later. Verify these declarations in [`inc/balua.h`](inc/balua.h).
- Pair `baLMalloc`/`baFree`, constructor/destructor, and Lua `__gc` consistently.

Do not touch the Lua stack while the BAS mutex is released.

## Mutex Rules

The Lua VM is protected by the BAS dispatcher mutex. A C binding starts with the mutex locked. If a C call blocks, sleeps, waits on I/O, does DNS/network operations, waits for a process/thread, or performs long computation, release the mutex around only that C section:

```c
ThreadMutex* m = baluaENV_getmutex(L);
ThreadMutex_release(m);
status = slow_c_call(o);
ThreadMutex_set(m);
```

or:

```c
GET_BAMUTEX;
balua_releasemutex(m);
status = slow_c_call(o);
balua_setmutex(m);
```

Rules:

- Never call any Lua C API while the mutex is released.
- Copy Lua strings or values needed by the slow call before releasing the mutex.
- Reacquire the mutex before pushing return values or raising Lua errors.
- Protect object state with a `busy` flag if another Lua coroutine could re-enter the same userdata while the mutex is released.
- If waiting for a helper thread to terminate from `__gc` or `close`, release/reacquire the mutex in the polling loop: release, sleep or wait briefly, reacquire, then check the termination flag again.

## Generic Object Binding Template

Use this for standalone modules that do not need BAS helpers:

```c
#include <lua.h>
#include <lauxlib.h>

#define MYOBJ_MT "MyObj"

typedef struct {
   int closed;
   MyHandle h;
} MyObj;

static MyObj*
MyObj_check(lua_State* L, int ix)
{
   MyObj* o = (MyObj*)luaL_checkudata(L, ix, MYOBJ_MT);
   if(o->closed)
      luaL_error(L, "closed");
   return o;
}

static int
MyObj_close(lua_State* L)
{
   MyObj* o = (MyObj*)luaL_checkudata(L, 1, MYOBJ_MT);
   if(!o->closed) {
      MyHandle_destructor(&o->h);
      o->closed = 1;
   }
   return 0;
}

static int
MyObj_method(lua_State* L)
{
   MyObj* o = MyObj_check(L, 1);
   (void)o;
   lua_pushboolean(L, 1);
   return 1;
}

static const luaL_Reg myObjMethods[] = {
   {"method", MyObj_method},
   {"close", MyObj_close},
   {"__close", MyObj_close},
   {"__gc", MyObj_close},
   {NULL, NULL}
};

static int
my_open(lua_State* L)
{
   MyObj* o = (MyObj*)lua_newuserdata(L, sizeof(MyObj));
   memset(o, 0, sizeof(*o));
   MyHandle_constructor(&o->h);

   if(luaL_newmetatable(L, MYOBJ_MT)) {
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");
      luaL_setfuncs(L, myObjMethods, 0);
   }
   lua_setmetatable(L, -2);
   return 1;
}

static const luaL_Reg myLib[] = {
   {"open", my_open},
   {NULL, NULL}
};

int
luaopen_mymodule(lua_State* L)
{
   luaL_newlib(L, myLib);
   return 1;
}
```

If the object owns a Lua callback, peer object, or state table, allocate the userdata with uservalue slots when supported by the target Lua version:

```c
MyObj* o = (MyObj*)lua_newuserdatauv(L, sizeof(MyObj), 1);
```

Then store the Lua callback/state in the uservalue so the garbage collector keeps it alive for the userdata lifetime. If C code must later recover the callback from an `LThreadMgr` job without having the userdata on the stack, also create a BAS weak reference with `balua_wkRef` and recover it with `balua_wkPush` in the job runner. Release the weak reference with `balua_wkUnref` when closing the object.

If the module is installed into global `ba`, use an install function that fetches the global `ba` table and registers functions on it:

```c
void
balua_mymodule(lua_State* L)
{
   lua_getglobal(L, "ba");
   luaL_setfuncs(L, myLib, 0);
   lua_pop(L, 1);
}
```

## C++ Binding Rules

Use these rules when the implementation language is C++.

Keep the Lua/BAS boundary C-compatible:

- Export Lua loader or runtime install functions with C linkage when they are called by Lua, C, Mako, Xedge, or a C build system:

```cpp
extern "C" int luaopen_mymodule(lua_State* L);
extern "C" void balua_myfeature(lua_State* L);
```

- Register only free functions or `static` member functions in `luaL_Reg`. Non-static C++ member functions do not match the Lua C function signature.
- Do not let C++ exceptions cross Lua C API calls, BAS callbacks, `HttpDir` service callbacks, `ThreadJob` callbacks, `__gc`, or `__close`.
- Catch C++ exceptions at every Lua-facing function boundary and convert them to `luaL_error` or `nil, err, code`.
- Do not rely on C++ destructors running after `luaL_error`; Lua normally uses longjmp-style error handling. Release critical resources before calling `luaL_error`, or return an error status instead.
- `__gc` and destructors must not throw. Catch all exceptions and return `0`.
- Use RAII inside C++ implementation code, but make Lua-visible ownership explicit with `close`, `__gc`, and a closed/constructed flag.
- Store C++ objects in Lua userdata with placement `new` and call the destructor explicitly from `close`/`__gc`.
- If a C++ object wraps or derives from BAS C objects, preserve the BAS C layout requirement. For `HttpDir` directory objects, keep the BAS-compatible `HttpDir` object at the expected start of the userdata payload or store the C++ object separately and use static C callback trampolines.
- When including C BAS headers from C++, use the headers' own C++ guards if present. If a required C header lacks C++ guards in the target runtime, wrap only C declarations with `extern "C"` as appropriate for that build.

Minimal C++ userdata pattern:

```cpp
#include <new>
#include <exception>
#include <lua.h>
#include <lauxlib.h>

class MyThing {
public:
   explicit MyThing(int value);
   ~MyThing();
   int value() const;
};

struct LMyThing {
   bool constructed;
   alignas(MyThing) unsigned char storage[sizeof(MyThing)];
};

static MyThing*
LMyThing_obj(LMyThing* u)
{
   return reinterpret_cast<MyThing*>(u->storage);
}

static int
LMyThing_gc(lua_State* L)
{
   LMyThing* u = static_cast<LMyThing*>(luaL_checkudata(L, 1, "MyThing"));
   if(u->constructed) {
      try {
         LMyThing_obj(u)->~MyThing();
      } catch(...) {
      }
      u->constructed = false;
   }
   return 0;
}

static int
LMyThing_new(lua_State* L)
{
   int value = static_cast<int>(luaL_checkinteger(L, 1));
   LMyThing* u = static_cast<LMyThing*>(lua_newuserdata(L, sizeof(LMyThing)));
   u->constructed = false;
   luaL_getmetatable(L, "MyThing");
   lua_setmetatable(L, -2);

   try {
      new (u->storage) MyThing(value);
      u->constructed = true;
   } catch(const std::exception& e) {
      return luaL_error(L, "%s", e.what());
   } catch(...) {
      return luaL_error(L, "MyThing constructor failed");
   }
   return 1;
}
```

For BAS-aware C++ bindings, keep the same BAS registration rules as C bindings: functions that call `baluaENV_*` must be registered with the BA table as upvalue, and directory objects must follow the `LHttpDir`/`HttpDir` layout rules.

## BAS-Aware Module Template

Use this when functions need `BaLua_param`, `IoIntf`, the server, the mutex, or other BAS userdata:

```c
#include <balua.h>
#include <HttpServer.h>

static int
my_status(lua_State* L)
{
   BaLua_param* p = baluaENV_getparam(L);
   lua_pushboolean(L, p->server ? TRUE : FALSE);
   return 1;
}

static int
my_with_request(lua_State* L)
{
   HttpCommand* cmd = baluaENV_checkcmd(L, 1);
   HttpRequest* req = &cmd->request;
   lua_pushstring(L, HttpRequest_getRequestURI(req));
   return 1;
}

static const luaL_Reg myLib[] = {
   {"status", my_status},
   {"requesturi", my_with_request},
   {NULL, NULL}
};

void
luaopen_ba_my(lua_State* L)
{
   balua_newlib(L, myLib);
}
```

To attach this module under `ba`, either call `luaopen_ba_my(L)` while building the `ba` table and set a field, or install later by fetching `ba`, pushing `balua_pushbatab(L)`, then calling `luaL_setfuncs(L, funcs, 1)`.

## BAS Typed Object Template

Use this for BAS objects that are not directories but need typed checks and inheritance:

```c
#define BA_TMYOBJ MY_ALLOCATED_TYPE_ID

typedef struct {
   MyCObj super;
   int ref;
} LMyObj;

#define LMyObj_check(L, ix) \
   ((LMyObj*)baluaENV_checkudata(L, ix, BA_TMYOBJ))

static int
LMyObj_gc(lua_State* L)
{
   LMyObj* o = LMyObj_check(L, 1);
   if(o->ref) {
      luaL_unref(L, LUA_REGISTRYINDEX, o->ref);
      o->ref = 0;
   }
   MyCObj_destructor(&o->super);
   return 0;
}

static int
LMyObj_create(lua_State* L)
{
   LMyObj* o = (LMyObj*)baluaENV_newuserdata(L, BA_TMYOBJ, sizeof(LMyObj));
   memset(o, 0, sizeof(*o));
   MyCObj_constructor(&o->super);
   return 1;
}

static const luaL_Reg myObjMethods[] = {
   {"__gc", LMyObj_gc},
   {NULL, NULL}
};

static const luaL_Reg myCreateLib[] = {
   {"myobj", LMyObj_create},
   {NULL, NULL}
};

void
balua_myobj(lua_State* L)
{
   int status;
   balua_pushbatab(L);
   lua_pushcclosure(L, my_register, 1);
   status = lua_pcall(L, 0, 0, 0);
   if(status)
      baFatalE(FE_BLUA_PANIC, status);

   lua_getglobal(L, "ba");
   lua_getfield(L, -1, "create");
   balua_pushbatab(L);
   luaL_setfuncs(L, myCreateLib, 1);
   lua_pop(L, 2);
}
```

Register the metatable from a closure that has BA table as upvalue:

```c
static int
my_register(lua_State* L)
{
   baluaENV_register(L, BA_TMYOBJ, 0, myObjMethods);
   return 0;
}
```

## Directory Object Design

Directory objects are special. `LHttpDir` is a header stored immediately before the actual `HttpDir` or derived directory instance in one Lua userdata:

```text
+----------+
| LHttpDir |
+----------+
| HttpDir  |  <- or struct whose first member inherits from HttpDir
+----------+
```

Use the exact `LHttpDir` declaration from [`inc/balua.h`](inc/balua.h). Do not re-declare it in generated code. Conceptually, the header carries the main Lua state, service override state, parent/child lifetime links, callback references, authentication references, and a reference to the userdata-associated table.

Use `baluaENV_createDir(L, mtId, dirSize)` to allocate the userdata. It:

- Allocates `sizeof(LHttpDir) + dirSize`.
- Sets the metatable for `mtId`.
- Creates a userdata uservalue table.
- Stores a weak reference to that table so service callbacks can recover Lua references.
- Stores the main Lua thread in `LHttpDir.LM`.
- Returns the `HttpDir*` pointer at `(ldir + 1)`.

Base helpers:

- `baluaENV_toDir(L, ix)`: get `HttpDir*` from a Lua directory userdata or a Lua table with a `dir` field.
- `baluaENV_createDir(L, mtId, dirSize)`: create and push directory userdata.
- `BA_TDIR`: base directory type.
- `baluaENV_register(L, MY_DIR_TYPE, BA_TDIR, myDirMethods)`: register derived directory metatable inheriting base methods.

The base directory API inherited from `BA_TDIR` includes:

- `dir:insert([child [, preserveChild]])`
- `dir:unlink()`
- `dir:name()`
- `dir:baseuri()`
- `dir:setfunc(func_or_nil)`
- `dir:service(request, relPath [, simulateForward])`
- `dir:setauth(authenticator [, authorizer])`
- `dir:p403(path)`
- `dir:redirect2tls()`

The BAS directory checker accepts either a directory userdata or a Lua table with a `dir` field containing the actual directory userdata. This supports Lua-level directory wrappers.

## Derived HttpDir Template

Use this pattern for new directory object bindings:

```c
#include <balua.h>
#include <HttpServer.h>

#define BA_TDIR_MYDIR MY_DIR_TYPE_ID

typedef struct {
   HttpDir super;
   int callbackRef;
   int stateRef;
} MyDir;

#define MyDir_check(L, ix) \
   ((MyDir*)(((LHttpDir*)baluaENV_checkudata(L, ix, BA_TDIR_MYDIR)) + 1))

static int
MyDir_service(HttpDir* super, const char* relPath, HttpCommand* cmd)
{
   MyDir* o = (MyDir*)super;

   if(cmd) {
      /* Serve request here, or call Lua through LThreadMgr/request env if needed. */
      HttpResponse_write(&cmd->response, "ok", 2, TRUE);
      return 0; /* stop directory search */
   }

   /*
      BAS may call service with cmd == NULL during cleanup in some service
      chaining paths. Restore or delegate according to your object's design.
   */
   return 0;
}

static int
MyDir_gc(lua_State* L)
{
   MyDir* o = MyDir_check(L, 1);
   LHttpDir* ldir = ((LHttpDir*)o) - 1;

   /*
      If you store Lua refs in the LHttpDir uservalue table, unref them before
      destroying the native object. If you store registry refs, luaL_unref them here.
   */
   (void)ldir;

   HttpDir_destructor((HttpDir*)o);
   return 0;
}

static int
MyDir_create(lua_State* L)
{
   const char* name = luaL_optstring(L, 1, "mydir");
   int prio = (int)luaL_optinteger(L, 2, 0);
   char* nameCopy;

   MyDir* o = (MyDir*)baluaENV_createDir(
      L, BA_TDIR_MYDIR, sizeof(MyDir) + strlen(name) + 1);
   memset(o, 0, sizeof(MyDir));

   nameCopy = (char*)(o + 1);
   strcpy(nameCopy, name);
   HttpDir_constructor((HttpDir*)o, nameCopy, (S8)prio);
   HttpDir_setService((HttpDir*)o, MyDir_service);

   return 1;
}

static const luaL_Reg myDirMethods[] = {
   {"__gc", MyDir_gc},
   {NULL, NULL}
};

static const luaL_Reg myCreateFuncs[] = {
   {"mydir", MyDir_create},
   {NULL, NULL}
};

static int
MyDir_register(lua_State* L)
{
   baluaENV_register(L, BA_TDIR_MYDIR, BA_TDIR, myDirMethods);
   return 0;
}

void
balua_mydir(lua_State* L)
{
   int status;
   int top = lua_gettop(L);

   balua_pushbatab(L);
   lua_pushcclosure(L, MyDir_register, 1);
   status = lua_pcall(L, 0, 0, 0);
   if(status)
      baFatalE(FE_BLUA_PANIC, status);

   lua_getglobal(L, "ba");
   lua_getfield(L, -1, "create");
   balua_pushbatab(L);
   luaL_setfuncs(L, myCreateFuncs, 1);
   lua_settop(L, top);
}
```

Important corrections for real code:

- Store variable-length names after the fixed struct, as shown above, unless the C type already owns stable name storage.
- If your derived object's first member is not exactly `HttpDir super`, it must still be layout-compatible with `HttpDir*` at the start.
- If your service callback calls Lua, use the request environment model: create or fetch the request command wrapper, push an error handler, push the Lua service function and request `_ENV`, save and restore the current directory/current LSP pathname fields, call with `lua_pcall`, then release the request command stack.
- If your service callback is C-only, do not create Lua state objects inside the hot path.

## Directory Lifetime and References

Directory objects often outlive the Lua stack frame that created them after insertion into the server tree. Preserve Lua values explicitly:

- For base `dir:insert(child, true)`, BAS keeps a parent-side reference to the child so the child is not collected.
- If a directory owns Lua callbacks or tables, store them in the `LHttpDir` uservalue table and keep integer refs in your C struct.
- If using ordinary registry refs, release them in `__gc`/`close`.
- If the directory has parent/child relationships, unlink before destruction if it may still be inserted.
- Use `HttpDir_isLinked()` to prevent double insertions.
- Call the matching destructor: `HttpDir_destructor`, `HttpResRdr_destructor`, `WebDAV_destructor`, or your derived destructor.

Some BAS implementations use private helper functions that store Lua references in the directory userdata table instead of the global registry. Do not assume these private helpers are exported. In portable binding code, either:

- Place callbacks in the userdata uservalue table with `balua_getuservalue()` and index them by known keys.
- If C job code must recover the callback without the userdata on the stack, create a `balua_wkRef` to the callback and use `balua_wkPush` in the job runner.
- Use `luaL_ref(L, LUA_REGISTRYINDEX)` only when a strong registry reference is deliberately required; release strong refs in `__gc`/`close`.

Prefer uservalue tables for ownership and BAS weak references for lookup from `LThreadMgr` jobs.

## Calling Lua from C Callbacks

Use `LThreadMgr` for C-to-Lua callbacks. Do not design alternate callback mechanisms for BAS bindings unless the user explicitly asks for a different model.

Required `LThreadMgr` pattern:

1. The event source records only plain C event data.
2. If the event source cannot safely lock the BAS dispatcher mutex, first move the event to a safe broker/worker context using the platform's queue or scheduler.
3. In the safe context, lock the BAS dispatcher mutex.
4. Allocate a `ThreadJob` with `ThreadJob_lcreate`.
5. Enqueue it with `LThreadMgr_run`.
6. Release the dispatcher mutex.
7. The `LThreadMgr` worker invokes the job callback with the mutex locked.
8. The job callback uses `tj->Lt`, pushes the Lua callback and arguments, and calls `lua_pcall` with the supplied message-handler index.

Design rules:

- Anchor the Lua callback for as long as native code can enqueue jobs that call it. Prefer storing it in the owning userdata's uservalue table and keeping a `balua_wkRef` integer in the native object for later recovery with `balua_wkPush`.
- Store only plain C event data in the `ThreadJob` struct. Do not store Lua stack indexes, Lua object pointers that are not separately anchored, or pointers to temporary strings.
- Copy string/binary payloads that must survive until the job runs, and free those copies in the job callback after `lua_pcall`.
- Use `tj->Lt` as the Lua state in the job callback.
- The job callback runs with the BAS mutex locked. It may use Lua C API.
- Install an error handler before `lua_pcall`.
- Keep callback stack cleanup explicit with `lua_settop(tj->Lt, 0)` or a known stack top after each event in a loop.
- Use a per-object `running`, `closed`, or equivalent flag to coalesce bursts and prevent duplicate jobs for the same object when events arrive faster than Lua can process them.
- On `lua_pcall` error, stop or close the owning object if continuing would repeatedly enqueue failing callbacks.
- Stop event production before destroying the owning userdata. Release `balua_wkRef` references with `balua_wkUnref` and clear uservalue-held callbacks during close/GC when appropriate.

Minimal shape:

```c
typedef struct {
   ThreadJob super;
   int callbackRef;
   int value;
   char* payload;
} MyJob;

static void
MyJob_run(ThreadJob* tj, int msgh, LThreadMgr* mgr)
{
   MyJob* job = (MyJob*)tj;
   lua_State* L = tj->Lt;

   balua_wkPush(L, job->callbackRef);
   lua_pushinteger(L, job->value);
   if(job->payload)
      lua_pushstring(L, job->payload);
   if(lua_pcall(L, job->payload ? 2 : 1, 0, msgh) != LUA_OK) {
      /* Error handler at msgh owns reporting. Keep stack cleanup explicit. */
      lua_settop(L, 0);
   }
   if(job->payload)
      baFree(job->payload);
}
```

Dispatch shape:

```c
MyJob* job = (MyJob*)ThreadJob_lcreate(sizeof(MyJob), MyJob_run);
if(!job)
   baFatalE(FE_MALLOC, 0);
job->callbackRef = owner->callbackRef;
job->value = value;
job->payload = payloadCopy;

ThreadMutex_set(dispatchMutex);
LThreadMgr_run(threadMgr, (ThreadJob*)job);
ThreadMutex_release(dispatchMutex);
```

Before using these skeletons, verify the exact `ThreadJob`, `ThreadJob_lcreate`, `LThreadMgr`, and `LThreadMgr_run` declarations in the target headers.

## Request/Response Bindings

Request and response in Lua are the same `BA_THTTPCMD` userdata wrapping `LHttpCommand`.

To accept request/response from Lua:

```c
HttpCommand* cmd = baluaENV_checkcmd(L, 1);
HttpRequest* req = &cmd->request;
HttpResponse* resp = &cmd->response;
```

This requires the binding function to be registered with the BA environment upvalue. Do not use `baluaENV_checkcmd` in a generic module function without `BA_ENV_IX`.

Request-scoped userdata becomes invalid after the request completes. `LHttpCommand_check` checks `cmd != NULL`; use it for all request/response methods.

When writing to response:

- Return `balua_pushstatus(L, HttpResponse_write(...))` when exposing status.
- Do not write after committed-state APIs reject the change.
- For filters or deferred responses, create userdata that owns the response buffer/filter and installs a proper `__gc`.

## IoIntf Bindings

`IoIntf` is the BAS filesystem/resource abstraction. Use it when binding code needs file access that works for disk, ZIP, VM I/O, or embedded resources.

Key APIs:

- `baluaENV_checkIoIntf(L, ix)` returns `IoIntf*` from a Lua I/O userdata.
- `balua_iointf(L, name, newio)` registers or retrieves a named persistent I/O interface.
- `balua_createiointf(L)` creates a Lua-owned dynamic I/O object and returns `IoIntf**` for initialization.
- `ba.openio(name)` fetches registered I/O by name.
- `ba.mkio(...)` creates duplicate/ZIP-derived I/O objects.

For directory iteration:

- Create iterator userdata with `BA_TDIRITER`.
- Store `IoIntf*` and `DirIntf*`.
- Return a closure whose upvalue is the iterator userdata.
- Close the `DirIntf` on end of iteration and in `__gc`.

## Installation Patterns

Core `ba` modules:

- `luaopen_ba()` creates the `ba` table and calls submodule open functions.
- Submodule functions usually push a table and the caller sets a field.
- `balua_newlib(L, funcs)` creates a table with BA table as upvalue.

External add-ons that extend `ba.create`:

1. Fetch the BA table from the registry with `balua_pushbatab(L)`.
2. Register any typed metatables with a closure using BA table as upvalue.
3. Fetch global `ba`.
4. Fetch `ba.create`.
5. Push BA table and `luaL_setfuncs(L, createFuncs, 1)`.
6. Restore the stack.

External add-ons that extend `ba` directly:

```c
lua_getglobal(L, "ba");
balua_pushbatab(L);
luaL_setfuncs(L, funcs, 1);
lua_pop(L, 1);
```

Use no upvalues only when the functions do not call `baluaENV_*`.

## Runtime Integration: Custom BAS

Use this section when the user owns the BAS startup code instead of using Mako or Xedge.

Install custom bindings after `balua_create` has created the Lua state and after any BAS libraries the binding depends on have been installed. Install the binding before executing Lua startup scripts, `.config`, application scripts, or user code that should see the new API.

Typical custom BAS startup order:

1. Construct the dispatcher mutex, socket dispatcher, `HttpServer`, optional `BaTimer`, and any required I/O interfaces.
2. Initialize `BaLua_param` with at least VM I/O and the `HttpServer`; include `BaTimer` when timer APIs or time-based Lua services are needed.
3. Call `balua_create(&param)` once and use that Lua state for BAS.
4. Install required BAS Lua libraries such as I/O interfaces, HTTP client, socket, crypto, trace logger, or other runtime libraries.
5. Construct `LThreadMgr` before installing bindings that use C-to-Lua callbacks, trace logging, cosockets, or thread jobs.
6. Call binding install functions such as `balua_myfeature(L)`.
7. Lock the dispatcher mutex before running Lua startup/config code if the runtime follows the standard BAS dispatcher model.
8. Start the HTTP command pool or enable the runtime's `LThreadMgr` HTTP-pool path.
9. Release the dispatcher mutex and run the dispatcher.
10. On shutdown, lock the mutex, run Lua unload hooks, destroy thread pools, terminate sessions, destroy timers, call `balua_close`, then destroy I/O/server/dispatcher/mutex objects.

Rules:

- Do not create a second Lua state for a BAS binding. Use the `lua_State*` returned by `balua_create`.
- Do not install BAS-aware bindings before `balua_create`; they need the BA registry table and runtime parameter object.
- If the binding needs `LThreadMgr`, either install it after the runtime constructs `LThreadMgr` or pass/store a pointer through a runtime-specific context that is valid until shutdown.
- If the binding needs an `IoIntf`, register or pass an existing runtime I/O object. Do not assume disk I/O exists in embedded builds.
- Keep install functions stack-balanced and idempotent where practical.

## Runtime Integration: Mako

Use this section when the user wants to add a binding to Mako Server, not just design the binding itself.

For Mako-specific hook declarations, check [`examples/MakoServer/src/mako.h`](examples/MakoServer/src/mako.h). Use the real `MakoOpenAUX` declaration from that header. Do not recreate the struct in generated code.

Mako startup installs bindings in this general order:

1. Create `SoDisp`, `HttpServer`, `BaTimer`, VM I/O, and the Lua state with `balua_create`.
2. Register standard I/O handles such as `net`, `disk`, and optionally `home`.
3. Install standard BAS Lua libraries such as HTTP client, `LThreadMgr`, socket, SharkSSL, crypto, trace logger, SQL, Lua I/O, and optional static modules.
4. Lock the dispatcher mutex.
5. Run `.openports`, adjust process user when configured, then run `.config`.
6. Start the HTTP command pool and release the mutex before `SoDisp_run`.
7. On shutdown, lock the mutex, run Lua `onunload`, destroy thread pools, terminate sessions, destroy timer, close Lua, and then destroy I/O/server/dispatcher objects.

Preferred Mako binding integration choices:

- If the binding must be available to `.config` or application startup code, install it before `.config` runs.
- If the binding is a normal Lua/BAS extension, expose an install function such as `balua_myfeature(lua_State* L)` and call it from the selected Mako hook.
- If the binding needs Mako extension context such as disk/home I/O or TPM secret integration, use `makoOpenAUX(MakoOpenAUX* aux)` and the fields provided by [`examples/MakoServer/src/mako.h`](examples/MakoServer/src/mako.h).
- If the binding only adds simple Lua functions and does not need Mako auxiliary context, it can be installed by calling its install function directly with the main `lua_State*`.
- Keep install functions stack-balanced. Save `int top = lua_gettop(L)` on entry and restore it before return unless the function explicitly returns values.

Mako also supports source-inclusion hooks through C header fragments ending in `.ch`. Treat these as compile-time insertion points in Mako's C source, not as Lua modules:

- `examples/MakoServer/src/MakoExt1.ch`: included before `MakoExt2.ch`; use for early compile-time definitions, declarations, or includes that must be visible to `MakoExt2.ch`.
- `examples/MakoServer/src/MakoExt2.ch`: the main customization file. Use it to set compile-time options, include binding headers, declare binding install functions, and implement extension hooks such as `makoOpenAUX`. It also contains the legacy `AUX_LUA_BINDINGS` / `myCustomBindings(lua_State* L)` pattern.
- `examples/MakoServer/src/MakoExtM1.ch`: included early inside `runMako()`, before most startup work. Use only for startup customization that does not require the Lua state.
- `examples/MakoServer/src/MakoExtM2.ch`: included after `.openports` and user switching, immediately before `.config`. This is a good place to call `balua_myfeature(L)` when the binding must be available to `.config`.
- `examples/MakoServer/src/MakoExtM3.ch`: included after the dispatcher mutex is released and just before `SoDisp_run`. Use for last-moment non-Lua startup hooks; do not use Lua C API here unless the mutex state is handled correctly.
- `examples/MakoServer/src/MakoExtM4.ch`: included during shutdown after the dispatcher mutex is locked and before Lua `onunload`. Use only for coordinated shutdown work that must happen before Lua app unload.

When using `.ch` hooks, prefer a small hook body that calls a normal binding installer implemented in a `.c` file. Example shape:

```c
/* In a Mako .ch hook selected by startup timing */
extern void balua_myfeature(lua_State* L);
balua_myfeature(L);
```

Use `myCustomBindings(L)` only for legacy or very small integrations. The Mako source marks this hook as deprecated in favor of the newer auxiliary extension path. If using it anyway, define `AUX_LUA_BINDINGS`, keep the function stack-balanced, and call normal installer functions from it.

## Runtime Integration: Xedge

Use this section when the user wants to add a binding to Xedge, not just design the binding itself. Keep this guidance platform-neutral; do not include board, chip, or port-specific details unless the user explicitly asks.

For Xedge-specific hook declarations, check [`examples/xedge/src/xedge.h`](examples/xedge/src/xedge.h). Use the real `XedgeOpenAUX` declaration from that header. Do not recreate the struct in generated code.

Xedge startup installs bindings in this general order:

1. Create `SoDisp`, `HttpServer`, `NetIo`, and `BaTimer`.
2. Initialize `BaLua_param` with VM I/O, server, timer, and optional ZIP signing/encryption settings.
3. Create the Lua state with `balua_create`.
4. Install optional I/O and BAS Lua libraries such as `net`, HTTP client, `LThreadMgr`, socket, SharkSSL, crypto, trace logger, Lua I/O, optional disk I/O, tracker, token generator, and optional static modules.
5. Lock the dispatcher mutex.
6. Run `.config` with `balua_loadconfigExt`; `.config` must return a start function and an `onunload` function.
7. Keep registry references to those two functions.
8. Configure the HTTP thread pool. When the runtime uses `LThreadMgr` as the HTTP pool, enable it with the runtime's `LThreadMgr_enableHttpPool` path.
9. Call `xedgeOpenAUX(XedgeOpenAUX* aux)` before signaling the Lua start function.
10. Signal Lua startup, release the mutex, and run the dispatcher.
11. On shutdown, lock the mutex, call `onunload`, destroy `LThreadMgr` and any HTTP pool, terminate sessions, destroy timer, close Lua, and then destroy I/O/server/dispatcher objects.

Preferred Xedge binding integration choices:

- Put product/runtime bindings in `xedgeOpenAUX`. This hook runs after the BAS Lua environment and standard libraries are installed, after `.config` has produced startup functions, and before the server is started.
- Expose an install function such as `balua_myfeature(lua_State* L)` and call it as `balua_myfeature(aux->L)` from `xedgeOpenAUX`.
- If the binding needs disk I/O or persistent configuration integration, use only the fields and callback hooks provided by `XedgeOpenAUX` in [`examples/xedge/src/xedge.h`](examples/xedge/src/xedge.h).
- If the binding supplies asynchronous C-to-Lua callbacks, use the runtime's `LThreadMgr`; Xedge already constructs it before `xedgeOpenAUX` runs.
- Do not open server sockets directly from the binding during startup. Xedge's `.config` owns server start/listen setup.
- Keep `xedgeOpenAUX` stack-balanced and return `0` on success. Return nonzero only for startup-fatal binding installation failures.

## Choosing a Type ID

If modifying the BAS source tree, add a new ID to `enum BaUserDataTypes` in `inc/balua.h` when the object must use BAS typed userdata or inherit from an existing BAS type.

If writing a loadable/external add-on and cannot modify `enum BaUserDataTypes`, do not use `baluaENV_register` unless the runtime reserves an extension ID. Use `luaL_newmetatable` for generic userdata, or expose native objects through existing BAS types such as `BA_TDIR` only if no new metatable is needed.

For derived `HttpDir` objects, a real BAS type ID is normally required so the object can inherit `BA_TDIR` methods and be accepted by `baluaENV_checkudata` as a directory.

## Design Checklist

Before coding:

- Define the Lua API first: module table, constructor name, methods, return conventions, ownership, close behavior, callback model, and whether it belongs under `ba`, `ba.create`, or `require`.
- Decide implementation language: C or C++.
- Decide generic vs BAS-aware vs directory object.
- Identify all C or C++ objects requiring constructor/destructor.
- Identify all blocking operations and exact mutex release windows.
- Identify all Lua callbacks and how they are anchored.
- Identify all request-scoped values and avoid storing them beyond the request.
- Identify all child/parent references for directory objects.
- Decide whether errors are programmer errors (`luaL_error`) or runtime status (`nil, err, code`).
- For C++, decide C-linkage entry points, exception boundaries, placement-new userdata storage, and destructor behavior before coding.

During implementation:

- Use BAS-style names: `LType_method`, `Type_check`, `luaopen_ba_x`, `balua_x`.
- Set `__index` to the metatable for object methods.
- Include `__gc`; include `__close` when deterministic close makes sense.
- Initialize userdata to a harmless closed/zero state before constructors that can fail.
- Set the metatable immediately after userdata allocation when possible.
- Anchor callbacks before registering C callbacks with the dispatcher or external systems.
- Unregister dispatcher connections before destructing their native backing storage.
- Reacquire the mutex before returning to Lua.
- Keep stack comments around complex callback setup.
- For C++, catch exceptions at Lua/BAS boundaries and do not throw from callbacks, `__gc`, or `__close`.

After implementation:

- Add the install call at the correct runtime startup point.
- Verify the binding loads in the target runtime.
- Exercise constructor, each method, invalid argument handling, close/GC, and repeated close.
- For blocking calls, verify concurrent HTTP requests still proceed.
- For directory objects, verify insert, request service, child insert, unlink, GC after unlink, and duplicate insert rejection.
- For callbacks, verify callback error reporting and cleanup after error.
- For request bindings, verify saved request objects fail cleanly after request completion.

## Common Pitfalls

- Registering a function with `luaL_setfuncs(..., 0)` and then calling `baluaENV_getparam` or `baluaENV_checkudata`; these require the BA table upvalue.
- Releasing the dispatcher mutex and then using Lua API before reacquiring it.
- Storing pointers to Lua string memory across a mutex release or after the stack value can be collected; copy the data if it must outlive the call.
- Forgetting to anchor Lua callback functions or threads used by C callbacks.
- Forgetting to restore `curDir` or `curLspPathname` around directory callback calls.
- Destroying a linked `HttpDir` without unlinking or coordinating references.
- Using `luaL_checkudata` for a BAS typed userdata that participates in inheritance.
- Returning raw BAS negative/positive status codes directly instead of `balua_pushstatus`.
- Making request/response userdata global and using it after request teardown.
- Calling `HttpResponse_*` APIs after the response is committed without checking the committed-state rules for that response operation.
- Creating a dynamic I/O object and forgetting whether the Lua object owns and frees the `IoIntf`.
- Letting a C++ exception escape through Lua C API, BAS callbacks, `HttpDir` service, `ThreadJob`, `__gc`, or `__close`.
- Storing a non-trivial C++ object in userdata and using `memset`, `memcpy`, or `baFree` instead of placement `new` plus explicit destructor.
- Exporting a C++ binding install function without `extern "C"` when it is called by C, Lua, Mako, Xedge, or a C-oriented build system.
- Using a non-static C++ member function in `luaL_Reg`.

## Minimal Derived Directory API Shape

Recommended Lua shape for new BAS directories:

```lua
local d = ba.create.mydir("name", {
   option = true,
   onevent = function(dir, request, relpath) end,
})

d:insert()
d:unlink()
```

Constructor guidance:

- Accept `name` as optional first argument; default to `""` or a clear service name.
- Accept priority as an optional number if ordering matters.
- Accept options in a table for non-trivial APIs.
- Accept callbacks as named table fields, not positional arguments, once there is more than one callback.
- Return the directory userdata directly so inherited `BA_TDIR` methods work.

Service callback guidance:

- Return `0` from C service when the request is handled and directory search should stop.
- Return non-zero only when BAS should keep searching/delegate according to existing `HttpDir` semantics.
- For Lua service functions, mirror `dir:setfunc`: if Lua returns boolean `false` and no abort/redirect occurred, delegate to original service when one exists.

## Documentation Expectations

Document every binding with:

- Lua constructor and method signatures.
- Return values including `nil, err, code`.
- Ownership: who closes/frees what.
- Threading: whether calls may block and whether the mutex is released.
- Callback context: request thread or `LThreadMgr` job context.
- Runtime support: target runtime limitations and compile-time macros.
- Directory behavior: insertion, priority, auth integration, child directory support, and unlink requirements.

## Review Questions for an AI Agent

When reviewing or generating a BAS Lua binding, answer these before finalizing:

1. Does every function that uses BAS state have the BA environment upvalue?
2. Are all userdata types checked with the correct checker?
3. Are metatable inheritance and type IDs correct?
4. Is every C or C++ constructor paired with a destructor in `__gc`/`close`?
5. Can `__gc` safely run on a partially constructed object?
6. Are callbacks and Lua threads anchored for the full native-side lifetime?
7. Are registry refs/uservalue refs released?
8. Are blocking calls wrapped by mutex release/reacquire without Lua API usage in between?
9. Does the binding avoid keeping request-scoped objects after request completion?
10. Do directory objects use `baluaENV_createDir`, inherit `BA_TDIR`, and call the correct `HttpDir` constructor/destructor?
11. Does the code restore global or per-request state after callbacks?
12. Does the return shape match BAS conventions?
13. For C++, are exported entry points C-compatible, are exceptions contained, and are userdata objects constructed/destructed with placement `new` and explicit destructor calls?
