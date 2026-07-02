# BAS/BWS Agent Guide

This repository contains the public amalgamated Barracuda App Server (BAS) and Barracuda Web Server (BWS) source package.

## Start Here

For native C or C++ BAS/BWS application work, first read:

- [skills/BAS-Skill.md](skills/BAS-Skill.md)

Use that skill when creating, modifying, or reviewing:

- BWS or BAS native C/C++ applications
- server startup and porting code
- C/C++ server-side web apps
- REST/JSON services
- WebSocket apps
- VFS layouts, static resources, generated CSP resources, WebDAV, uploads, and custom directory objects
- outbound HTTP/HTTPS client code

For Lua bindings, BAS Lua APIs, LSP integration, Mako/Xedge Lua extensions, or exposing C/C++ functions to Lua, use the public Lua binding skill:

- https://realtimelogic.com/downloads/ai-skills/Lua-Binding-Skill.md

For broader BAS skill selection, especially Lua/LSP application design where VFS, authentication, SMQ, SQLite, security, or Lua bindings may overlap, use the public BAS skill selector:

- https://realtimelogic.com/downloads/ai-skills/AGENTS.md

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
