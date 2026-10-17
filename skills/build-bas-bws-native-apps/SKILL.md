---
name: build-bas-bws-native-apps
description: Build, modify, review, port, and test native C or C++ applications using the Barracuda Web Server (BWS) or Barracuda App Server (BAS) amalgamated source. Use for HttpServer startup, virtual filesystems and routes, REST/JSON services, WebSockets, static/disk/ZIP/CSP resources, authentication, WebDAV, uploads, asynchronous responses, TLS clients or listeners, socket takeover, HLOS/RTOS porting, native build files, or custom BAS hosts that embed Lua/LSP.
---

# Build BAS/BWS Native Apps

Create small, compilable native applications from the public BAS/BWS headers and exactly one amalgamated library. Verify signatures in the target checkout instead of recreating BAS types or callbacks from memory.

Read [references/api-usage.md](references/api-usage.md) when selecting APIs or translating calls between C and C++. It maps tasks to headers, Doxygen, and the focused recipes below.

## Start With Evidence

1. Locate the active BAS checkout and read its nearest `AGENTS.md`.
2. Run `git status --short` before editing and preserve unrelated changes.
3. Classify the request before coding:
   - runtime: BWS, BAS, Mako/Xedge host, or custom embedded integration;
   - language: C or C++;
   - service: HTTP page/directory, REST/JSON, WebSocket, resources, WebDAV, upload, client, socket takeover, or Lua binding;
   - target: Windows/POSIX HLOS, embedded Linux, RTOS, or firmware;
   - resources: disk, embedded ZIP, generated CSP, network development tree, or none;
   - blocking and thread ownership: dispatcher, HTTP command thread, worker, or ISR/driver callback.
4. State the selected pattern, amalgamation, porting layers, and resource strategy.
5. Read the exact public headers for every object and callback used.

Use these source priorities:

1. User constraints and target-specific build files.
2. Public headers under `inc/`.
3. Current source and generated declarations in the target checkout.
4. Focused examples and C documentation.
5. This skill's distilled guidance.

Read [references/source-map.md](references/source-map.md) when locating APIs or examples, or when the full-source and public amalgamated trees differ.

## Preserve BAS Source Formatting and Copyright

- In new or modified C/C++ control-flow blocks, put opening and closing braces on their own lines and each statement on its own line, using the surrounding indentation (normally three spaces in BAS). Do not compress multi-statement blocks into one line. Small code means a small implementation and binary, not fewer source lines; preserve deliberate compact argument parsing and single-character switches.

  ```c
  if(status == 0)
  {
     /* Release resources owned by this operation. */
     releaseResources(o);
     return 0;
  }
  ```

- Use POSIX LF (`\n`) line endings in BAS-owned source and header files, including when editing on Windows. Normalize CRLF or mixed endings in files changed for the task; do not sweep unrelated files.
- Make scripted writes explicit about newlines, for example Python `open(path, "w", encoding="utf-8", newline="\n")`, or byte-preserving replacements. Default Windows text writes can introduce CRLF, displayed as `^M` in Emacs. Preserve the existing encoding and literal C escape sequences such as `"\r\n"`.
- Update the existing Real Time Logic copyright end year in each modified source/header to the current year, unless the user specifies another year. Preserve the start year, owner, license text, third-party notices, and revision metadata such as `$Id`. Do not silently move an existing later end year backwards; clarify a conflicting requested year.
- Before finishing, check the edited files as bytes for CRLF and stray carriage returns, and inspect the copyright lines. For line-ending/copyright-only changes, verify that no other content changed; a rebuild or runtime test is unnecessary.

## Select One Amalgamation

| Need | Compile |
| --- | --- |
| Native HTTP(S), REST, JSON, VFS, WebSocket, WebDAV, or uploads without Lua | `src/BWS.c` |
| Lua, LSP, Mako, Xedge, BAS-only APIs, or native bindings into a BAS Lua state | `src/BAS.c` |

Never compile both. Do not compile separate component source already present in the chosen amalgamation unless the package build explicitly identifies it as an external optional module.

Treat direct C services and optional Modern C++ convenience layers as complementary choices. Prefer direct C when the target is C-only or every byte of additional code and runtime support matters. Use a C++ layer when its routing, validation, and ownership helpers justify the compiler and footprint cost. Measure the actual linked image/map for the target instead of declaring either approach universally superior.

Add the target's thread, socket-dispatcher, and optional filesystem porting sources. Read [references/build-porting.md](references/build-porting.md) before creating or changing a build, porting to another OS/RTOS, choosing compiler macros, or integrating generated web UI resources.

## Keep the Implementation Small

- Implement the smallest design that satisfies the requested behavior,
  correctness, security, ownership, cleanup, and established compatibility
  contracts.
- Leave out optional features, speculative extension points, compatibility
  aliases, configuration switches, fallbacks, dependencies, wrappers, and
  abstraction layers unless the user or an existing contract requires them.
- Prefer direct public BAS/BWS APIs and a narrow callback interface. Do not add
  a framework, portability layer, or generalized mechanism for one known
  target or call site.
- Add a source file or module only when it creates a useful ownership,
  portability, reuse, test, or security boundary. Keep closely related code
  together when splitting it would add indirection without reducing risk.
- After the required path works, remove unused branches, fields, functions,
  includes, allocation paths, and duplicated validation introduced by the
  change. Never remove required bounds checks, error handling, synchronization,
  or destruction logic to reduce code size.
