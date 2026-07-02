# BAS/BWS C and C++ Skill

Use this skill to write native C or C++ code against the public BAS/BWS amalgamated package.

BAS and BWS share the same low-level C/C++ server primitives. BWS is the compact HTTP(S)/WebSocket server library. BAS includes BWS and adds Lua/LSP/application-server features. For Lua binding design, switch to the public Lua binding skill:

https://realtimelogic.com/downloads/ai-skills/Lua-Binding-Skill.md

For Lua-side VFS design in BAS-derived runtimes, the related public skill is:

https://realtimelogic.com/downloads/ai-skills/VFS-skill.md

## Operating Procedure

Start by classifying the request:

1. Runtime: BWS, BAS without Lua, BAS with Lua/LSP, Mako, Xedge, or a custom embedded integration.
2. Language: C or C++.
3. Service shape: REST/JSON service, static resources, generated CSP/resources, upload endpoint, WebDAV, outbound HTTP(S) client, WebSocket service, socket takeover, custom `HttpDir`, custom `HttpPage`, `IoIntf`, startup/porting code, or build integration.
4. Target: HLOS host process, embedded Linux, RTOS task/thread, or firmware with no filesystem.
5. Resources: disk files, embedded ZIP, generated C arrays, or no static resources.
6. Blocking behavior: file I/O, socket writes, hardware calls, DNS/network operations, long computations, or worker threads.
7. Thread ownership: dispatcher thread only, HTTP command pool, application worker thread, or ISR/RTOS callback.

Before coding, state the chosen pattern and the files or headers that make it valid.

## Implementation Loop

When asked to create an app, run this loop:

1. Classify the app using the operating-procedure questions. If the user specified only an app type, choose the closest recipe below and state assumptions.
2. Choose `src/BWS.c` for native C/C++ only, or `src/BAS.c` when Lua/LSP/Mako/Xedge/BAS-only APIs are required.
3. Create a minimal source shape: startup/server creation, one `installVirtualDir(HttpServer*)` module, app-specific service modules, and optional resource packaging.
4. Keep all VFS objects and backing objects in static storage, application-owned heap storage, or another lifetime that outlives the dispatcher loop.
5. Implement the smallest working route first, then add authentication, uploads, WebSocket broadcast, generated resources, or WebDAV.
6. Compile and fix against the public headers. Do not recreate structs or callback signatures from memory.
7. Smoke test each mounted URL or protocol endpoint with a real HTTP/WebSocket/client request when the environment can run it.

Ask only when a missing answer changes architecture: C or C++, BAS or BWS, target OS/RTOS, filesystem availability, TLS requirement, authentication requirement, or whether Lua/LSP is expected. Otherwise choose conservative defaults and continue.

## App Type Recipes

Use this table to translate user intent into a BAS/BWS design:

| User asks for | Use this design | Main headers |
| --- | --- | --- |
| Static web site or packaged UI | `HttpResRdr` over `DiskIo`, `ZipIo`, generated embedded ZIP, or custom `IoIntf` | `HttpResRdr.h`, `BaDiskIo.h`, `ZipIo.h`, `IoIntf.h` |
| C/C++ server-side web app | `HttpResRdr` for assets plus `HttpDir` or `HttpPage` handlers for dynamic routes; use forwards/includes only through BAS/BWS response APIs | `HttpServer.h`, `HttpResRdr.h` |
| REST/JSON API | Named `HttpDir` such as `api`, method/path dispatch inside the callback, JSON parser/encoder for bodies | `HttpServer.h`, `HttpRecData.h`, `JParser.h`, `JEncoder.h`, `JVal.h` |
| WebSocket app | `HttpPage` or named `HttpDir` upgrade endpoint, one `WSS`/`WSSCB` object per client, optional worker thread for broadcast | `WebSocketServer.h`, `HttpServer.h`, `ThreadLib.h` |
| Upload app | Named `HttpDir` containing `HttpUpload`, completion/error callbacks, optional `HttpAsynchResp` worker | `HttpUpload.h`, `HttpAsynchResp.h`, `BaDiskIo.h` |
| File manager or WebDAV | `HttpResMgr` or `WebDAV` over writable `IoIntf`, protected by authenticator | `HttpResMgr.h`, `WebDAV.h`, `Authenticator.h` |
| Generated CSP/static package | Generated `httpInitGeneratedCode`, `CspReader`, optional generated `ZipReader`, then `HttpServer_insertCSP` or insert into `HttpResRdr` | `CspRunTm.h`, `HttpServer.h`, `ZipIo.h` |
| Outbound HTTP(S) client app | `HttpClient` with `SoDisp`; add `SharkSsl` client and CA list for HTTPS validation | `xrc/misc/HttpClient.h`, `SharkSSL.h`, `SoDisp.h` |
| Custom protocol after HTTP handshake | Socket takeover with `SoDispCon_moveCon`, worker thread, and explicit connection destruction | `SoDispCon.h`, `ThreadLib.h` |
| Lua/LSP app or Lua binding | Use BAS startup plus the public Lua binding skill; keep native C/C++ services in this skill only when they remain native HTTP/WebSocket/VFS code | `balua.h`, plus https://realtimelogic.com/downloads/ai-skills/Lua-Binding-Skill.md |

