---
name: build-xedge-mako-lua-apps
description: Create, modify, review, package, deploy, and test Lua and Lua Server Pages applications for Mako Server, Xedge, and Xedge32. Use for .preload, .lua modules, .config packages, .xlua programs, LSP pages, BAS Lua VFS routes, browser interfaces, authentication, SMQ, SQLite, OPC UA, or Mako/Xedge application lifecycle work. Use the native BAS/BWS skill instead for C/C++-only server applications.
---

# Build Xedge and Mako Lua Apps

Build small, lifecycle-correct BAS Lua applications against the selected runtime and the vendored official API references. Keep Mako, Xedge, and Xedge32 capabilities distinct and verify the real application root before editing or running anything.

## Start With the Application

1. Read the nearest `AGENTS.md`, the selected application's `README.md`, and any files they identify as required.
2. Run `git status --short` and preserve unrelated work.
3. Identify:
   - runtime: Mako Server, Xedge, or Xedge32;
   - runnable root: directory, `www/`, variant directory, or ZIP;
   - application form: `.preload`, `.lua`, `.config`, `.xlua`, LSP, static browser assets, or a combination;
   - interface: full-page LSP, htmx fragment, fetch/REST, WebSocket, SMQ, file service, or background protocol client;
   - state owner and cleanup path;
   - required writable storage, network services, credentials, certificates, or hardware.
4. Validate the vendored upstream bundle before relying on it:

   ```text
   python scripts/update_components.py --check
   ```

   Resolve `scripts/` relative to this `SKILL.md`, not the application repository.
5. Search the applicable API source for every unfamiliar operation and read the surrounding contract. Do not guess an API from an example.

## Select the Runtime Deliberately

| Runtime | Appropriate use | Do not assume |
| --- | --- | --- |
| Mako Server | Desktop/server development, embedded Linux, services, VPS deployment, and testing compatible BAS apps | Xedge package management or Xedge32 hardware APIs |
| Xedge | Product-integrated BAS application packages, `.config`, `.preload`, LSP, and independent `.xlua` programs | Mako process/service controls or unrestricted host filesystem access |
| Xedge32 | Xedge application model on ESP32 with hardware APIs | Generic Xedge/Mako behavior proves board, timing, storage, or electrical behavior |

Mako is often the quickest development host, but test the final Xedge/Xedge32 package on its target. A desktop simulation validates application logic only.

## Route to Vendored Components

The component list, source URLs, and refresh metadata live in [references/upstream-sources.json](references/upstream-sources.json) and [references/upstream/manifest.json](references/upstream/manifest.json). Load only what the task needs:

| Task | Upstream source to consult |
| --- | --- |
| Any BAS Lua/LSP/Mako/Xedge API | Search relevant definitions in [basapi.md](references/upstream/basapi.md) |
| Xedge32 or ESP32 hardware | [esp32api.md](references/upstream/esp32api.md) |
| OPC UA Client, Server, or PubSub | [opcuaapi.md](references/upstream/opcuaapi.md) |
| VFS mounts, routing, resource readers, WFS/WebDAV, base URLs | [VFS-skill.md](references/upstream/VFS-skill.md) |
| Authentication or authorization | [Authentication-Authorization-Skill.md](references/upstream/Authentication-Authorization-Skill.md) |
| Public, administrative, upload, file, or device-control security | [OWASP-General-Security-Skill.md](references/upstream/OWASP-General-Security-Skill.md) and the authentication skill when identity is involved |
| SMQ topics, presence, or device messaging | [SMQ-Skill.md](references/upstream/SMQ-Skill.md) |
| SQLite schema or writes | [SQLite-Skill.md](references/upstream/SQLite-Skill.md) |
| Lua/C/C++ bindings | [Lua-Binding-Skill.md](references/upstream/Lua-Binding-Skill.md), plus the repository's native BAS/BWS skill |
| Installing or operating Mako Server | [Deploy-Mako-Server-Skill.md](references/upstream/Deploy-Mako-Server-Skill.md) |
| LSP, htmx, fetch, WebSocket RPC, or SMQ browser UI | [Build-LSP-Web-Interfaces-Skill.md](references/upstream/Build-LSP-Web-Interfaces-Skill.md) |

Official API references control names, signatures, and runtime behavior. If an example or skill conflicts with the applicable API source, report the conflict and follow the runtime-correct API.

Paths such as `inc/balua.h` inside an upstream component are relative to the target BAS checkout, not `references/upstream/`. The consolidated API bundles may also retain relative links from their original generated HTML documentation; search the downloaded Markdown bundle for the referenced symbol or section when such a navigation link has no vendored target.

## Preserve the BAS Application Model

- `.preload` owns application startup, shared services, mounts, timers, databases, and cleanup.
- `.lua/` contains private server-side modules.
- `.config` defines Xedge package metadata and startup/base-URL behavior.
- `.xlua` is an independent Xedge/Xedge32 background program with its own lifecycle.
- `*.lsp` executes per request and may mix Lua with response content.
- Static HTML, CSS, JavaScript, images, and fonts are browser resources.

