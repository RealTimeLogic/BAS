# Embedded Modern C\+\+ REST layer

This directory contains a C\+\+11 REST routing library for the native
[Barracuda App Server](https://realtimelogic.com/products/barracuda-application-server/)
and
[Barracuda Web Server](https://realtimelogic.com/products/barracuda-web-server/)
(BAS/BWS). A C or C\+\+ application installs `RestService` into its
existing
[Virtual File System](https://realtimelogic.com/ba/doc/en/VirtualFileSystem.html).

This implementation is optimized for embedded systems with limited memory.
It uses no regular-expression engine, RTTI, coroutine runtime, `std::function`,
`std::string`, or per-request routing allocation.

## Files

- `inc/bas/rest/Rest.h`: the public API together with the bounded handler adaptation.
- `src/Rest.cpp`: BAS/BWS adapters, route compilation/matching, typed parsing,
  and response helpers.
- `../../examples/Modern-Cpp-REST-Service`: complete native BWS example.

Add both `cpp/rest/inc` and the normal BAS/BWS include directories to the
C\+\+ include path. Compile `cpp/rest/src/Rest.cpp` as C\+\+11 or newer. Compile
the selected BAS/BWS amalgamation and porting layers as C, then link the final
program with a C\+\+ linker.

The public include is written as:

```cpp
#include <bas/rest/Rest.h>
```

Here, `bas` comes from the directory hierarchy beneath the configured include
root: `cpp/rest/inc` + `bas/rest/Rest.h` resolves to
`cpp/rest/inc/bas/rest/Rest.h`. It is a namespace-like directory used to group
the library's public headers. Configure the compiler
with `cpp/rest/inc` as the include directory, not with
`cpp/rest/inc/bas/rest`. This keeps the same portable include statement in the
library, examples, and application code.

## Minimal use

```cpp
#include <bas/rest/Rest.h>

static RestRouter router;

static void installApi(HttpServer& server)
{
   router.get("/health",
      [](RestContext&, RestRequest&, RestResponse& response) -> RestResult {
         return response.json(200, "{\"status\":\"ok\"}");
      });

   router.post("/gpio/{pin:int}/set/{state:uint}",
      [](RestContext& context, RestRequest& request,
         RestResponse& response) -> RestResult {
         S32 pin;
         U32 state;
         if(!request.getIntParameter("pin", pin) ||
            !request.getUnsignedParameter("state", state))
            return response.error(400, "Invalid parameters");

         bool ok = context.withServerUnlocked([&] {
            return updateDevice(pin, state); // no BAS/BWS calls here
         });
         return ok ? response.noContent() :
            response.error(500, "Device update failed");
      });

   static RestService service("api", router);
   server.insertRootDir(service.getHttpDir());
}
```

Verify each `RestRouteStatus` during startup in production environments. Constructing
`RestService` freezes its router; later registration returns
`RestRouteStatus_Frozen`. The router and service objects must persist for the duration of the running application.

## Routing contract

Patterns begin with `/` relative to the `RestService` mount. Supported
parameters are:

- `{name}` or `{name:string}`: one non-empty bounded segment;
- `{name:int}`: strict signed 32-bit decimal;
- `{name:uint}`: strict unsigned 32-bit decimal;
- `{name:path}`: a bounded non-empty remainder and therefore the final pattern
  component.

Static components take precedence over numeric parameters, numeric parameters
over string parameters, and string parameters over path remainders. This is
independent of registration order. Duplicate routes and same-method numeric
ambiguities are rejected during registration.

The initial method set is `GET`, `HEAD`, `POST`, `PUT`, `PATCH`, and `DELETE`.
`OPTIONS` is reserved for automatic handling. 

Trailing slashes are strict: `/health` and `/health/` are different. A `HEAD`
request falls back to its matching `GET` handler, and BAS/BWS suppresses the
body while calculating the length. `OPTIONS` is automatic. A path match with
the wrong method returns `405` with `Allow`; unknown paths return `404`;
malformed typed parameters return `400`.

BAS/BWS percent-decodes and performs its dot-segment processing before calling
an `HttpDir_Service`. It rejects malformed escapes and encoded NUL/control
bytes before this layer runs. Consequently, the router matches the normalized
decoded path and does not decode it again. An encoded separator such as `%2F`
is therefore a path separator, not segment data.

## Ownership and allocation

`RestStringView` is borrowed and length-delimited. A missing value has a null
`data()` pointer; a present-but-empty value has non-null data and size zero.
Views returned for route parameters, headers, query parameters, cookies, URI,
and authenticated-user names are valid only during the active handler. Use
`copyTo()` or another caller-owned bounded buffer before retaining data or
handing it to asynchronous work.

Route patterns and small callable objects are copied into the router during
startup. Dispatch itself uses router-owned immutable data and stack-local
parameter state. It does not allocate from the heap. A callable capture larger
than `BAS_REST_HANDLER_SIZE` fails at compile time; use the function-pointer
plus context overload to keep a larger application-owned handler elsewhere.
A callable's own constructor may, of course, allocate.

BAS/BWS may allocate while processing its normal server features, including
header storage, cookie parsing, sessions, and response header growth. The REST
layer does not change those existing policies.

The default capacities may be overridden before including `Rest.h`:

| Macro | Default | When and how to use it |
| --- | ---: | --- |
| `BAS_REST_MAX_ROUTES` | 8 | Maximum number of routes registered in one `RestRouter`; each HTTP method and pattern combination consumes one slot. Set it to the largest router in the application. Increasing it grows every `RestRouter` by one complete route slot per added route, so keep it close to the actual route count. Registration beyond the limit returns `RestRouteStatus_RouteCapacity`. |
| `BAS_REST_MAX_SEGMENTS` | 8 | Maximum number of slash-separated components in a route pattern. For example, `/devices/{id}/outputs/{pin}` uses four segments. Increase it only when the deepest registered pattern needs more components. Each additional segment increases every route slot and therefore the persistent size of every router. Registration of a deeper pattern returns `RestRouteStatus_SegmentCapacity`. |
| `BAS_REST_MAX_PARAMS` | 4 | Maximum number of named parameters in one pattern, including `{name}`, `{name:int}`, `{name:uint}`, and `{name:path}`. Set it to the most parameters used by any single route. Increasing it enlarges stack-local matching/request state; it does not increase the route-pattern buffer. Registration with too many parameters returns `RestRouteStatus_ParameterCapacity`. |
| `BAS_REST_MAX_PATTERN_LENGTH` | 96 | Maximum route-pattern length in bytes, excluding the terminating NUL. Set it to the longest pattern passed to `get()`, `post()`, or another registration function. The implementation uses one fixed pattern buffer per route, so increasing this value increases persistent router RAM by about one buffer per route. The current compact offset representation imposes a hard maximum of 255. An oversized pattern returns `RestRouteStatus_PatternTooLong`. |
| `BAS_REST_MAX_REQUEST_PATH` | 256 | Maximum decoded, normalized relative request-path length inspected by the router. Increase it when valid deployed URLs can exceed this length; reduce it to reject unexpectedly long paths earlier. This is a scan limit rather than a path-buffer allocation, so changing it does not directly enlarge `RestRouter` or `RestRequest`. An oversized request is answered with `414 URI Too Long`. BAS/BWS may impose an earlier request-line limit. |
| `BAS_REST_MAX_PARAMETER_LENGTH` | 64 | Maximum byte length accepted for one captured parameter value. It applies to string, numeric, and `{name:path}` values; the latter may contain the complete remaining path. Increase it for legitimate long identifiers or path remainders, or reduce it as an application input bound. Values are borrowed rather than copied, so this limit does not reserve a buffer of the same size. Exceeding it makes the route input malformed and produces `400`. |
| `BAS_REST_HANDLER_SIZE` | 24 | Inline storage, in bytes, reserved in every route slot for a capturing lambda or other callable. Use the smallest value that holds the application's largest stored handler. Increasing it grows persistent router RAM once per route. If a capture does not fit, compilation fails; prefer the function-pointer-plus-context overload for large application-owned state instead of inflating every route slot. |

Define overrides consistently in the build before `Rest.h` is included, for
example with compiler options such as `-DBAS_REST_MAX_ROUTES=12` and
`-DBAS_REST_MAX_REQUEST_PATH=192`, or in one project configuration header that
is force-included before `Rest.h`. 

## Dispatcher mutex and application concurrency

BAS/BWS enters the service callback with the dispatcher mutex locked.
`RestContext::withServerUnlocked()` and `RestServerUnlock` use the native
`ThreadReleaseLock(HttpRequest*)` mechanism. The outermost region releases the
mutex and always reacquires it before returning, including exception unwinding
when exceptions are enabled. Nested regions are allowed and do not unlock or
relock twice.

Treat an unlocked region as application-only code:

1. validate and copy all required request input while locked;
2. perform blocking device, database, or other non-BAS work while unlocked;
3. write the response only after the mutex has been reacquired.

Do not call `RestRequest`, `RestResponse` or other BAS/BWS APIs while
unlocked. Releasing the server mutex also removes its incidental
serialization of application state; protect shared device or database
state with an application-owned mutex.

## Bodies and responses

`RestBodyReader` is a stack wrapper over [HttpRecData](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpRecData.html). It supports bounded incremental consumption without forcing the complete request into RAM. Check `RestRequest::validateBody()` or the reader's validation code, enforce an application-specific maximum, and consume or destroy the reader before the handler returns. Multipart upload and asynchronous upload lifecycles remain the responsibility of the existing `HttpUpload`, `MultipartUpload`, and `HttpAsynchReq` APIs.

`RestResponse` writes directly through `HttpResponse`. `write()`, `send()`,
`beginStreaming()`, and `flush()` propagate native errors such as a peer
disconnect. BAS/BWS chooses buffering and HTTP transfer framing. The `text()`
and `json()` conveniences are for already encoded small bodies. In particular,
`json()` sets the JSON content type but does not validate, escape, or otherwise
encode its argument. Once committed, status and header changes return the
native `E_IS_COMMITTED` error. Header names and values are checked for response
splitting characters, and unknown BAS status codes are rejected.

Use `nativeCommand()`, `nativeRequest()`, `nativeResponse()`, and `writer()` as
explicit escape hatches for an existing BAS/BWS feature. Their lifetime and
locking rules remain unchanged.

## JSON parsing and encoding

Both the BAS and BWS C code libraries include native JSON facilities. No
additional JSON library is required:

- [JParser](https://realtimelogic.com/ba/doc/en/C/reference/html/structJParser.html) incrementally parses JSON;
- [JParserValFact](https://realtimelogic.com/ba/doc/en/C/reference/html/structJParserValFact.html) can build a `JVal` tree from parser events;
- [JVal](https://realtimelogic.com/ba/doc/en/C/reference/html/structJVal.html) provides typed extraction from that tree;
- [JEncoder](https://realtimelogic.com/ba/doc/en/C/reference/html/structJEncoder.html) safely escapes and writes JSON to a [BufPrint](https://realtimelogic.com/ba/doc/en/C/reference/html/structBufPrint.html) such as the active
  HTTP response writer.

These APIs complement the REST layer. `RestBodyReader` provides bounded access
to the request body, and `RestResponse::writer()` connects `JEncoder` directly
to the response. The following basic endpoint accepts
`{"message":"hello"}` and returns `{"echo":"hello"}`. The request is fed to
the streaming parser in small chunks; the complete JSON document is never
copied into a fixed-size input buffer:

```cpp
#include <bas/rest/Rest.h>
#include <JEncoder.h>
#include <JParser.h>
#include <JVal.h>

static bool isJsonContentType(RestStringView contentType)
{
   static const char mediaType[] = "application/json";
   size_t i;
   if(!contentType.isPresent() ||
      contentType.size() < sizeof(mediaType) - 1)
      return false;
   for(i = 0; i < sizeof(mediaType) - 1; ++i)
   {
      char ch = contentType.data()[i];
      if(ch >= 'A' && ch <= 'Z')
         ch = (char)(ch + ('a' - 'A'));
      if(ch != mediaType[i])
         return false;
   }
   while(i < contentType.size() &&
         (contentType.data()[i] == ' ' || contentType.data()[i] == '\t'))
      ++i;
   return i == contentType.size() || contentType.data()[i] == ';';
}

static RestResult writeEcho(RestResponse& response, const char* message)
{
   RestResult result = response.setStatus(200);
   if(result.succeeded())
   {
      result = response.setContentType("application/json; charset=UTF-8");
      if(result.succeeded())
      {
         result = response.beginStreaming();
         if(result.succeeded())
         {
            JErr error;
            JEncoder encoder(&error, response.writer());
            int encodeResult = encoder.set("{s}", "echo", message);
            int flushResult = encoder.flush();
            result = RestResult::fromNative(
               encodeResult ? encodeResult : flushResult);
         }
      }
   }
   return result;
}

static RestResult echoJson(RestContext&, RestRequest& request,
                           RestResponse& response)
{
   if(!isJsonContentType(request.contentType()))
      return response.error(415, "Expected application/json");

   RestBodyReader body(request);
   if(!body.isValid())
      return response.error(400, "Invalid request body");

   AllocatorIntf* allocator = AllocatorIntf::getDefault();
   JParserValFact values(allocator, allocator);
   char memberName[64];
   JParser parser(static_cast<JParserIntf*>(&values),
                  memberName, sizeof(memberName), allocator);

   U8 input[256];
   S32 count;
   while((count = body.read(input, sizeof(input))) > 0)
   {
      if(parser.parse(input, (U32)count) < 0)
         break;
   }
   if(count < 0)
      return response.error(400, "Cannot read request body");
   if(parser.getStatus() != JParsStat_DoneEOS)
      return response.error(400, "Malformed JSON");

   const char* message = 0;
   JErr error;
   JVal* root = values.getFirstVal();
   if(root)
      root->get(&error, "{s}", "message", &message);
   if(!root || error.isError() || !message)
      return response.error(400, "String member 'message' required");

   return writeEcho(response, message);
}

static RestRouteStatus registerJsonRoute(RestRouter& router)
{
   // Call during startup, before RestService freezes the router.
   return router.post("/echo", echoJson);
}
```

`JParserValFact` owns the generated `JVal` tree, so extracted string pointers
remain valid only until the factory is destroyed at the end of the handler.
The 256-byte array is only a reusable input chunk; `JParser` retains its parse
state between reads. `JParserValFact` still allocates the generated tree nodes
and strings. The example uses the default BAS/BWS allocator, but a
memory-constrained target should supply an application-owned bounded allocator
and enforce any application-specific document limits. Seee the [JSON Library Tutorial](https://realtimelogic.com/ba/doc/en/C/reference/html/md_en_C_md_JSON.html) for details.

## Language and error model

The required language level is C\+\+11. RTTI is not used. Normal failures use
`RestResult`, `RestRouteStatus`, and native BAS/BWS codes. Builds may disable
exceptions. When exceptions are enabled, `RestService` catches all exceptions
at the C callback boundary and emits a controlled `500` if the response is not
already committed; no exception crosses into C.

Handler captures are shared immutable route state. If a handler mutates a
captured application object, synchronization is the application developer's
responsibility, particularly inside unlocked regions.

The Rest library intentionally has no allocating middleware chain.
Use the existing `HttpDir` authenticator/authorizer on
`RestService::getHttpDir()` for access control, wrap a handler with a small
application-owned policy function for route-local validation, and set CORS or
logging headers through the direct request/response facades. This preserves a
small core while leaving policy composition explicit.