Default file shape for a new native app:

- `main.c` or target startup: calls `hostInit` if needed, then `barracuda()` from a dedicated thread or process main.
- `server.c` / `server.cpp`: constructs `ThreadMutex`, `SoDisp`, `HttpServerConfig`, `HttpServer`, optional `HttpCmdThreadPool`, listeners, and dispatcher loop.
- `app.c` / `app.cpp`: exports `installVirtualDir(HttpServer*)` and owns the persistent VFS objects.
- `app.h`: declares `installVirtualDir` with `extern "C"` guards when C++ is involved.
- `resources.c` or generated files: optional embedded ZIP/CSP data.

## Source Package Model

Choose one amalgamated library:

- Use `src/BWS.c` for BWS native HTTP(S), WebSocket, REST, and C/C++ web services.
- Use `src/BAS.c` for BAS builds that need Lua, LSP, Xedge, Mako, or BAS-only APIs.
- Do not compile both `src/BWS.c` and `src/BAS.c` into the same target.

Add porting layers:

- Thread/kernel layer: `src/arch/<OS>/ThreadLib.c`
- Socket dispatcher/TCP layer: `src/arch/NET/<NET>/SoDisp.c` or a combined OS-specific dispatcher where provided
- Optional filesystem layer: `src/DiskIo/<FS>/BaFile.c`

Include directories normally include:

- `inc`
- `inc/arch/<OS>`
- `inc/arch/NET/<NET>`
- Any filesystem or target-specific include directory required by the chosen `BaFile.c`

Use `examples/HostInit/Main.c` and `examples/HostInit/HostInit.c` as HLOS scaffolding only. For RTOS/firmware, create an equivalent system init path and run `barracuda()` from a dedicated task/thread.

## Header Map

Use public headers as the source of truth. Do not recreate structures or callback typedefs from memory.

- `inc/barracuda.h`: broad convenience include for many BWS/BAS primitives.
- `inc/HttpServer.h`: `HttpServer`, `HttpServerConfig`, `HttpDir`, `HttpPage`, `HttpCommand`, `HttpRequest`, `HttpResponse`, C++ wrappers.
- `inc/HttpServCon.h`: HTTP listening socket/server connection.
- `inc/HttpSharkSslServCon.h`: HTTPS listening socket/server connection.
- `inc/HttpCmdThreadPool.h`: HTTP command worker pool.
- `inc/SoDisp.h`: socket dispatcher and dispatcher mutex access.
- `inc/ThreadLib.h`: `ThreadMutex`, `Thread`, `ThreadSemaphore`, C++ `ThreadLock`, `ThreadReleaseLock`.
- `inc/HttpResRdr.h`: static resource reader for `IoIntf` trees.
- `inc/HttpResMgr.h`: read/write resource manager directory.
- `inc/IoIntf.h`: virtual filesystem/resource interface.
- `inc/BaDiskIo.h`: disk-backed `IoIntf`.
- `inc/ZipIo.h` and `inc/IoIntfZipReader.h`: ZIP-backed `IoIntf`.
- `inc/CspRunTm.h`: CSP runtime and generated page/resource integration.
- `inc/Authenticator.h`, `inc/BasicAuthenticator.h`, `inc/DigestAuthenticator.h`, `inc/FormAuthenticator.h`, `inc/AuthenticatedUser.h`: authentication, login response, user database, and authorization hooks.
- `inc/HttpUpload.h`, `inc/MultipartUpload.h`, `inc/HttpAsynchResp.h`: upload handling and deferred responses.
- `inc/WebDAV.h`: WebDAV directory object.
- `inc/WebSocketServer.h`: native WebSocket server `WSS` and `WSSCB`.
- `inc/SoDispCon.h`: active socket connection handoff and low-level connection I/O.
- `inc/NetIo.h`: HTTP-backed `IoIntf`, useful when a device fetches its resource tree from a development server.
- `inc/HttpRecData.h`: request body receiver for POST/PUT data.
- `inc/SharkSSL.h`: SharkSSL TLS client/server object used with HTTPS listeners and outbound HTTPS.
- `xrc/misc/HttpClient.h`: outbound HTTP/HTTPS client when the public package includes the XRC client helper.
- `inc/JEncoder.h`, `inc/JParser.h`, `inc/JVal.h`: JSON encoding/parsing.

## Minimal Server Pattern

The core startup pattern is the same in C and C++:

1. Construct `ThreadMutex`.
2. Construct `SoDisp` with the mutex.
3. Construct and tune `HttpServerConfig`.
4. Construct `HttpServer`.
5. Construct `HttpCmdThreadPool` when request handlers may block or when the example pattern uses a command pool.
6. Open HTTP/HTTPS listen sockets with `HttpServCon` / `HttpSharkSslServCon` or a helper such as `examples/HostInit/OpenSocketCon.h`.
7. Insert root directories/pages/services into the virtual file system.
8. Run the dispatcher with `SoDisp_run(&dispatcher, -1)` or `dispatcher.run()`.

