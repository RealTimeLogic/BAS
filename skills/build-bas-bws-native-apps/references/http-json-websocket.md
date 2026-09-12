# HTTP, JSON, REST, and WebSocket Recipes

## Contents

- [Define the protocol contract](#define-the-protocol-contract)
- [REST directory pattern](#rest-directory-pattern)
- [Optional Modern C++ REST layer](#optional-modern-c-rest-layer)
- [Request body and JSON parsing](#request-body-and-json-parsing)
- [JSON responses](#json-responses)
- [REST validation](#rest-validation)
- [WebSocket connection pattern](#websocket-connection-pattern)
- [Broadcast and concurrency](#broadcast-and-concurrency)
- [WebSocket validation](#websocket-validation)

## Define the protocol contract

Before coding, write down:

- method and path or WebSocket endpoint;
- accepted content types and message/frame types;
- authentication/authorization;
- maximum header, body, frame, upload, and response sizes;
- success status/body and error status/body;
- timeout, disconnect, retry, idempotency, and side effects;
- thread that owns mutable state.

Treat paths, headers, query/form parameters, cookies, bodies, frames, and peer data as untrusted.

## REST directory pattern

Use a named `HttpDir` for a URL tree. This is a routing sketch; implement the indicated method/body validation and response before using it:

```c
static int restService(HttpDir* dir, const char* rel, HttpCommand* cmd)
{
   HttpRequest* req;
   HttpResponse* resp;

   if(!cmd)
   {
      /* Static directory storage stays alive; clean up the base object. */
      HttpDir_destructor(dir);
      return 0;
   }

   if(strncmp(rel, "users", 5) != 0 ||
      (rel[5] != 0 && rel[5] != '/'))
      return -1;

   req = HttpCommand_getRequest(cmd);
   resp = HttpCommand_getResponse(cmd);
   /* Validate method/body and write response. */
   return 0;
}

void installVirtualDir(HttpServer* server)
{
   static HttpDir api;
   HttpDir_constructor(&api, "api", 0);
   HttpDir_setService(&api, restService);
   HttpServer_insertRootDir(server, &api);
}
```

Do not use an unbounded prefix check that makes `usersXYZ` match `users`. Parse numeric IDs with range and full-string validation; do not accept `atoi` partial parses. Map unsupported methods to `405` and consider an `Allow` header. Use `404` for a missing resource and `400`/`422` for malformed input according to the defined contract.

Protect shared REST state. A command pool permits concurrent handlers; a static array or database is not automatically thread safe.

## Optional Modern C++ REST layer

When the active public checkout contains `cpp/rest`, inspect its current `README.md` and `inc/bas/rest/Rest.h` before using it. It is a convenience layer over the same BAS/BWS server and is not required for direct C services.

- Add `cpp/rest/inc` as an include root and include `<bas/rest/Rest.h>`. The `bas` component comes from the directory beneath that include root; it is not the repository folder name and remains stable if the checkout directory is renamed.
- Compile `cpp/rest/src/Rest.cpp` and application `.cpp` files with the documented C++ standard, while compiling amalgamated and porting `.c` files as C. Link with the C++ linker.
- Use `RestBodyReader` for incremental request consumption and `RestResponse::writer()` as the `BufPrint` destination for `JEncoder`.
- Treat `RestResponse::json()` as a convenience for JSON that is already encoded. It sets response metadata but does not validate, escape, or encode dynamic values.
- Preserve native request/response lifetime, dispatcher locking, and error conventions when using escape hatches such as native request, response, command, or writer access.

Do not present this layer as a replacement for the direct C example. The direct API remains appropriate when only C is available or minimum footprint dominates.

## Request body and JSON parsing

Use `HttpRecData_valid`, `HttpRecData_constructor`, `HttpRecData_read`, and `HttpRecData_destructor` for a streaming request body. Check both content length and transfer encoding according to the header/API behavior.

Accept a parsed media type of `application/json`; do not reject a valid parameter such as `application/json; charset=UTF-8` by using only exact raw-string equality. Reject unexpected media types with `415` when a body is required.

Typical JSON value-factory flow:

1. allocate `JParserValFact` and parser buffers with explicit limits;
2. construct `JParserValFact`;
3. construct `JParser` over it;
4. feed every body chunk to `JParser_parse`;
5. continue until the framed request body reaches end-of-input, then require the final parser status expected by the target API, such as `JParsStat_DoneEOS`;
6. destroy `JParser` on all paths;
7. destroy and free the value factory after consuming the value;
8. destroy `HttpRecData` on all paths.

If parsing makes the connection framing unsafe, terminate the connection as documented. Otherwise, send a bounded JSON error response rather than silently closing.

Use `JVal_get` or the target's value APIs with a `JErr`, then validate required members, types, lengths, ranges, duplicate semantics, and unknown-member policy. Parsed string pointers belong to the parse tree; copy them before destroying it.

A reusable small input chunk bounds the transport scratch buffer, not the complete parsed representation. `JParserValFact` retains generated nodes and strings, so memory still grows with document content. Use an application-specific document limit, a bounded/application-owned allocator, or an event-driven `JParserIntf` that does not build a full tree when the target cannot retain arbitrary JSON. Do not accumulate the complete body merely to call the parser once.

The public `C-RESTful-Service/src/RestJsonUtils.c` shows the mechanics, but improve its exact `Content-Type` check, allocation/error response, and application-specific limits.

## JSON responses

Set response metadata before constructing output:

```c
JErr err;
JEncoder enc;
HttpResponse_setStatus(resp, 200);
HttpResponse_setContentType(resp, "application/json; charset=UTF-8");
JErr_constructor(&err);
JEncoder_constructor(&enc, &err, HttpResponse_getWriter(resp));
JEncoder_set(&enc, "{s}", "status", "ok");
JEncoder_destructor(&enc); /* flush/finalize */
```

Confirm format flags in `JEncoder.h`; do not infer them from `printf`. Check `JErr`/encoder status for complex output.

In C++, use a small RAII wrapper only when its `JErr` member is constructed before the base `JEncoder` needs it. C++ base classes initialize before members, so passing a not-yet-constructed member to a base constructor is unsafe. Prefer explicit construction or an ownership arrangement with valid initialization order.

Do not write a response body for statuses that prohibit one, such as `204`.

## REST validation

Exercise at least:

- every supported method and route;
- unknown path and unsupported method;
- no body, empty body, malformed JSON, wrong media type, chunked body;
- missing, wrong-type, oversized, out-of-range, and extra fields;
- maximum configured request size;
- concurrent requests against shared state;
- authorization failure and direct URL access;
- exact status, content type, response JSON, and side effect.

Use the example's Python test as a starting point, but assert results instead of only printing them.

## WebSocket connection pattern

Create one `WSS`-derived or containing object per accepted client. Provide `WSSCB` frame and close callbacks; ping is optional.

Upgrade either with the combined API in the target header:

```c
if(WSS_upgrade(&client->wss, req) != 0)
   destroyClient(client);
```

or explicitly:

```cpp
if(req->wsUpgrade() == 0)
{
   Client* client = new Client(req->getServer()->getDispatcher());
   if(client->connect(req->getConnection()) != 0)
      delete client;
}
else
{
   resp->sendError(400, "Not a WebSocket request");
}
```

Verify exact success conventions in `HttpServer.h` and `WebSocketServer.h`.

The close callback owns final removal from application lists and object destruction. Prevent double deletion between failed upgrade, peer close, application close, and shutdown.

Frame callback data is length-delimited and may be binary or not NUL-terminated. Never use `%s`, `strlen`, or string functions without first validating/copying the length. Define maximum message size and fragmentation behavior from the target WSS API.

## Broadcast and concurrency

`WSS::write`/`WSS_write` may block and may release the dispatcher mutex. Do not synchronously broadcast to many clients from the frame callback.

Preferred design:

1. copy the received frame into an application-owned bounded message;
2. queue it;
3. signal a broadcast worker;
4. in the worker, lock the dispatcher mutex with `ThreadLock` or C mutex APIs;
5. iterate connections and write;
6. assume a write can release the mutex and cause disconnect/removal;
7. restart or validate the iterator after each such change;
8. free the queued message.

Protect both the client list and queue. A semaphore alone does not protect queue mutation unless all producers/consumers run under a documented common lock.

Give the worker an explicit stop protocol for applications that can unload. Do not leave an infinite thread referencing destroyed VFS nodes.

## WebSocket validation

Use a real WebSocket client, not an HTTP GET. Test:

- successful and rejected upgrade;
- at least two simultaneous clients for broadcast;
- text and binary frames if both are supported;
- embedded NUL and non-NUL-terminated text payload;
- maximum/oversized and fragmented frames;
- peer close, server close, abrupt disconnect, reconnect;
- slow receiver while other clients remain responsive;
- application shutdown/unload with connected clients;
- TLS (`wss`) when the deployed interface requires it;
- authentication and origin policy where browsers connect.
