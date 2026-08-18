---
name: bas-vfs-builder
description: Use when building BAS, Mako Server, or Xedge applications that need virtual file system composition, resource readers, directory callbacks, mounted subdirectories, fallback handlers, LSP-enabled resources, or authentication-ready layout.
---

# BAS Virtual File System Skill

Use this skill when designing routing for web applications. The goal is to compose routing, static files, and authentication boundaries with BAS virtual file system primitives instead of hiding those decisions inside one large request handler.

If you are new to BAS: the VFS is the server-side request routing tree. A directory object is a routing or mount point, a resource reader maps an IO tree such as a ZIP, disk directory, or embedded resource set to URLs, and LSP files are server-side Lua pages served through that tree.

## Standalone Use And Local Cache

This skill may be downloaded without
`https://realtimelogic.com/downloads/ai-skills/AGENTS.md`. When this file
references another public BAS document or BAS AI skill, resolve it in this order:

1. A same-directory file with the referenced filename.
2. `./.agents/reference/rtl/` relative to this file.
3. The public source URL.

If a referenced file is missing and network access is available, download it
from the public URL and save a local copy before using it. Record the source URL
and download date in `./.agents/reference/rtl/manifest.md` or in a short header
in the cached file.

Use local copies for normal work. Re-fetch only when the user asks for
current/latest guidance, the local copy is missing, or the local copy conflicts
with observed API/runtime behavior.

For broader OWASP-style review of routed web surfaces, uploads, REST APIs, WebDAV/WFS, browser sessions, secrets, TLS, logging, and error handling, also use https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md.

## Core Model

BAS applications are mounted as a chain of virtual directory instances. A directory instance can be a resource reader, a callback directory, an authentication/prologue directory, or another directory tree. Requests move through this chain until a directory handles the resource.

Key objects:

- `io`: the application IO object provided by the BAS-derived runtime.
- `dir`: the default resource reader/directory object provided to `.preload`.
- `ba.openio(ioname)`: opens an installed Barracuda I/O object, such as a runtime-provided `home` IO.
- `ba.mkio(baseIo, path)`: creates a new IO rooted below another IO.
- `ba.create.resrdr([name][, priority], io)`: creates a static resource reader from an IO, optionally mounted below a URL name and/or assigned a search priority.
- `ba.create.dir([name][, priority])`: creates a virtual directory, optionally mounted as a named subdirectory or assigned a priority. A numeric first argument is used for an unnamed priority directory in BAS examples.
- `ba.create.wfs([name][, priority], io [, lockdir] [, maxuploads, maxlocks])`: creates a Web File Server directory node for browser-based upload/download and WebDAV access.
- `ba.create.dav([name][, priority], io [, lockdir] [, maxuploads, maxlocks])`: creates a WebDAV directory node for remote filesystem access.
- `dir:setfunc(fn)`: assigns a request callback to a directory.
- `dir:baseuri()`: returns the installed base URI for a directory.
- `dir:insert(child [, reference])`: inserts a child directory. Set `reference` true when the parent should keep the child from being garbage collected.
- `dir:insert()`: inserts the directory as a root directory in the server VFS. Keep a Lua reference to root-inserted directories so they are not garbage collected.
- `dir:unlink()`: removes a directory from the VFS chain. It is commonly used during unload cleanup, but can be called at runtime to dynamically remove a mounted directory.
- `rsrdr:lspfilter([appTable])`: enables LSP processing for resources served by a resource reader and optionally exposes an app table to those pages.
- `rsrdr:insertprolog(child [, reference])`: inserts a child directory in the resource reader prologue list, which is searched before static resources.

Keep references to created directories in Lua variables or on `app` unless the directory is a child inserted with `reference` set to true. BAS directory objects should not be allowed to become unreachable while mounted.

## Search Order And Priority

The VFS may contain multiple root directories or child directories with the same name. Search order is controlled by directory priority:

- Default priority is `0`.
- Valid priority range is `-127` through `127`.
- Higher-priority directories are searched first.
- If a directory returns `false`, the VFS continues to the next matching directory.
- If a directory handles the request, returns `true`, or returns no value, the VFS stops searching.

Use priorities intentionally:

- Higher-priority resource readers can override packaged defaults with user-provided files.
- High-priority callback directories can act as filters or authentication gates.
- Low-priority callback directories can act as catch-all 404 handlers.

## Callback Contract

A directory callback receives the request command environment and the relative path for that directory:

```lua
local appDir = ba.create.dir()
appDir:setfunc(function(_ENV, relpath)
   if relpath == "" then
      response:write("handled")
      return true
   end
   return false
end)
```

Rules:

- Return `true` only after the callback has fully handled the request.
- Return `false` when the resource is not found by this callback and the VFS should continue searching. Do not rely on `nil` or no return value for this; the documented default is that the resource was found and the VFS search stops.
- Do not send a 404 merely because this callback did not recognize a path. Let the VFS chain continue.
- Do not put unrelated sub-app dispatch in a root callback when a named subdirectory can be mounted instead.
- Keep request-specific state in the command environment or a per-page/request table, not in shared globals.