Keep server, dispatcher, directories, resource readers, socket listeners, and thread pools alive for the full server lifetime. Static storage is common in examples because inserted directories and listeners must outlive the function that installs them.

Call a `createServer` helper like the examples below once. If an application may start/stop the server repeatedly, add an explicit initialized flag or construct/destruct the objects in an application-owned lifecycle object instead of re-running constructors on static C objects.

C skeleton:

```c
static HttpServer*
createServer(void)
{
   HttpServerConfig scfg;
   static ThreadMutex mutex;
   static SoDisp dispatcher;
   static HttpServer server;
   static HttpCmdThreadPool pool;

   ThreadMutex_constructor(&mutex);
   SoDisp_constructor(&dispatcher, &mutex);
   HttpServerConfig_constructor(&scfg);
   HttpServerConfig_setNoOfHttpCommands(&scfg, 3);
   HttpServerConfig_setNoOfHttpConnections(&scfg, 12);
   HttpServerConfig_setRequest(&scfg, 2 * 1024, 8 * 1024);
   HttpServerConfig_setResponseData(&scfg, 8 * 1024);
   HttpServer_constructor(&server, &dispatcher, &scfg);
   HttpCmdThreadPool_constructor(&pool, &server, ThreadPrioNormal, BA_STACKSZ);
   openSocketCon(&server, &dispatcher);
   return &server;
}
```

C++ skeleton:

```cpp
static HttpServer*
createServer()
{
   static ThreadMutex mutex;
   static SoDisp dispatcher(&mutex);
   static HttpServerConfig scfg;

   scfg.setNoOfHttpCommands(3);
   scfg.setNoOfHttpConnections(12);
   scfg.setRequest(2 * 1024, 8 * 1024);
   scfg.setResponseData(8 * 1024);

   static HttpServer server(&dispatcher, &scfg);
   static HttpCmdThreadPool pool(&server, ThreadPrioNormal, BA_STACKSZ);
   openSocketCon(&server, &dispatcher);
   return &server;
}

extern "C" void
barracuda(void)
{
   HttpServer* server = createServer();
   installVirtualDir(server);
   server->getDispatcher()->run();
}
```

Use `extern "C"` for C++ entry points called by C startup code or RTOS task launch glue.

## HTTP Service Pattern

Use an `HttpDir` service for URL-tree services such as REST endpoints.

C pattern:

```c
static int
myService(HttpDir* dir, const char* path, HttpCommand* cmd)
{
   HttpRequest* req = HttpCommand_getRequest(cmd);
   HttpResponse* resp = HttpCommand_getResponse(cmd);

   if(strstr(path, "status") != path)
      return -1; /* Not handled by this directory */

   HttpResponse_setStatus(resp, 200);
   HttpResponse_setContentType(resp, "application/json; charset=UTF-8");
   HttpResponse_write(resp, "{\"ok\":true}", 11, TRUE);
   return 0; /* handled */
}

void
installVirtualDir(HttpServer* server)
{
   static HttpDir dir;
   HttpDir_constructor(&dir, "api", 0);
   HttpDir_setService(&dir, myService);
   HttpServer_insertRootDir(server, &dir);
}
```

Rules:

- Return `0` when the request was handled.
- Return a negative value when this directory does not handle the path and BAS/BWS should keep searching.
- Use persistent storage for inserted `HttpDir`, `HttpPage`, `HttpResRdr`, and backing `IoIntf` objects.
- Do not store `HttpCommand`, `HttpRequest`, or `HttpResponse` beyond the request.
- Set content type before writing response bodies.
- Use `HttpResponse_setStatus` for status codes and close/terminate the connection only for protocol or parse errors that require it.

## REST and JSON Pattern

Use the REST example as the model:

- C: `examples/C-RESTful-Service/src/RestService.c`
- C++: `examples/C-RESTful-Service/src/RestService.cpp`
- JSON helpers: `examples/C-RESTful-Service/src/RestJsonUtils.c`

For request JSON:

- Check content length or transfer encoding.
- Require `Content-Type: application/json` for JSON payloads.
- Use `HttpRecData` to read the body.
- Parse with `JParser` and `JParserValFact`.
- Destroy `JParser`, `HttpRecData`, and `JParserValFact` on all paths.
- On parse failure, terminate the connection with `HttpConnection_setState(..., HttpConnection_Terminated)` when appropriate.

For response JSON:

- Create `JErr`.
- Construct `JEncoder` over `HttpResponse_getWriter(resp)`.
- Use `JEncoder_set`, `JEncoder_beginArray`, `JEncoder_endArray`, object/array methods, or the C++ wrapper methods.
- Destroy the encoder to flush/finalize output.

In C++ wrappers, prefer small RAII helpers like the example `JsonResp : public JEncoder`, but keep ownership explicit and avoid throwing through service callbacks.

