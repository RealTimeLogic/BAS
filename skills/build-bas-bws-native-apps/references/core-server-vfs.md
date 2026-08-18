# Core Server and Virtual Filesystem

## Contents

- [Construction order](#construction-order)
- [Minimal C server](#minimal-c-server)
- [Minimal C++ server](#minimal-c-server-1)
- [VFS nodes and mounting](#vfs-nodes-and-mounting)
- [Directory and page callbacks](#directory-and-page-callbacks)
- [Request and response handling](#request-and-response-handling)
- [Static resources](#static-resources)
- [Generated CSP and embedded resources](#generated-csp-and-embedded-resources)
- [Lifetime checklist](#lifetime-checklist)

## Construction order

Create objects in this order:

1. `ThreadMutex`
2. `SoDisp`
3. `HttpServerConfig`; apply all sizing/configuration setters
4. `HttpServer`
5. optional `HttpCmdThreadPool`
6. HTTP/HTTPS listeners
7. persistent VFS nodes and backing I/O
8. dispatcher loop

`HttpServerConfig_setNoOfHttpCommands` controls concurrent command objects. Size the command thread pool consistently. `HttpServerConfig_setNoOfHttpConnections` controls HTTP engine sockets, not every socket sharing the dispatcher. Check the TCP stack and dispatcher limit before increasing it.

Construct once unless the application has a deliberate teardown/restart owner. Static constructors called repeatedly corrupt lifecycle assumptions.

## Minimal C server

```c
static HttpServer*
createServer(void)
{
   HttpServerConfig cfg;
   static ThreadMutex mutex;
   static SoDisp dispatcher;
   static HttpServer server;
   static HttpCmdThreadPool pool;

   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&dispatcher, &mutex);
   HttpServerConfig_constructor(&cfg);
   HttpServerConfig_setNoOfHttpCommands(&cfg, 3);
   HttpServerConfig_setNoOfHttpConnections(&cfg, 12);
   HttpServerConfig_setRequest(&cfg, 2 * 1024, 8 * 1024);
   HttpServerConfig_setResponseData(&cfg, 8 * 1024);
   HttpServer_constructor(&server, &dispatcher, &cfg);
   HttpCmdThreadPool_constructor(
      &pool, &server, ThreadPrioNormal, BA_STACKSZ);
   openSocketCon(&server, &dispatcher); /* HLOS helper, when applicable */
   return &server;
}

void barracuda(void)
{
   HttpServer* server = createServer();
   installVirtualDir(server);
   SoDisp_run(HttpServer_getDispatcher(server), -1);
}
```

For a nonblocking simulator loop, call `SoDisp_run(dispatcher, 0)` and yield/sleep according to the platform. Do not use polling by default on a real target.

## Minimal C++ server

Ensure config setters run before the static server constructor:

```cpp
static HttpServer* createServer()
{
   static bool initialized = false;
   static ThreadMutex mutex;
   static SoDisp dispatcher(&mutex);
   static HttpServerConfig cfg;
   static HttpServer* server = 0;
   static HttpCmdThreadPool* pool = 0;

   if(!initialized)
   {
      cfg.setNoOfHttpCommands(3);
      cfg.setNoOfHttpConnections(12);
      cfg.setRequest(2 * 1024, 8 * 1024);
      cfg.setResponseData(8 * 1024);
      static HttpServer serverStorage(&dispatcher, &cfg);
      static HttpCmdThreadPool poolStorage(
         &serverStorage, ThreadPrioNormal, BA_STACKSZ);
      server = &serverStorage;
      pool = &poolStorage;
      openSocketCon(server, &dispatcher);
      initialized = true;
   }
   return server;
}

extern "C" void barracuda(void)
{
   HttpServer* server = createServer();
   installVirtualDir(server);
   server->getDispatcher()->run();
}
```

If the target forbids function-local static initialization guards or dynamic C++ runtime support, use application-owned static storage with explicit C constructors.

## VFS nodes and mounting

Core VFS types:

- `HttpDir`: named directory, unnamed fallback/filter, or parent node.
- `HttpPage`: named leaf callback.
- `HttpResRdr`: read-only resources over `IoIntf`; inherits `HttpDir` semantics.
- `HttpResMgr`: managed read/write resources over `IoIntf`.
- `WebDAV`: WebDAV methods over writable `IoIntf`.
- generated CSP tree: generated pages/directories installed through `HttpServer_insertCSP`.

Mount APIs:

```c
HttpServer_insertRootDir(server, &rootDir);
HttpServer_insertDir(server, "api", &v1Dir);
HttpDir_insertDir(&parentDir, &childDir);
HttpDir_insertPage(&parentDir, &page);
HttpResRdr_insertPrologDir(&resources, &gateDir);
```

`HttpServer_insertDir(server, virtualDirRootPath, dir)` combines the root path and the directory's own name. A directory named `v1` inserted at `api` mounts at `/api/v1/`. Insert a named directory at root with `HttpServer_insertRootDir` or a root/null path. Do not mount an unnamed directory beneath a non-root path.

Names are retained by pointer. Use string literals or persistent memory.

Call `HttpDir_unlink` or `HttpPage_unlink` before destroying a linked node. Use `HttpDir_getRootPath` or `HttpDir_makeAbsPath` for mount-safe generated paths; release returned allocated paths with `baFree` when the header requires it.

## Directory and page callbacks

The directory callback is:

```c
typedef int (*HttpDir_Service)(
   HttpDir* dir, const char* relPath, HttpCommand* cmd);
```

Return `0` when handled. Return `-1` when this node did not handle the relative path and VFS lookup must continue.

```c
static int apiService(HttpDir* dir, const char* rel, HttpCommand* cmd)
{
   HttpResponse* resp;
   if(strcmp(rel, "status") != 0)
      return -1;
   resp = HttpCommand_getResponse(cmd);
   HttpResponse_setStatus(resp, 200);
   HttpResponse_setContentType(resp, "application/json; charset=UTF-8");
   HttpResponse_write(resp, "{\"ok\":true}", 11, TRUE);
   return 0;
}
```

Create persistent nodes:

```c
static HttpDir api;
HttpDir_constructor(&api, "api", 0);
HttpDir_setService(&api, apiService);
HttpServer_insertRootDir(server, &api);
```

`HttpPage_Service` receives `HttpPage*`, `HttpRequest*`, and `HttpResponse*` and does not use the directory callback's integer search contract. Check `HttpServer.h` for the exact typedef in the target version.

Use priority only when duplicate names or deliberate filter/fallback ordering require it. The documented C/C++ range is `-14` through `14`; higher priority is searched first.

## Request and response handling

Obtain request/response from `HttpCommand`:

```c
HttpRequest* req = HttpCommand_getRequest(cmd);
HttpResponse* resp = HttpCommand_getResponse(cmd);
```

Common request operations live in `HttpServer.h`: method type/name, headers, parameters, cookies, session, authenticated user, connection, and server. Verify target-version macros/functions before use.

Response sequence:

1. validate method, path, headers, and body limits;
2. set status;
3. set content type and any headers;
4. write/send/encode body;
5. flush/finalize the selected encoder if required.

Use `HttpResponse_forward`/`include` only for server-side VFS resources. A forward is not a browser redirect. Use redirect APIs for a client-visible redirect.

Do not save request/response pointers, header pointers, parameter pointers, or `relPath` beyond the callback.

## Static resources

Disk-backed C pattern:

```c
static DiskIo io;
static HttpResRdr resources;
DiskIo_constructor(&io);
if(DiskIo_setRootDir(&io, "html"))
   baFatalE(FE_USER_ERROR_1, 0);
HttpResRdr_constructor(&resources, (IoIntf*)&io, 0, 0, 0);
HttpServer_insertRootDir(server, (HttpDir*)&resources);
```

Embedded ZIP pattern:

```c
extern ZipReader* getHtmlZipReader(void);
static ZipIo io;
static HttpResRdr resources;
ZipReader* reader = getHtmlZipReader();
if(!CspReader_isValid((CspReader*)reader))
   baFatalE(FE_USER_ERROR_2, 0);
ZipIo_constructor(&io, reader, 0, 0);
if(ZipIo_getECode(&io) != ZipErr_NoError)
   baFatalE(FE_USER_ERROR_3, 0);
HttpResRdr_constructor(&resources, (IoIntf*)&io, 0, 0, 0);
```

Insert a prologue directory when a gate/filter must run before static lookup. Insert normal child pages/directories on `HttpResRdr` for dynamic handling after static resources miss.

`HttpResRdr` treats `.shtml` resources as hidden from direct client requests; they remain available to server-side `HttpResponse_forward` and `HttpResponse_include`. Custom `IoIntf` implementations can expose the header-defined `hidden` property for other resources. Use those contracts instead of inventing a filename convention, and never redirect a browser to a hidden resource.

## Generated CSP and embedded resources

Generated declarations commonly look like:

```c
extern void httpInitGeneratedCode(HttpDir*, CspReader*);
extern CspReader* getCspReader(void);
extern ZipReader* getHtmlZipReader(void);
```

Validate `CspReader` and `ZipReader`, construct backing I/O, then call:

```c
HttpServer_insertCSP(server, httpInitGeneratedCode, "/", cspReader);
```

Treat these functions as generated build outputs. Do not recreate their data or signatures manually. Include the generated C sources in the final firmware/link target.

For a custom file/device-backed generated reader, retain the backing resource for the reader's lifetime and close it in the reader destructor. Mark a `CspReader` valid with `CspReader_setIsValid` only after opening and minimally validating its backing data. Follow the reader callback's documented success/error convention rather than translating standard C I/O return values by assumption.

When an embedded ZIP is optional, guard both the generated data and the mounting code with one application feature macro. Keeping a committed generated C file as an empty translation unit when the macro is disabled lets development builds and Visual Studio projects list one stable source set without embedding the UI. Keep the generated reader, `ZipIo`, and `HttpResRdr` alive for as long as the mount is active.

## Lifetime checklist

- Keep mutex before dispatcher, dispatcher before server, server before mounted nodes.
- Keep listener and command pool alive while the dispatcher runs.
- Keep `IoIntf` alive longer than every resource manager/reader using it.
- Keep authenticators, user databases, login callbacks, and authorizers alive longer than protected directories.
- Unlink dynamic nodes before destruction.
- Match `baMalloc/baFree`, C constructors/destructors, and `new/delete` exactly.
- Make repeated start/stop an explicit lifecycle design; do not rerun one-shot static constructors.