## Resource Reader Before Dynamic Fallback

Use this when static assets should take precedence over dynamic routes.

```lua
local app = app or {}

local rootIo = ba.openio("home") or io
local assetsName = app.assetsDirName or "assets"

assert(rootIo:stat(assetsName) or rootIo:mkdir(assetsName))
local assetsIo = assert(ba.mkio(rootIo, assetsName))
local assetsDir = assert(ba.create.resrdr(0, assetsIo))
assetsDir:lspfilter(app)

local dynamicDir = ba.create.dir()
dynamicDir:setfunc(function(_ENV, relpath)
   return router(_ENV, relpath, app)
end)

assetsDir:insert(dynamicDir, true)
assetsDir:insert()

function onunload()
   assetsDir:unlink()
end
```

With this structure, BAS first checks `assets/`. If no asset matches, the unnamed dynamic directory acts as the application fallback.

An unnamed child directory is treated as a sibling/fallback associated with the parent directory, not as a URL subdirectory. Unlinking the parent also removes its unnamed sibling. Use this when a dynamic fallback should run only after the parent resource reader fails to find a file.

## Fallback And Filter Directories

Use a low-priority callback directory for custom 404 handling:

```lua
local notFoundDir = ba.create.dir(-126)
notFoundDir:setfunc(function(_ENV, relpath)
   response:setstatus(404)
   response:write("Not found")
   return true
end)
notFoundDir:insert()
```

Use a high-priority callback directory as a filter or authorization boundary. If the filter approves the request, return `false` so the VFS continues to the protected resource. If the filter rejects the request, send the response and return `true`.

```lua
local filterDir = ba.create.dir(100)
filterDir:setfunc(function(_ENV, relpath)
   if isAllowed(_ENV, relpath) then
      return false
   end
   response:setstatus(401)
   response:write("Unauthorized")
   return true
end)
filterDir:insert()
```

## Named Subdirectories

Use named directories for independent application sections such as `/admin/`, `/api/`, `/docs/`, or `/rpc/`.

```lua
local adminDir = ba.create.dir("admin")
adminDir:setfunc(function(_ENV, relpath)
   return adminHandler(_ENV, relpath, app)
end)

assetsDir:insert(adminDir, true)
```

The callback receives paths relative to `/admin/`. For example:

- `/admin/` gives `relpath == ""`.
- `/admin/content/` gives `relpath == "content/"`.
- `/admin/admin.css` should return `false` if the callback does not own it, allowing static fallback when the file exists.

## Fine-Grained Routing Inside A Directory

For REST-style APIs, mount one named VFS directory such as `/api/`, then dispatch method and path combinations inside that directory callback. This keeps the VFS responsible for coarse application boundaries while a small router handles endpoint-level routing.

Use this pattern when one application boundary owns many related endpoints, such as:

- `GET /api/users`
- `POST /api/users`
- `GET /api/users/{id}`
- `PUT /api/users/{id}`
- `DELETE /api/users/{id}`

The REST example demonstrates this pattern by mounting a reusable Lua router under `/api`, using exact and wildcard route matching, returning JSON responses, and returning `false` when no route matches so the VFS can continue searching:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/REST

## Protocol And Service Endpoints

Use named VFS directories for protocol or service endpoints that need to take over an HTTP request, such as SMQ, CGI, WebSocket, or custom service adapters. The directory callback should hand the request to the service and return `true` when the request is accepted.

```lua
local smqDir = ba.create.dir("smq")
smqDir:setfunc(function(_ENV, relpath)
   smq:connect(request)
   return true
end)
dir:insert(smqDir, true)
```

Examples:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/SMQ-examples
- https://github.com/RealTimeLogic/LSP-Examples/tree/master/CGI

## Authentication Boundaries

Authentication belongs on VFS directory boundaries, not scattered through individual route handlers. Keep this skill focused on where authentication is installed in the VFS tree; use the BAS authentication documentation for authenticator and authorizer implementation details.

For full application protection, apply authentication directly to the app's resource reader:

```lua
dir:setauth(authenticator, authorizer)
```

Use a named directory when an entire logical section should be protected:

```lua
local adminDir = ba.create.dir("admin")
adminDir:setfunc(adminHandler)
adminDir:setauth(authenticator, authorizer)
parentDir:insert(adminDir, true)
```

When protecting a subdirectory served by a resource reader, use a prologue directory. A prologue directory is searched before the resource reader serves static or LSP resources, so it can enforce authentication for only that subtree:

```lua
local adminGate = ba.create.dir("admin")
adminGate:setauth(authenticator, authorizer)
rsrdr:insertprolog(adminGate, true)
```

This pattern protects resources under `/admin/` while leaving the rest of the resource reader public.

Example:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/authentication

## Mount-Safe URLs

