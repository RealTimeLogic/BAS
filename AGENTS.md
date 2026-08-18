# BAS/BWS Agent Guide

This repository contains the public amalgamated Barracuda App Server (BAS) and Barracuda Web Server (BWS) source package.

## Start Here

For native C or C++ BAS/BWS application work, first read:

- [skills/build-bas-bws-native-apps/SKILL.md](skills/build-bas-bws-native-apps/SKILL.md)

Use that skill when creating, modifying, or reviewing:

- BWS or BAS native C/C++ applications
- server startup and porting code
- C/C++ server-side web apps
- REST/JSON services
- WebSocket apps
- VFS layouts, static resources, generated CSP resources, WebDAV, uploads, and custom directory objects
- outbound HTTP/HTTPS client code

For Lua and Lua Server Pages applications targeting Mako Server, Xedge, or Xedge32, first read:

- [skills/build-xedge-mako-lua-apps/SKILL.md](skills/build-xedge-mako-lua-apps/SKILL.md)

Use that skill for application lifecycle, VFS routing, LSP/browser interfaces, authentication, SMQ, SQLite, Xedge packages, Mako deployment, and Lua/native bindings. It vendors the applicable official API and specialist skill components and explains how to refresh them.

For a native Lua/C/C++ binding, use both local skills: the Lua skill controls the Lua application contract and the native C/C++ skill controls the BAS ABI, ownership, and build integration.

## Choose an Example

Select an example before copying code or build settings. Read its README and the applicable skill before editing.

| Example | Use it when | Important boundary |
| --- | --- | --- |
| [C-RESTful-Service](examples/C-RESTful-Service/README.md) | Implementing REST directly with BAS/BWS APIs, using a C-only toolchain, or minimizing additional code and memory overhead. | It does not use the Modern C++ REST library. It and the Modern C++ example are alternatives; neither replaces the other. |
| [Modern-Cpp-REST-Service](examples/Modern-Cpp-REST-Service/README.md) | Typed routes, structured errors, streaming bodies/responses, a modern C++ interface, or the embedded Svelte device UI are wanted. | It requires a modern C++ compiler and `cpp/rest`. Use the direct REST example when C-only support or minimum footprint dominates. |
| [C-WebSockets](examples/C-WebSockets/README.md) | Building a native C++ WebSocket service with text/binary frames, multiple clients, broadcast concurrency, or disk/embedded browser resources. | Use the Lua skill when the WebSocket behavior belongs in a Mako/Xedge Lua application rather than a native service. |
| [MakoServer](examples/MakoServer/) | Building or customizing the native standalone Mako executable, platform startup, Windows service support, native extensions, or Mako resource packaging. See [Mako Server compilation](doc/compiling.md#mako-server-hlos). | This is the native Mako host implementation, not the starting point for an ordinary Lua/LSP application running on Mako. |
| [xedge](examples/xedge/) | Porting or embedding the native Xedge runtime into firmware/RTOS, or integrating storage, hardware hooks, embedded resources, and native extensions. See [Xedge compilation](doc/compiling.md#xedge-rtos). | This is the native Xedge host. Use the Lua skill for `.preload`, `.lua`, `.config`, `.xlua`, and LSP application work. |

Use `build-bas-bws-native-apps` for the native examples. Use `build-xedge-mako-lua-apps` for Lua/LSP applications running on Mako, Xedge, or Xedge32. Use both when changing a native Mako/Xedge host and its Lua integration boundary.

`examples/HostInit` is shared HLOS startup scaffolding, not a standalone application choice. Use it only when the selected host example or build documentation calls for it, and replace it with target-specific startup for RTOS or firmware integrations.

## Core Rules

- Use public headers in `inc/` as the source of truth. Do not recreate structs, callback typedefs, or object layouts from memory.
- Compile exactly one amalgamated library: `src/BWS.c` for native BWS work, or `src/BAS.c` when BAS/Lua/LSP features are required.
- Keep inserted `HttpDir`, `HttpPage`, resource readers, authenticators, listeners, dispatchers, and backing `IoIntf` objects alive for the server lifetime or until explicitly unlinked.
- For VFS work, preserve C/C++ callback semantics: return `0` when handled and a negative value, normally `-1`, when the VFS should continue searching.
- For C++ code called from C startup or callbacks, use `extern "C"` where required and do not throw exceptions through BAS/BWS C callbacks.
- Prefer small, compilable changes and verify against the target porting layer whenever possible.

## Build Orientation

The normal build shape is:

- one amalgamated source: `src/BWS.c` or `src/BAS.c`
- one OS thread layer: `src/arch/<OS>/ThreadLib.c`
- one socket dispatcher/TCP layer: `src/arch/NET/<NET>/SoDisp.c` or the target-specific equivalent
- optional filesystem layer: `src/DiskIo/<FS>/BaFile.c`
- application source files and any generated resources

Do not assume host examples apply unchanged to RTOS or firmware targets. Replace host startup with the target's task/thread and network initialization model.