## C/C++ Virtual File System Model

Do not collapse VFS design into one large request handler. Build a directory tree from persistent `HttpDir`-derived objects and let the BAS/BWS VFS select the node.

Core C/C++ VFS node types:

- `HttpDir`: a directory node with child directories, pages, optional authenticator/authorizer, and an optional service callback.
- `HttpPage`: a named leaf page inserted into an `HttpDir`.
- `HttpResRdr`: a read-only resource reader backed by an `IoIntf`; it is also an `HttpDir`.
- `HttpResMgr`: a read/write resource manager backed by an `IoIntf`; it is also an `HttpDir`.
- `WebDAV`: a WebDAV directory backed by a writable `IoIntf`; it is also an `HttpDir`.
- Generated CSP trees: generated code inserts pages/directories under an `HttpDir` or via `HttpServer_insertCSP`.

Mounting APIs:

```c
HttpServer_insertRootDir(server, &rootDir);
HttpServer_insertDir(server, "api", &v1Dir);
HttpDir_insertDir(&parentDir, &childDir);
HttpDir_insertPage(&parentDir, &page);
HttpResRdr_insertPrologDir(&resources, &gateDir);
```

C++ wrappers use the same model:

```cpp
server->insertRootDir(&rootDir);
server->insertDir("api", &v1Dir);
parentDir.insertDir(&childDir);
parentDir.insertPage(&page);
resources.insertPrologDir(&gateDir);
```

`HttpServer_insertDir(server, virtualDirRootPath, dir)` combines the root path and the directory object's own name. For example, inserting a directory named `v1` with root path `api` creates `/api/v1/`. To mount a named directory at the server root, use `HttpServer_insertRootDir` or pass `NULL`/`"/"` as the root path. Do not insert an unnamed directory under a non-root `virtualDirRootPath`; the public header documents this as invalid.

Rules that prevent most VFS bugs:

- Keep every mounted `HttpDir`, `HttpPage`, `HttpResRdr`, `HttpResMgr`, `WebDAV`, authenticator, authorizer, and backing `IoIntf` alive until it is unlinked or the server stops.
- Do not pass temporary strings as directory or page names. `HttpDir` and `HttpPage` store the name pointer; use string literals or memory with persistent lifetime.
- Use `HttpDir_unlink` / `HttpDir::unlink` before destroying a directory that may still be mounted.
- Use `HttpDir_getRootPath` / `HttpDir::getRootPath` or `HttpDir_makeAbsPath` / `HttpDir::makeAbsPath` when generated URLs or forwards must be mount-safe. Free returned paths with `baFree`.
- Use named directories for independent sections such as `api`, `admin`, `dav`, `fs`, `smq`, or `ws`.
- Use an unnamed `HttpDir` (`name == NULL` or `""`) only as a root node, a chained fallback/filter node, or a deliberate sibling in the current VFS chain.

## VFS Search, Priority, and Callback Contract

`HttpDir` priority is used when duplicate directory names can match. Default priority is `0`; higher priority is searched first. The public header documents the C/C++ priority range as `-14` through `14`.

The C/C++ service callback is:

```c
typedef int (*HttpDir_Service)(HttpDir* dir, const char* relPath,
                               HttpCommand* cmd);
```

Callback return rules:

- Return `0` after the callback has handled the request.
- Return a negative value, normally `-1`, when this node did not find or handle the resource and the VFS should continue searching.
- Do not write a 404 just because this callback did not recognize `relPath`; return `-1` so lower-priority nodes, sibling roots, resource readers, pages, or fallback directories can still handle the request.
- If a filter rejects a request, send the rejection response and return `0`.
- If a filter approves a request and should let the next VFS node handle it, return `-1`.
- When `cmd == NULL`, the service may be running as part of destruction; destructor-aware services must clean up and return `0`.

`HttpResRdr` has a special search shape:

1. Authentication/authorization on the resource reader runs first.
2. Prologue directories inserted with `HttpResRdr_insertPrologDir` run before static resource lookup.
3. The `IoIntf` resource tree is searched and served if found.
4. If the resource is not found, `HttpResRdr` delegates to its original `HttpDir` service, so inserted pages, subdirectories, and unnamed fallback directories can run.

Use prologue directories for gates that must run before static resources in a specific subtree. Use normal `HttpDir_insertDir` on the `HttpResRdr` for epilogue/fallback dynamic handlers that should run only after static lookup misses.

## Static Resources and Dynamic Fallback

Use `HttpResRdr` to serve static content from any `IoIntf`:

- Disk: `DiskIo` from `inc/BaDiskIo.h`
- ZIP: `ZipIo` from `inc/ZipIo.h`
- Custom resources: implement `IoIntf`, `DirIntf`, and `ResIntf`

Pattern:

```cpp
static DiskIo io;
if(io.setRootDir("html"))
   baFatalE(FE_USER_ERROR_1, 0);

static HttpResRdr resources(&io, 0);
server->insertRootDir(&resources);
```

