# Source Map and Evidence Rules

## Contents

- [Repository and checkout sources](#repository-and-checkout-sources)
- [Header routing](#header-routing)
- [Documentation routing](#documentation-routing)
- [Example routing](#example-routing)
- [Related public guidance](#related-public-guidance)
- [Excluded material](#excluded-material)
- [Known example hazards](#known-example-hazards)
- [Efficient source searches](#efficient-source-searches)

## Repository and checkout sources

Use the active checkout supplied by the user. In the public amalgamated repository, resolve these paths relative to the checkout root:

- public headers: `inc/`
- amalgamations and porting sources: `src/`
- build documentation: `doc/`
- direct C REST example: `examples/C-RESTful-Service/`
- Modern C++ REST layer: `cpp/rest/`
- Modern C++ REST example: `examples/Modern-Cpp-REST-Service/`
- WebSocket example: `examples/C-WebSockets/`
- HLOS startup scaffolding: `examples/HostInit/`

Do not embed a clone directory name or an absolute developer-machine path in instructions, public includes, or build files. If the user supplies an expanded/full SDK checkout, locate its headers, documentation, examples, and manifests within that checkout. Full and amalgamated packages can arrange porting sources differently; never mix paths between them. Public amalgamated applications normally consume `src/BWS.c` or `src/BAS.c`, while expanded-source builds may use other manifests.

## Header routing

Use `inc/barracuda.h` as a convenience umbrella, but open the owning header before using an API.

| Concern | Primary headers |
| --- | --- |
| Server, command, request, response, VFS directories/pages, config, sessions | `HttpServer.h` |
| Dispatcher and connections | `SoDisp.h`, `SoDispCon.h`, `HttpConnection.h` |
| Threads, mutexes, semaphores, RAII locks | `ThreadLib.h` |
| HTTP worker pool | `HttpCmdThreadPool.h`, `HttpCmdThreadPoolIntf.h` |
| HTTP and HTTPS listeners | `HttpServCon.h`, `HttpSharkSslServCon.h`, `SharkSSL.h` |
| Static resources | `HttpResRdr.h`, `IoIntf.h`, `BaDiskIo.h`, `ZipIo.h` |
| Generated CSP/ZIP readers | `CspRunTm.h`, `IoIntfCspReader.h`, `IoIntfZipReader.h` |
| Writable resources and WebDAV | `HttpResMgr.h`, `WebDAV.h` |
| Request bodies and multipart upload | `HttpRecData.h`, `HttpUpload.h`, `MultipartUpload.h` |
| Deferred/async HTTP | `HttpAsynchReq.h`, `HttpAsynchResp.h` |
| Authentication and authorization | `AuthenticatedUser.h`, `Authenticator.h`, `BasicAuthenticator.h`, `DigestAuthenticator.h`, `FormAuthenticator.h` |
| Native WebSocket | `WebSocketServer.h` |
| Outbound HTTP(S) | `HttpClient.h`, `SharkSSL.h`, `SharkSslEx.h` |
| JSON | `JEncoder.h`, `JParser.h`, `JVal.h`, `JDecoder.h` |
| Custom VFS implementation | `IoIntf.h`, `RecIoIter.h` |
| BAS/Lua host | `balua.h`, `lua.h`, `lauxlib.h`, `lualib.h` |
| Errors, tracing, buffers, allocators | `BaErrorCodes.h`, `HttpTrace.h`, `BufPrint.h`, `DynBuffer.h`, `AllocatorIntf.h` |

Confirm C callback typedefs and C++ inline wrappers in the same header. C++ wrappers are thin frontends over the C ABI. Check the inline body: a wrapper may discard a C return value or expose different arguments. Do not infer error reporting or automatic cleanup from its C++ spelling.

## Documentation routing

Use [Native API usage](api-usage.md) for task selection and call-contract checks. Prefer locally generated Doxygen matching the active SDK; the published reference may describe a different release.

Use the documentation present in the active checkout:

- `doc/compiling.md`: Mako Server and Xedge host builds, feature macros, and shared porting-layer orientation.
- `cpp/rest/README.md`: Modern C++ REST configuration, streaming bodies/responses, and BAS/BWS JSON integration.
- `examples/C-RESTful-Service/README.md`: direct C/C++ service build and API usage.
- `examples/Modern-Cpp-REST-Service/README.md`: optional C++ REST layer, development UI, committed embedded UI, and release/regeneration workflows.
- `examples/C-WebSockets/README.md`: native WebSocket example and supported resource/build modes.

An expanded SDK may also provide focused JSON, maximum-socket, Lua-binding, or signed/encrypted-ZIP documents. Locate them in that checkout instead of assuming a fixed private path. Treat documentation as explanatory; header declarations control signatures.

## Example routing

When an expanded SDK includes them, use these examples selectively:

| Need | Example files |
| --- | --- |
| Minimal server and VFS/resource modes | `examples/introduction/src/start.c`, `InstallVD.c` |
| Read/write resource manager and basic auth | `examples/HttpResMgr/src/InstallVD.c` |
| HTTPS client, CA list, trust checks | `examples/HttpsClient/src/HttpsClientTest1.c` |
| Socket takeover | `examples/ServerTcpCon/src/MyService.c` |
| Upload plus worker and `HttpAsynchResp` | `examples/UploadFirmware/src/UploadFirmware.cpp` |
| WebDAV | `examples/WebDAV/src/Barracuda.cpp` |
| Authentication/authorization | `examples/security/src/InstallVD.c`, `UserDB.c`, `realm.c` |
| File-backed CSP/ZIP readers | `examples/FileReader/` |
| Network-backed development resources | `examples/LspNetIo/` |
| Older WebSocket/threading reference | `examples/WebSocket-Chat/src/ChatServer.cpp` |

Prefer these public examples for new work:

- `examples/C-RESTful-Service`: direct BAS/BWS route service, streaming request-body parser, C/C++ JSON response, host Makefile and VS project, and API test. Prefer it for C-only targets or the smallest additional REST footprint.
- `cpp/rest` and `examples/Modern-Cpp-REST-Service`: optional C++11 REST routing/validation layer, typed parameters, streaming bodies/responses, host Makefile and VS project, and development versus committed embedded-UI workflows. Prefer it when the target has a modern C++ compiler and the convenience justifies the measured footprint.
- `examples/C-WebSockets`: native C++ WebSocket, disk versus embedded ZIP resources, broadcast worker, Linux/Windows/RTOS build shapes.

The direct C and Modern C++ REST examples are alternatives, not a replacement sequence. Keep relative cross-links accurate when documenting them.

Use `examples/HostInit/Main.c`, `HostInit.c`, and `OpenSocketCon.h` only as HLOS scaffolding. Replace them for RTOS/firmware.

## Related public guidance

Native C/C++ HTTP, VFS, JSON, and WebSocket work stays in this skill. For adjacent Lua work, use the public guidance when it is available:

- Lua bindings and native objects: <https://realtimelogic.com/downloads/ai-skills/Lua-Binding-Skill.md>
- Lua-side VFS application design: <https://realtimelogic.com/downloads/ai-skills/VFS-skill.md>

Do not use Lua-side VFS guidance as a substitute for the native `HttpDir`/`HttpPage` contracts in the public headers.

## Excluded material

Do not use these as primary design sources unless a user explicitly targets them:

- `examples/FuguHub`
- `examples/TEST`, `XluaTst`, test directories, logs, and test harness output
- `obj`, generated build products, Visual Studio `tlog`, ZIPs, binaries, and old project copies
- product-specific Xedge/Mako application resource trees
- `CRAP`, miscellaneous experiments, and unrelated demos

Do not scan `.svn/pristine` or infer current behavior from cached historical files.

## Known example hazards

Examples are educational and sometimes old. Correct them using the headers:

- The public REST C++ example constructs its static `HttpServer` before applying `HttpServerConfig` setters. Configure first, then construct.
- The legacy `ServerTcpCon` example calls `Thread_start` in both its constructor and caller. Start exactly once.
- Several examples use static demo passwords and permissive error handling. Never copy these into production.
- Some examples assume one-shot initialization without an explicit guard. Preserve that constraint or add a lifecycle owner.
- Some examples use raw `malloc/free`, `new/delete`, or placement new in ways that require exact matching. Do not mix allocators.
- Old path and port defaults are demonstration details, not API contracts.

## Efficient source searches

Search before coding:

```text
rg -n "typedef .*HttpDir_Service|HttpDir_constructor|HttpDir_setService" inc/HttpServer.h
rg -n "HttpServerConfig_|HttpServer_constructor|HttpServer_insert" inc/HttpServer.h
rg -n "WSS_constructor|WSS_upgrade|WSS_connect|WSS_write" inc/WebSocketServer.h
rg -n "HttpUpload_|HttpAsynchResp_" inc/HttpUpload.h inc/HttpAsynchResp.h
rg -n "Authenticator|UserIntf|AuthorizerIntf|LoginRespIntf" inc/*.h
rg -n "constructor|destructor|setRootDir|getECode" inc/BaDiskIo.h inc/ZipIo.h
```

Use `rg --files` to locate target-specific porting sources. Avoid copying a path from a different checkout layout.