- Preserve an established public or wire contract unless the user authorizes
  its replacement. Compatibility code is not required for contracts outside
  the requested scope.

When the task includes documentation or a handover, give every public
argument, variable, structure member, callback value, return value, and
configuration field an explicit type. State optional status, defaults,
accepted ranges, units, ownership, and lifetime where relevant. Every code
or configuration example must include a useful comment explaining intent,
prerequisites, ownership, or a non-obvious choice. Comments must not merely
translate the code into prose.

## Use a Stable Application Shape

Prefer these modules:

- `main.c` or target startup: initialize the platform and call `barracuda()` from the correct task/thread.
- `server.c` or `server.cpp`: own `ThreadMutex`, `SoDisp`, `HttpServerConfig`, `HttpServer`, listeners, and optional `HttpCmdThreadPool`.
- `app.c` or `app.cpp`: export `installVirtualDir(HttpServer*)` and own persistent VFS nodes and backing I/O.
- service modules: REST, WebSocket, upload, authentication, or protocol logic.
- generated resources: embedded ZIP/CSP sources when required.

Keep server, dispatcher, listener, pool, directories, pages, authenticators, readers, and backing I/O alive until explicitly unlinked or the server stops. Static or application-owned storage is normally correct. Do not mount stack-local objects whose functions return.

Configure `HttpServerConfig` before constructing `HttpServer`. Ensure one-time constructors and worker starts cannot execute twice.

Read [references/core-server-vfs.md](references/core-server-vfs.md) for startup order, routing contracts, request/response APIs, priorities, static resources, and generated CSP integration.

## Route and Respond Correctly

For `HttpDir_Service`:

- return `0` only after handling the request or sending a rejection;
- return a negative value, normally `-1`, when this node does not own the path;
- do not send a premature 404 when VFS lookup should continue;
- treat `cmd == NULL` as a possible destruction callback when the selected service pattern documents it.

For `HttpPage_Service`, complete the response in the callback or successfully morph the request into the intended persistent protocol.

Set status and content type before writing. Do not retain `HttpCommand`, `HttpRequest`, `HttpResponse`, request headers, or request-backed buffers beyond the request. Copy data needed by workers.

## Choose the Relevant Recipe

- REST, JSON, request bodies, response encoding, or WebSockets: read [references/http-json-websocket.md](references/http-json-websocket.md).
- Authentication, authorization, TLS, uploads, asynchronous responses, WebDAV, custom `IoIntf`, outbound clients, or socket takeover: read [references/security-io-async.md](references/security-io-async.md).
- Lua/C bindings: use `src/BAS.c`, then read [Lua binding guidance](../build-xedge-mako-lua-apps/references/upstream/Lua-Binding-Skill.md) and, for peripherals, [hardware binding guidance](../build-xedge-mako-lua-apps/references/hardware-bindings.md). Never call the Lua C API while the BAS dispatcher mutex is released.

## Enforce Ownership and Concurrency

- Protect dispatcher-owned state from external threads with the dispatcher mutex.
- In C, use `ThreadMutex_set`/`ThreadMutex_release`. In C++, use `ThreadLock`; use `ThreadReleaseLock` only around non-BAS blocking work while already holding the mutex.
- Assume socket/WebSocket writes may release the dispatcher mutex. Revalidate objects and iterators afterward.
- Queue multi-client broadcasts to a worker instead of blocking a receive callback.
- Start each thread exactly once and destroy moved socket connections in their owner thread.
- Never invoke BAS/BWS APIs from an ISR. Queue work to a normal task.
- Do not throw C++ exceptions through C callbacks or BAS destructors.
- Add `extern "C"` for C++ entry points called by C startup code.

## Build and Verify

Compile the smallest relevant target first. Treat warnings as findings; require a zero compiler and linker exit code.

Verify behavior with real protocol traffic:

- HTTP/REST: method, path, status, content type, body, limits, malformed input, and missing route.
- WebSocket: real upgrade, text and binary frames as applicable, two clients for broadcast, close, malformed/oversized input, and reconnect.
- Authentication: anonymous, valid, invalid, insufficient role, method restrictions, and direct protected-resource access.
- Upload/WebDAV: authorized read/write, size and path failures, interruption, cleanup, and concurrency.
- TLS client/server: trust result, hostname validation, failure with an untrusted peer, and no sensitive data before trust.
- Embedded resources: validate generated readers, ZIP/CSP mounting, missing/corrupt resource behavior, and firmware build inclusion.
- RTOS: deploy to the actual target; a host build proves only portable logic.

Inspect trace output and use sanitizers, leak checks, or platform diagnostics when available. Report exact commands, target, compiler, runtime checks, and anything not exercised.

## Review Gate

Before finishing, confirm:

- exactly one of `BWS.c` or `BAS.c` is compiled;
- include paths and porting sources match the target;
- configuration precedes server construction;
- mounted objects and name strings have persistent lifetime;
- callback signatures and return values match headers;
- constructors/destructors and allocation/free pairs match;
- thread and dispatcher mutex ownership is explicit;
- untrusted input is bounded and contextually encoded;
- credentials, keys, generated ZIPs, and test output were not unintentionally committed;
- the changed behavior was compiled and exercised through its real protocol.

Stop and report instead of inventing an API when the target header lacks the required operation, the example conflicts with the header, target hardware or credentials are unavailable, or validation would require unauthorized destructive changes.