For firmware builds, prefer embedded ZIP resources when a filesystem is not guaranteed. Use the package tools or build scripts to convert a ZIP into a C source file that returns a `ZipReader*`, then construct `ZipIo` from that reader.

Insert dynamic pages or services into a resource reader when static files should handle most URLs and custom code should handle specific pages or upgrade endpoints.

C fallback-after-static pattern:

```c
static int
fallback(HttpDir* dir, const char* relPath, HttpCommand* cmd)
{
   if(!isDynamicRoute(relPath))
      return -1;
   HttpResponse_setContentType(&cmd->response, "text/plain");
   HttpResponse_write(&cmd->response, "dynamic", 7, TRUE);
   return 0;
}

static DiskIo io;
static HttpResRdr resources;
static HttpDir dynamicDir;

DiskIo_constructor(&io);
DiskIo_setRootDir(&io, "html");
HttpResRdr_constructor(&resources, (IoIntf*)&io, 0, 0, 0);
HttpDir_constructor(&dynamicDir, 0, 0);
HttpDir_setService(&dynamicDir, fallback);
HttpDir_insertDir((HttpDir*)&resources, &dynamicDir);
HttpServer_insertRootDir(server, (HttpDir*)&resources);
```

The unnamed fallback directory is called only after the resource reader and normal page lookup do not handle the request. If the fallback does not own the path, it returns `-1`.

High-priority filter/gate pattern:

```c
static int
gate(HttpDir* dir, const char* relPath, HttpCommand* cmd)
{
   if(isAllowed(cmd, relPath))
      return -1; /* continue to the protected resource */
   HttpResponse_setStatus(&cmd->response, 401);
   HttpResponse_write(&cmd->response, "Unauthorized", 12, TRUE);
   return 0;
}

static HttpDir gateDir;
HttpDir_constructor(&gateDir, 0, 10);
HttpDir_setService(&gateDir, gate);
HttpResRdr_insertPrologDir(&resources, &gateDir);
```

For authentication-backed protection, prefer `HttpDir_setAuthenticator` / `HttpResRdr_setAuthenticator` over ad hoc gate code.

Hidden resources:

- `HttpResRdr` treats `.shtml` files as hidden from direct client requests; they can still be used through `HttpResponse_forward` and `HttpResponse_include`.
- `IoIntf` supports a `hidden` property for resource implementations that expose hidden metadata. Use the header-defined property mechanism instead of inventing naming rules.
- Do not redirect the browser to hidden resources; forward/include them server-side.

## Generated CSP and Resource Trees

BAS/BWS applications commonly combine generated resources, CSP pages, and an `IoIntf` tree. Treat generated functions as declarations supplied by the build output, not APIs to recreate manually.

Generated CSP/resource integration usually has this shape:

```c
extern void httpInitGeneratedCode(HttpDir* parent, CspReader* reader);
extern CspReader* getCspReader(void);
extern ZipReader* getHtmlZipReader(void);

static CspReader* cspReader = 0;
cspReader = getCspReader();
if(!CspReader_isValid(cspReader))
   baFatalE(FE_INVALID_CSPREADER, 0);
HttpServer_insertCSP(server, httpInitGeneratedCode, "/", cspReader);
```

Use the generated function signatures and reader getters from the generated header or source. Common variants:

- Generated CSP data: `CspReader*` plus `httpInitGeneratedCode(HttpDir*, CspReader*)`.
- Generated embedded ZIP: `ZipReader*` used to construct a `ZipIo`.
- Disk-backed CSP or ZIP: a custom `CspReader` or `ZipReader` that reads from the device filesystem.
- Network-backed development tree: `NetIo` as the VM/resource `IoIntf`, then switch to ZIP or disk for deployment.

Always validate readers before inserting:

```c
if(!CspReader_isValid(cspReader))
   baFatalE(FE_INVALID_CSPREADER, 0);

ZipIo_constructor(&zipIo, zipReader, 0, 0);
if(ZipIo_getECode(&zipIo) != ZipErr_NoError)
   baFatalE(FE_USER_ERROR_3, 0);
```

When using a custom reader, implement the reader callback declared in the header, keep any backing file/resource handle in the reader object, and call `CspReader_setIsValid` only after the backing data has been opened and basic format checks pass. For disk ZIP readers, use the file size when constructing `ZipReader`, close the backing `ResIntfPtr` in the destructor, and return `0` for successful reads and a negative value for read/seek errors.

## Resource Managers and Authentication

Use `HttpResRdr` for read-only static resources and `HttpResMgr` when the directory must allow managed read/write access over an `IoIntf`.

Pattern:

```c
static DiskIo diskIo;
static HttpResMgr mgr;
static BasicAuthenticator authenticator;
static UserIntf users;
static LoginRespIntf loginResp;

DiskIo_constructor(&diskIo);
DiskIo_setRootDir(&diskIo, "/www");
UserIntf_constructor(&users, getPassword);
LoginRespIntf_constructor(&loginResp, loginService);
BasicAuthenticator_constructor(&authenticator, &users, "private", &loginResp);
HttpResMgr_constructor(&mgr, (IoIntf*)&diskIo, 3, "private", 0, 0);
HttpResMgr_setAuthenticator(&mgr, (AuthenticatorIntf*)&authenticator, 0);
HttpServer_insertRootDir(server, (HttpDir*)&mgr);
```

