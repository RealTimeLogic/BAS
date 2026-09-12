# Native API usage

Use this reference to select an API and check its calling contract. Keep detailed parameter and return-value documentation in Doxygen. Read the owning header and implementation in the active checkout before writing calls; do not mix headers and amalgamations from different SDK versions.

## Find the API

The links below point to the published C/C++ reference. An expanded SDK can also provide matching HTML under `doc/en/C/reference/html`. Prefer that local reference when available; the published version may differ from the target checkout.

| Application task | APIs and owning headers | Focused recipe and Doxygen |
| --- | --- | --- |
| Configure and start a server | `HttpServerConfig`, `HttpServer` in `HttpServer.h`; `SoDisp.h` | [Startup and VFS](core-server-vfs.md), [HttpServer](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpServer.html) |
| Install a route or endpoint | `HttpDir`, `HttpPage` in `HttpServer.h` | [HTTP services](http-json-websocket.md), [HttpDir](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpDir.html) |
| Read parameters and send responses | `HttpRequest`, `HttpResponse` in `HttpServer.h` | [Request/response](core-server-vfs.md), [HttpResponse](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpResponse.html) |
| Stream a request body | `HttpRecData.h` | [Body handling](http-json-websocket.md), [HttpRecData](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpRecData.html) |
| Parse or encode JSON | `JParser.h`, `JDecoder.h`, `JVal.h`, `JEncoder.h` | [JSON recipes](http-json-websocket.md), [JSON guide](https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_JSON.html) |
| Serve disk or ZIP resources | `HttpResRdr.h`, `IoIntf.h`, `BaDiskIo.h`, `ZipIo.h` | [Resource mounting](core-server-vfs.md), [IoIntf](https://realtimelogic.com/ba/doc/en/C/reference/html/structIoIntf.html) |
| Accept uploads or provide WebDAV | `HttpUpload.h`, `MultipartUpload.h`, `WebDAV.h` | [Upload and WebDAV](security-io-async.md), [HttpUpload](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpUpload.html) |
| Defer an HTTP response | `HttpAsynchReq.h`, `HttpAsynchResp.h` | [Asynchronous HTTP](security-io-async.md), [HttpAsynchResp](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpAsynchResp.html) |
| Handle WebSockets | `WebSocketServer.h` | [WebSockets](http-json-websocket.md), [WebSocketServer](https://realtimelogic.com/ba/doc/en/C/reference/html/structWSS.html) |
| Make outbound HTTP requests | `HttpClient.h` | [Outbound clients](security-io-async.md), [HttpClient](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpClient.html) |
| Authenticate and authorize users | `AuthenticatedUser.h`, `Authenticator.h`, concrete authenticator headers | [Authentication](security-io-async.md), [AuthenticatedUser](https://realtimelogic.com/ba/doc/en/C/reference/html/structAuthenticatedUser.html) |
| Deliver native events to Lua | `ThreadJob_lcreate`, `LThreadMgr_run` in `lxrc.h` | [Thread manager](https://realtimelogic.com/ba/doc/en/C/reference/html/group__ThreadMgr.html); also use the repository's Lua application skill |

For OS initialization, listeners, and compiler settings, use [Build and porting](build-porting.md).

## Check the call contract

Before implementing a call, determine:

- Inputs: exact types, lengths and units, optional values, and whether strings or buffers are copied or borrowed.
- Results: distinguish status codes, byte counts, Boolean results, queued work, and completed work. Inspect output parameters on the documented paths only.
- Ownership: identify the matching destructor or close operation and whether closing consumes the object even when it reports failure.
- Callbacks: check argument lifetimes, callback return semantics, and cleanup invocations with NULL request arguments.
- Concurrency: establish which thread owns the object and when the dispatcher mutex is held or temporarily released. Never access Lua while that mutex is released.

Do not assume all constructors can report allocation failure: check whether the selected constructor returns a status or follows the library's fatal-error policy. Check C++ inline wrappers as well as C declarations; a wrapper can discard a status returned by the C function.

## C and C++ response examples

These are alternative helpers for an existing HTTP service callback. `response` must be live, the dispatcher mutex must be held, and headers must not have been committed. `text` is a required NUL-terminated UTF-8 string. Both helpers return zero on success or a nonzero BAS status on failure.

```c
#include "HttpServer.h"

int sendTextC(HttpResponse* response, const char* text)
{
   int status = HttpResponse_setStatus(response, 200);
   if(!status)
      status = HttpResponse_setContentType(response, "text/plain; charset=utf-8");
   if(!status)
      /* Negative length selects strlen; use an explicit length for binary data. */
      status = HttpResponse_write(response, text, -1, TRUE);
   return status;
}
```

```cpp
#include "HttpServer.h"

int sendTextCpp(HttpResponse* response, const char* text)
{
   // The void C++ setStatus wrapper discards the C status in this API.
   int status = HttpResponse_setStatus(response, 200);
   if(!status)
      status = response->setContentType("text/plain; charset=utf-8");
   if(!status)
      status = response->write(text);
   return status;
}
```

A successful buffered write does not prove delivery to the peer. A failure may follow partial output; do not automatically attempt a second response.

Do not return either helper's status directly from `HttpDir_Service`. That callback's return value controls route lookup: return zero when the directory owns and handles the request, handling or logging write failures according to the application. Return a negative value only when declining the request without sending a response.

## Lifetime and callback rules

- In a directory service, check `cmd == NULL` before reading the relative path. This is a cleanup invocation. Run the appropriate base cleanup and release only storage the callback owns. Page cleanup similarly uses NULL request/response arguments.
- Request objects and request-backed strings are borrowed. Copy data needed after the request or use the documented asynchronous request/response mechanism.
- Check nullable results, including session creation, before using the returned object.
- Construct persistent VFS nodes once, check insertion results, and keep their dependencies alive until unlinked and cleaned up.
- `ThreadJob_create` and `ThreadJob_lcreate` return NULL on allocation failure. Follow the application's allocation-failure policy; do not introduce indefinite retry loops.
- Deliver native events to Lua through `ThreadJob_lcreate` and `LThreadMgr_run`, following `examples/xedge/src/AsynchLua.c`. Copy payload into job-owned memory. The manager owns a queued job and releases it after execution; use its Lua state and a protected Lua call in the callback. Queueing does not establish callback success.
- For Lua bindings, throw for incorrect API usage; return `nil, error` for recoverable operational failures. Preserve each native API's documented status convention. Do not propagate C++ exceptions through C callbacks.

## Verify generated application code

Compile C examples as C and C++ examples as C++ against the target headers and porting includes. Then exercise the application's actual protocol and cleanup paths using the relevant recipe. Compilation validates declarations and syntax, not runtime behavior. If documentation and implementation disagree, record the precise discrepancy rather than inventing a contract.