Avoid hardcoded absolute paths when generating internal links, redirects, forwards, help URLs, or logout URLs from reusable app code. Use `dir:baseuri()` on the relevant installed directory so the same code works whether the app is mounted at `/`, `/myapp/`, or another base path.

```lua
local base = dir:baseuri()
response:forward(base .. ".edit/create.lsp")
```

Examples:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/fs-sso
- https://github.com/RealTimeLogic/LSP-Examples/tree/master/SQLite

## Resource Readers And LSP

A resource reader serves files from an IO object. Calling `lspfilter(app)` lets `.lsp` files and included LSP pages run through the BAS LSP engine.

Use `lspfilter(app)` only for directories that should execute LSP. Static upload directories that must never execute code should not use an LSP filter.

When LSP pages participate in the same request through include/forward/callback chains, BAS provides a shared request/response command environment. Each physical LSP page also has a persistent page table. If rendering LSP text from a database or another virtual source, emulate this by keeping a stable page table per virtual path.

Example:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/Light-Dashboard

## Hidden Resources

Resources whose names start with a dot are hidden from direct client requests. HTML files ending in `.shtml` are also hidden. A direct browser request for `.page.lsp`, `.lua/module.lua`, `.private/`, or `fragment.shtml` should produce a 404.

Hidden resources are still available to server-side code through `response:include()` and `response:forward()`. Do not use `response:sendredirect()` or other client-side redirects for hidden resources; redirects require the browser to request the hidden URL directly.

Use hidden resources for internal LSP pages, includes, private Lua files, and implementation details that should not be addressable as public URLs.

## Home Directory And Writable Data

BAS-derived deployments commonly package an application as a ZIP file or another read-only resource tree. Packaged application IO may be read-only, so writable data should normally live in a runtime-provided writable IO such as `ba.openio("home")`, when available.

```lua
local rootIo = ba.openio("home") or io
assert(rootIo:stat("data") or rootIo:mkdir("data"))
assert(rootIo:stat("assets") or rootIo:mkdir("assets"))
```

Use separate IO roots for concerns:

- Application code and packaged defaults: `io`
- Runtime data, SQLite files, uploaded assets, selected theme: `ba.openio("home")`
- Static asset serving: `ba.mkio(rootIo, "assets")`
- Installed themes or plugins: a dedicated home subdirectory IO

## Web File Server And WebDAV

`ba.create.wfs([name][, priority], io [, lockdir] [, maxuploads, maxlocks])` and `ba.create.dav([name][, priority], io [, lockdir] [, maxuploads, maxlocks])` create VFS nodes and inherit directory behavior such as insertion, priority, and authentication. Treat them as mounted application surfaces, not as raw filesystem shortcuts.

Use these rules:

- Always protect WebDAV with authentication in production.
- WebDAV is useful for editing LSP pages on a remote server through a mounted drive.
- ZIP-backed IO is read-only for WebDAV; use a writable disk or home IO when edits must persist.
- Web File Server and WebDAV nodes should normally be mounted under named paths such as `/fs/` or `/dav/`.
- Use `wfs:insert()` when the file server should be a root VFS node such as `/fs/`.
- Use `dir:insert(wfs, true)` when the file server should be relative to the current app, such as `/myapp/fs/`.
- Use `dir:baseuri()` when configuring help, logout, or generated links for an app-relative file server.
- Place authentication at the VFS boundary for these nodes before exposing them on a network.

Example:

- https://github.com/RealTimeLogic/LSP-Examples/tree/master/File-Server

## Callback Response Basics

For dynamic callbacks:

- Set headers before writing response bodies.
- Use `response:setstatus(code)` before writing custom status responses such as 304 or JSON/text 404 bodies.
- Use `response:senderror(code [, message])` for simple standard error pages.
- Use `response:sendredirect(url [, permanent [, return2caller]])` for redirects.
- Use `request:checktime(time)` or explicit validators for browser cache revalidation.

## Practical Checklist

Before implementing:

- Decide which paths are static resources, dynamic routes, admin routes, and APIs.
- Mount each independent section as a named VFS directory.
- Set explicit priorities when multiple nodes can match the same path.
- Put authentication at directory boundaries.
- Use `dir:baseuri()` for generated links or forwards from code that may run under different mount points.
- Let callbacks return `false` for resources they do not handle.
- Use low-priority fallback directories for custom 404 behavior.
- Use high-priority filter directories for request screening that may continue by returning `false`.
- Keep hidden dot resources accessible only through server-side include/forward flows.
- Keep writable runtime state under `ba.openio("home")`.
- Enable `lspfilter(app)` only where LSP execution is intended.
- Protect WebDAV/Web File Server nodes before network exposure.
- Keep an `onunload()` function that unlinks mounted directories and closes runtime resources.

After implementing:

- Request a known static resource.
- Request a dynamic page.
- Request a named subdirectory route.
- Request a missing route and confirm normal 404 behavior.
- Verify static fallback still works under named subdirectories.
- Verify authentication applies only to the intended subtree.