Authentication pieces:

- `UserIntf`: maps a user name to a password or digest material. Fill the fields specified by `inc/Authenticator.h` and related headers; for digest/HA1 flows, set the content type field as required by the header.
- `BasicAuthenticator`, `DigestAuthenticator`, or `FormAuthenticator`: choose one based on client and UX needs.
- `LoginRespIntf`: handles failed login, login-page forwarding, and error-page forwarding.
- `AuthorizerIntf`: optional authorization callback that checks the authenticated user, method, and relative path.

Authorizer rules:

- Return `FALSE` when no authenticated user is available for a restricted path.
- Compare the relative path against the most specific restricted prefixes first.
- Check the HTTP method as part of the authorization decision.
- Map user names to roles or permissions outside the callback, then keep the callback as a small policy check.

For form/basic/digest protected subtrees, install the authenticator on the directory that owns the protected path:

```c
HttpDir_constructor(&secureDir, "private", 0);
HttpDir_setAuthenticator(&secureDir, (AuthenticatorIntf*)&authenticator,
                         (AuthorizerIntf*)&authorizer);
HttpServer_insertRootDir(server, &secureDir);
```

Keep user databases, authenticators, authorizers, login response objects, and protected directories alive for the full server lifetime.

## Upload Pattern

Use `inc/HttpUpload.h` for upload endpoints. Model uploads as a custom `HttpDir` with an upload helper and an upload completion callback.

C++ shape:

```cpp
class UploadCb : public HttpUploadCbIntf {
public:
   UploadCb() : HttpUploadCbIntf(onFile, onError) {}
private:
   static void onFile(HttpUploadCbIntf*, HttpUploadNode*, BaBool completed);
   static void onError(HttpUploadCbIntf*, HttpUploadNode*, int ecode,
                       const char* msg);
};

class UploadDir : public HttpDir {
   UploadCb cb;
   DiskIo io;
   HttpUpload upload;
   static int service(HttpDir*, const char*, HttpCommand*);
public:
   UploadDir() : HttpDir("upload"), upload(&io, AllocatorIntf::getDefault(), &cb, 1) {
      io.setRootDir(".");
      setService(service);
   }
};
```

Service rules:

- Handle `GET` separately, typically by forwarding to an upload page or status page.
- For `POST` and `PUT`, delegate to `HttpUpload::service` or the C equivalent and return handled when it succeeds.
- Return a clear error such as `403` for invalid upload methods or paths.
- In error callbacks, use `HttpUploadNode::getResponse()` / C equivalent to set status, content type, and a short error body.

Do not perform long post-upload work in the upload callback while holding the request path. If the upload triggers firmware extraction, scanning, compression, or other slow work, copy any transient names/metadata you need, then start a worker thread. Use `HttpAsynchResp` when the worker must continue writing a response after the original request callback returns.

## WebDAV Pattern

Use `inc/WebDAV.h` when the application needs remote file management over HTTP methods such as `PROPFIND`, `PUT`, `DELETE`, and related WebDAV operations.

Pattern:

```cpp
static DiskIo io;
static WebDAV dav(&io, 3, "dav", ".davlocks");
static HttpDir root(NULL);
root.insertDir(&dav);
server->insertRootDir(&root);
```

Rules:

- Back WebDAV with a writable `IoIntf`, commonly `DiskIo`.
- Provide a lock directory relative to the `IoIntf` root.
- Use a lock directory unique to each `WebDAV` instance.
- Match `HttpServerConfig_setNoOfHttpCommands` and the command thread pool size to expected concurrent transfers.
- Protect WebDAV with an authenticator unless the deployment is deliberately anonymous and isolated.

## Outbound HTTP and HTTPS Client

Use the HTTP client helper when application code must call another HTTP(S) service from C or C++.

Core pieces:

- `SoDisp` and `ThreadMutex` are still required.
- Construct `HttpClient` with the dispatcher.
- For HTTPS, construct a `SharkSsl` client object, seed the SharkSSL RNG, set a CA list when validating certificates, and call `HttpClient_setSSL` / `HttpClient::setSSL`.
- Use `HttpClient_setAcceptTrusted(TRUE)` / `HttpClient::setAcceptTrusted(true)` before sending credentials or sensitive data.
- Call `HttpClient_close` between independent requests when not using persistent connections.
- Destroy the client before destroying the dispatcher or TLS objects.

C shape:

