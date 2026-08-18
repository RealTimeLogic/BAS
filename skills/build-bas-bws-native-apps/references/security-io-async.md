# Security, I/O, Async Work, and Native Networking

## Contents

- [Authentication and authorization](#authentication-and-authorization)
- [Static and writable resources](#static-and-writable-resources)
- [WebDAV](#webdav)
- [Uploads and asynchronous responses](#uploads-and-asynchronous-responses)
- [Outbound HTTP and HTTPS](#outbound-http-and-https)
- [HTTP and HTTPS listeners](#http-and-https-listeners)
- [Socket takeover](#socket-takeover)
- [Custom I/O interfaces](#custom-io-interfaces)
- [Security validation](#security-validation)

## Authentication and authorization

Authentication resolves identity. Authorization independently decides whether that identity may use the method/path. Install both at the directory boundary that owns the protected subtree.

Core interfaces:

- `UserIntf`: password/HA1 lookup callback.
- `LoginRespIntf`: failed-login or form-login response callback.
- `BasicAuthenticator`, `DigestAuthenticator`, or `FormAuthenticator`.
- `AuthorizerIntf`: authenticated user, HTTP method, and relative-path policy.
- optional `LoginTracker`: rate/cache/session tracking according to the target API.

Persistent C shape:

```c
static UserIntf users;
static LoginRespIntf login;
static BasicAuthenticator auth;
static AuthorizerIntf authorizer;
static HttpDir privateDir;

UserIntf_constructor(&users, lookupPassword);
LoginRespIntf_constructor(&login, loginResponse);
BasicAuthenticator_constructor(&auth, &users, "private", &login);
AuthorizerIntf_constructor(&authorizer, authorize);
HttpDir_constructor(&privateDir, "private", 0);
HttpDir_setAuthenticator(
   &privateDir, (AuthenticatorIntf*)&auth, &authorizer);
HttpServer_insertRootDir(server, &privateDir);
```

Read callback typedefs in `AuthenticatedUser.h`. In the authorizer:

- deny when a restricted path has no user;
- match the most specific path before broader prefixes;
- validate path boundaries, not only raw prefixes;
- include the HTTP method in the decision;
- return false by default when no explicit rule permits a protected action.

Do not use example credentials. Do not log passwords, HA1 values, bearer/session URLs, or private keys. Use generic login failures and protect state-changing browser operations against CSRF.

`BasicAuthenticator` exposes credentials to the server after TLS protection only; do not use it over plaintext on an untrusted network. Select form/digest behavior based on the real client and threat model, not convenience.

## Static and writable resources

Use:

- `HttpResRdr` for read-only resources;
- `HttpResMgr` for managed read/write resources;
- `WebDAV` for WebDAV methods over a writable `IoIntf`.

Example `HttpResMgr` shape:

```c
static DiskIo io;
static HttpResMgr manager;
DiskIo_constructor(&io);
if(DiskIo_setRootDir(&io, "data"))
   baFatalE(FE_USER_ERROR_1, 0);
HttpResMgr_constructor(&manager, (IoIntf*)&io,
                       3, "files", 0, 0);
HttpResMgr_setAuthenticator(
   &manager, (AuthenticatorIntf*)&auth, &authorizer);
HttpServer_insertRootDir(server, (HttpDir*)&manager);
```

Protect path normalization, overwrite/delete permissions, quotas, executable/script extensions, and symlink behavior. Never expose uploaded or writable content as executable LSP/Lua/CSP unless explicitly designed and authorized.

## WebDAV

Construct `WebDAV` over writable I/O and a lock directory relative to that I/O root. Use a unique lock directory per instance.

```cpp
static DiskIo io;
static WebDAV dav(&io, 3, "dav", ".davlocks");
static HttpDir root(0);
root.insertDir(&dav);
server->insertRootDir(&root);
```

Match maximum uploads with expected `HttpCommand` and command-pool concurrency. Protect WebDAV unless deployment is deliberately anonymous and isolated. Test `PROPFIND`, read, create, overwrite, move/rename, delete, lock behavior, invalid paths, and concurrent operations.

## Uploads and asynchronous responses

Use `HttpUpload` for POST/PUT upload parsing over an `IoIntf`. Supply an `HttpUploadCbIntf` with completion and error callbacks.

The directory service should:

- handle the documented GET page/status paths;
- delegate allowed POST/PUT paths to `HttpUpload_service`;
- send a clear error for invalid method/path;
- return handled after sending/delegating;
- enforce authentication, authorization, size, filename, type, storage, and quota limits.

Upload callback names and request-backed metadata may be transient. Copy everything a worker needs before the callback returns.

Use `HttpAsynchResp` only when work must continue the original response after the request callback returns. Construct it from the active request/connection as documented, switch to asynchronous thread mode if required by the target API, and hold its dispatcher mutex whenever calling server/response operations from the worker.

C++ worker shape:

```cpp
void Worker::run()
{
   /* Perform non-BAS file/compute work without the dispatcher lock. */
   {
      ThreadLock lock(resp.getMutex());
      resp.setHeader("Content-Type", "text/plain");
      resp.getWriter()->printf("complete\n");
      resp.close();
   }
   delete this;
}
```

Verify exact close/destructor locking requirements in `HttpAsynchResp.h`. Do not hold the dispatcher mutex during ZIP extraction, flash writes, scanning, sleeps, DNS, or other slow work.

For firmware uploads, treat the uploaded file as hostile: authenticate, authorize, bound size, verify format, signature, target/version, rollback policy, and storage writes before activation.

## Outbound HTTP and HTTPS

Construct `HttpClient` with the shared dispatcher. For HTTPS:

1. seed SharkSSL entropy using a target-appropriate entropy source;
2. construct a client `SharkSsl` object;
3. install the correct CA list/trust anchors;
4. attach it with `HttpClient_setSSL`;
5. require trusted certificates before sending credentials or sensitive data;
6. close between independent nonpersistent requests;
7. destroy the client before its dispatcher/TLS owners.

```c
HttpClient client;
HttpClient_constructor(&client, dispatcher, 0);
HttpClient_setSSL(&client, shark);
HttpClient_setAcceptTrusted(&client, TRUE);
status = HttpClient_request(
   &client, HttpMethod_Get, url, 0, 0, 0, 0);
HttpClient_close(&client);
HttpClient_destructor(&client);
```

Run the dispatcher, preferably in its normal owner thread, for persistent clients and peer-close detection. Configure connect/read timeouts. Validate hostname and chain; do not equate successful TLS negotiation with trusted identity.

## HTTP and HTTPS listeners

Use `HttpServCon` for HTTP and `HttpSharkSslServCon` with a server-mode `SharkSsl` object and certificate for HTTPS. The HLOS `OpenSocketCon.h` helper is convenient for examples but hides policy. For products, explicitly control:

- port and interface/address family;
- certificate/key source and lifetime;
- TLS versions, ciphers, ALPN, and client certificate policy;
- listener error handling and retry;
- whether plaintext HTTP redirects, serves, or is disabled.

Keep listeners and TLS objects alive for the dispatcher lifetime. Seed entropy before TLS use and verify clock/time requirements for certificate validation.

## Socket takeover

Use takeover only when the connection deliberately stops being HTTP. Prefer WebSocket for browser-compatible bidirectional traffic.

```c
typedef struct Service {
   Thread thread;
   SoDispCon con;
} Service;

static void run(Thread* thread)
{
   Service* self = (Service*)thread;
   /* SoDispCon_blockRead / SoDispCon_sendData loop */
   SoDispCon_destructor(&self->con);
   Thread_destructor(&self->thread);
   baFree(self);
}

static int startService(HttpRequest* req)
{
   Service* self = (Service*)baMalloc(sizeof(Service));
   SoDispCon* active;
   SoDisp* dispatcher;
   if(!self) return -1;
   memset(self, 0, sizeof(*self));
   active = (SoDispCon*)HttpRequest_getConnection(req);
   dispatcher = active->dispatcher;
   Thread_constructor(&self->thread, run, ThreadPrioNormal, BA_STACKSZ);
   if(SoDispCon_moveCon(active, &self->con) != 0) {
      Thread_destructor(&self->thread);
      baFree(self);
      return -1;
   }
   self->con.dispatcher = dispatcher;
   Thread_start(&self->thread); /* exactly once */
   return 0;
}
```

Check the target's `SoDispCon_moveCon` failure semantics and whether dispatcher data remains readable after moving. Do not retain the original request/response. Make ownership transfer explicit so normal HTTP handling does not also complete the moved connection.

## Custom I/O interfaces

Implement `IoIntf`, `DirIntf`, and `ResIntf` only when existing `DiskIo`, `ZipIo`, `NetIo`, or generated readers do not fit.

Read the callback typedefs and constructor macros in `IoIntf.h`. Define:

- read-only versus read/write operations;
- error/status and `ecode` conventions;
- path normalization and traversal policy;
- resource, directory iterator, and backing-device lifetime;
- seek/size semantics and large-file types;
- concurrency and blocking behavior;
- `propertyFp` support only for properties actually implemented.

Close every resource/directory on every error path. A reader callback returns success/failure exactly as documented; do not translate conventions from standard C I/O without checking.

## Security validation

Test at least:

- anonymous, valid, invalid, expired/revoked, and insufficient-role identity;
- every protected method, not only GET;
- path traversal, encoded separators, case rules, symlinks, overwrite, and delete;
- CSRF for browser state changes and origin policy for WebSockets;
- upload size/type/signature and interrupted upload cleanup;
- TLS trust, hostname mismatch, expired/untrusted certificate, and clock failure;
- resource exhaustion: connections, command threads, memory, storage, locks, and slow peers;
- log output for accidental credentials or secrets.