Keep databases, logs, uploads, secrets, keys, and mutable configuration in writable data/home I/O, not inside a deployed application ZIP.

Use `appreq"module"` for application-private modules and `app.appreq"module"` when an LSP page loads one directly. Use normal `require` for built-in or deliberately process-shared modules. Do not add `mako.createloader(io)` to new applications merely to make private `require` calls work.

The creator of a mount, timer, thread, socket, database connection, subscription, or protocol client owns its cleanup. Provide `onunload()` where the application model requires it, unlink mounts, stop owned activity, and verify reload does not duplicate resources. Never retain request, response, command-environment, or request-reader objects after the request ends.

Keep state in the narrowest valid scope:

- request environment: one request and its includes/forwards;
- `page`: one LSP resource across requests, never per-user secrets;
- `app`: one application instance and its owned services;
- session: one browser/user session;
- `_G`: deliberate process-global facilities only.

## Route and Respond Correctly

- Prefer a named `ba.create.dir("name")` for a real sub-application or security boundary.
- Return `true` only when a Lua directory callback handled the request. Return `false` when lookup should continue; do not rely on `nil` or send a premature 404.
- Use priority only for deliberate filters, overrides, or fallbacks.
- Use `dir:baseuri()` and mount-aware URL helpers instead of assuming `/`.
- Keep `.preload`, `.lua`, databases, keys, and private configuration outside public resource space.
- Mount authentication and authorization at the directory boundary they protect. Hidden UI controls are not authorization.

Choose the smallest browser/protocol interface that fits:

- ordinary navigation: full-page LSP;
- one-region update: htmx plus an HTML fragment;
- programmatic request/response: fetch with a defined JSON, text, or binary contract;
- correlated persistent calls: WebSocket RPC with IDs, timeouts, disconnects, and error envelopes;
- topics, presence, and synchronized devices: SMQ with defined topics, payload types, authorization, reconnect, and state restoration.

Do not introduce JSON, WebSocket, or SMQ merely to avoid a page reload. Keep rendering in the browser and validation, authority, device logic, and secrets on the server.

## Bound Input, State, and Blocking Work

Define method/path or topic, input shape, size limits, authorization, output, errors, timeout, retry, idempotency, and side effects before implementation. Treat every client, file, database, network, and device input as untrusted and encode output for its actual context.

Use `request:rawrdr()` for one streamable request payload and multipart handling only when the protocol needs multipart fields/files. Do not use GET for state changes. Keep request handlers bounded and use deferred response handling only under its documented ownership/threading contract.

For SQLite, serialize schema changes and writes through one dedicated BAS writer thread per database. Let it own one persistent write connection, keep transactions short, and never use a request's normal response object from that writer.

For native bindings, use target headers, define ownership/GC/callback references/blocking/unload order, and never call the Lua C API while the BAS dispatcher mutex is released. Route C/C++ implementation details through `../build-bas-bws-native-apps/SKILL.md`.

## Package and Verify

For Xedge packages, place `.config` at the ZIP root and avoid an unintended leading directory. Keep generated packages, databases, credentials, reference caches, and test output out of Git unless explicitly requested.

Verify in proportion to the change:

- startup, trace output, unload/reload, and restart without duplicate resources;
- HTTP method, path, status, content type, body, validation, authorization, and side effects;
- browser navigation, forms/fragments, responsive layout, console, network failures, and one failure path;
- database persistence, writer ownership, concurrency, cleanup, and restart;
- real WebSocket/SMQ handshake, two-client behavior when applicable, reconnect, malformed input, and denied actions;
- Xedge ZIP structure, install/start, base path, unload, and replacement;
- target deployment for Xedge32 hardware or external industrial protocols;
- foreground Mako startup before service installation, then service identity, paths, listeners, certificates, logs, stop/start, and boot behavior.

A successful file write, HTTP 200, or package load does not prove the intended Lua code executed. Inspect BAS/Mako/Xedge trace output after the relevant action.

## Refresh Upstream Components

Vendored components make the skill usable offline. Do not silently refresh them during an unrelated application edit. Refresh when the user requests it, when maintaining this skill, or when a material source conflict suggests the snapshot is stale:

```text
python scripts/update_components.py --refresh
python scripts/update_components.py --check
```

Use `--component <filename>` to refresh or check selected components. The updater downloads to temporary files, rejects empty/HTML responses, validates every local component, replaces only changed files, and regenerates the hash manifest. Review every resulting diff; an upstream change is not permission to rewrite application code.

## Stop and Report

Stop instead of inventing a workaround when the official API is missing or contradictory, the target/runtime/app root is ambiguous, required hardware/services/credentials are unavailable, or validation needs unauthorized destructive changes.

Report the selected runtime and app root, files changed, exact tests and runtime used, upstream conflicts, and anything not exercised.