```c
static ThreadMutex mutex;
static SoDisp dispatcher;
static SharkSsl shark;
HttpClient httpc;

sharkssl_entropy(baGetUnixTime() ^ (ptrdiff_t)&httpc);
ThreadMutex_constructor(&mutex);
SoDisp_constructor(&dispatcher, &mutex);
SharkSsl_constructor(&shark, SharkSsl_Client, 5, 8000, 3072);
SharkSsl_setCAList(&shark, sharkSslCAList);
HttpClient_constructor(&httpc, &dispatcher, 0);
HttpClient_setSSL(&httpc, &shark);
HttpClient_setAcceptTrusted(&httpc, TRUE);
status = HttpClient_request(&httpc, HttpMethod_Get, url, 0, 0, 0, 0);
HttpClient_close(&httpc);
HttpClient_destructor(&httpc);
```

For persistent outbound connections, run the dispatcher, preferably from a dedicated thread, so peer closes are detected. For one-shot blocking fetches, the dispatcher may be constructed without entering a permanent dispatcher loop if the client API/header documents that usage.

## Socket Takeover Pattern

Use socket takeover only when intentionally morphing an HTTP request into a custom protocol or proprietary TCP service that is no longer an HTTP response. Prefer WebSocket for browser-compatible bidirectional protocols.

Pattern:

```c
typedef struct MyService {
   Thread thread;
   SoDispCon con;
} MyService;

static void
run(Thread* t)
{
   MyService* self = (MyService*)t;
   /* Use SoDispCon_blockRead / SoDispCon_sendData here. */
   SoDispCon_destructor(&self->con);
   Thread_destructor(&self->thread);
   baFree(self);
}

int
startMyService(HttpRequest* req)
{
   MyService* self = baMalloc(sizeof(MyService));
   SoDispCon* active = (SoDispCon*)HttpRequest_getConnection(req);
   Thread_constructor(&self->thread, run, ThreadPrioNormal, BA_STACKSZ);
   SoDispCon_moveCon(active, &self->con);
   self->con.dispatcher = active->dispatcher;
   Thread_start(&self->thread);
   return 0;
}
```

Rules:

- Start the service thread exactly once.
- After `SoDispCon_moveCon`, the original HTTP request path no longer owns normal HTTP response handling.
- Set the moved connection's dispatcher pointer as required by `SoDispCon` so read/write calls can coordinate with the dispatcher mutex.
- Destroy the moved connection and thread in the worker before freeing the service object.
- Never keep `HttpRequest` or `HttpResponse` pointers in the worker; copy only data needed after the request callback returns.

## WebSocket Pattern

Use `inc/WebSocketServer.h` and `examples/C-WebSockets/src/ChatServer.cpp` as the model.

Core pieces:

- `WSSCB`: callback interface for frame, close, and optional ping events.
- `WSS`: one WebSocket connection.
- `HttpPage` or `HttpDir` service: detects WebSocket upgrade request.
- `HttpRequest::wsUpgrade` or C equivalent performs the HTTP upgrade.
- `WSS::connect` / `WSS_connect` morphs the HTTP connection into a WebSocket connection after successful handshake.

Rules:

- Allocate one connection object per client.
- Delete/free the connection from the close callback after removing it from any application lists.
- `WSS::write` may block and may release the dispatcher mutex.
- When broadcasting to many clients, queue messages and send from a worker thread instead of blocking the receive callback.
- Protect shared connection lists with the dispatcher mutex. In C++ use `ThreadLock`; in C use `ThreadMutex_set` / `ThreadMutex_release`.
- If a write releases the mutex, assume connection lists may change while the mutex is released; restart or validate iteration state.

## Threading and Mutex Rules

The dispatcher mutex protects server state. Request service callbacks normally run with the dispatcher/server context already coordinated by the framework.

Use these rules:

- Do not access server connection lists, directory trees, WebSocket objects, or shared server structures from another thread without locking the dispatcher mutex.
- In C, use `ThreadMutex_set` and `ThreadMutex_release`.
- In C++, use `ThreadLock` for scoped locking.
- In C++, use `ThreadReleaseLock` to release a locked mutex around blocking non-server work inside a request/service callback.
- Do not call BAS/BWS APIs that require the dispatcher context while the mutex is released.
- Keep ISR callbacks and low-level RTOS callbacks out of BAS/BWS APIs; queue work to a normal task/thread first.

## C vs C++ Style

The public headers expose C functions plus C++ wrappers. Pick one style per module where possible.

C style:

- Use explicit constructor/destructor pairs such as `HttpServer_constructor` / `HttpServer_destructor`.
- Use function callbacks declared in headers.
- Keep structs alive until the server is stopped.
- Pair `baMalloc` with `baFree`, and library constructors with matching destructors.

C++ style:

- Use wrapper constructors and destructors when available.
- Use `ThreadLock` and `ThreadReleaseLock` for scoped mutex handling.
- Use static member functions or free functions for C callback slots.
- Use `extern "C"` for entry points called by C code.
- Do not throw C++ exceptions through BAS/BWS callbacks, service functions, destructors, or RTOS C APIs.
- Respect the C object layout. Many C++ wrappers inherit from C structs and are thin inline wrappers over the C API.

If mixing C and C++, put C-callable declarations in headers with:

```c
#ifdef __cplusplus
extern "C" {
#endif

void installVirtualDir(HttpServer* server);

#ifdef __cplusplus
}
#endif
```

## BAS-Specific Guidance

Use BAS when the application needs Lua, Lua Server Pages, Mako, Xedge, BAS Lua APIs, or future Lua bindings. BAS includes BWS C/C++ primitives, so native HTTP/WebSocket services can usually be written the same way as in BWS.

For custom BAS startup:

- Compile `src/BAS.c`.
- Add the same porting layers as BWS.
- Initialize the same dispatcher/server primitives.
- Add BAS-specific startup only when Lua/LSP is needed.
- Use the separate Lua binding skill for C/C++ functions exposed to Lua:
  https://realtimelogic.com/downloads/ai-skills/Lua-Binding-Skill.md

For Mako and Xedge source integration, prefer the runtime's existing extension hooks and startup order. Do not invent a second server or Lua state.

## BWS-Specific Guidance

Use BWS when the application is native C/C++ only and does not need Lua/LSP.

Typical BWS application code includes:

- `src/BWS.c`
- `ThreadLib.c`
- `SoDisp.c`
- optional `BaFile.c`
- application services such as REST or WebSocket code
- HLOS scaffolding or RTOS startup task

The C REST and C++ WebSocket examples are BWS examples, but the same service code can generally be linked with BAS when the broader application uses BAS.

## Build Guidance

Linux/POSIX style:

```sh
gcc -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix \
  src/BWS.c src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c \
  examples/HostInit/Main.c examples/HostInit/HostInit.c \
  my_service.c -lpthread -lm -ldl
```

For C++ application files, compile `.cpp` with the C++ compiler and link with the C++ compiler or add the required C++ runtime library:

```sh
g++ -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix \
  src/BWS.c src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c \
  examples/HostInit/Main.c examples/HostInit/HostInit.c \
  my_service.cpp -lpthread -lm -ldl
```

For BAS, replace `src/BWS.c` with `src/BAS.c`.

For RTOS builds:

- Do not copy host `main` blindly.
- Create platform init code similar to `HostInit.c`.
- Start a dedicated BAS/BWS task/thread that calls `barracuda()`.
- Add the correct `ThreadLib.c`, `SoDisp.c`, and optional `BaFile.c` for the target.
- Use embedded ZIP resources when no filesystem is available.

## Review Checklist

Before finalizing code, verify:

1. Exactly one of `src/BWS.c` or `src/BAS.c` is compiled.
2. Include paths match the selected OS, NET, and filesystem porting layers.
3. Server, dispatcher, mutex, listeners, directories, pages, resource readers, and I/O objects outlive the dispatcher loop.
4. Constructors and destructors are paired correctly.
5. Inserted `HttpDir`/`HttpPage` objects use persistent storage.
6. Directory and page names use string literals or other persistent memory, not stack or temporary strings.
7. `HttpServer_insertDir` path composition is intentional: `virtualDirRootPath` plus `HttpDir` name produces the final URL.
8. VFS priorities are within the documented C/C++ range `-14..14`, and duplicate names rely on explicit priority only when needed.
9. Service callbacks return `0` when handled and negative, normally `-1`, when not handled.
10. Filter/prologue callbacks return `-1` to continue after approval and `0` only after rejection or complete handling.
11. Missing-route callbacks do not send premature 404 responses before the VFS chain can continue.
12. Static resource fallback order is deliberate: `HttpResRdr_insertPrologDir` before static lookup, `HttpDir_insertDir` after static misses.
13. Request/response pointers are not saved beyond the request.
14. JSON parser/encoder objects are destroyed on all paths.
15. Generated `CspReader` and `ZipReader` objects are validated before insertion, and `ZipIo_getECode` is checked.
16. Authenticators, authorizers, login response objects, user databases, protected directories, and resource managers have persistent lifetime.
17. Upload endpoints bound accepted methods/paths, handle upload errors, and move slow post-upload work to a worker thread.
18. `HttpAsynchResp` is used only when a worker must continue a response after the request callback returns, and transient request data is copied first.
19. WebDAV uses a writable `IoIntf`, a lock directory relative to that `IoIntf` root, and a unique lock directory per instance.
20. Outbound HTTPS seeds SharkSSL entropy, installs a CA list when validation is required, and requires trusted certificates before sending sensitive data.
21. Socket takeover code starts the worker thread exactly once, moves the connection only once, sets the moved connection's dispatcher, and destroys the moved connection in the worker.
22. WebSocket writes and shared lists are protected by the dispatcher mutex.
23. If a write can release the mutex, list iteration is restarted or validated before continuing.
24. Blocking work does not hold the dispatcher mutex unless the API requires it.
25. C++ callbacks are static/free functions and exceptions do not cross C callback boundaries.
26. Entry points called from C are exported with `extern "C"`.
27. RTOS integrations replace host scaffolding with target startup code.
28. Lua binding tasks are routed to the Lua binding skill, not solved by ad hoc native API guesses.
