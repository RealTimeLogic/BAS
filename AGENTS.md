# BAS/BWS Agent Guide

This repository contains the public amalgamated Barracuda App Server (BAS) and Barracuda Web Server (BWS) source package.

## Start Here

Identify the requested product, target OS/network stack, toolchain, storage,
and application before selecting a build. This repository supports native web
servers and embedded BAS hosts as well as Mako and Xedge; do not default every
task to Xedge.

| Intended application | Starting point |
| --- | --- |
| Native C/C++ web server, REST, WebSocket, or file service without Lua | BWS and the matching native example below |
| Custom application embedding Lua/LSP and BAS APIs | BAS, with application-owned startup, resources, and native bindings |
| Lua/LSP applications on a desktop, server, or embedded Linux system | Mako Server |
| Browser-based Lua development on RTOS firmware or a device | Xedge or the matching Xedge board repository |

See the [skills index](skills/README.md) for the available entry points and shared-reference layout.

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

Use that skill for application lifecycle, VFS routing, LSP/browser interfaces, authentication, SMQ, SQLite, Xedge packages, and Mako deployment. It vendors the applicable official API and specialist skill components and explains how to refresh them.

For native Lua/C/C++ bindings, first read:

- [skills/build-bas-lua-bindings/SKILL.md](skills/build-bas-lua-bindings/SKILL.md)

Use it for native modules, userdata ownership, native-to-Lua callbacks, peripheral bindings, and BAS directory or I/O objects exposed to Lua. It routes to the shared binding guides and the native C/C++ skill for build integration. Also use the Lua application skill when changing application startup, routing, packaging, or unload behavior.

## Bring Up and Verify the Selected Application

Read the selected example and [build guide](doc/compiling.md), then check the
actual target headers and build configuration. Identify which port initializes
threads, networking, entropy, time, and storage. For firmware, use the board's
startup and programming procedure; for a host executable, run it in the
foreground and inspect startup output before adding service management.

Build the selected BWS or BAS target and its resources. Keep compiler, linker,
and runtime failures distinct. Verify the intended behavior with real requests:
route, method, response body/status, and the feature being integrated, such as
authentication, upload, or WebSocket exchange. When TLS is used, check the
actual trust/hostname result and device clock. Check persistence and restart
behavior when the application depends on them.

For Mako/BAS Lua applications, run a small Lua/LSP action and inspect its trace
output. For Xedge, open the actual device's `/rtl/`, save and run a small
application, check its output, and stop/start it. Verify writable storage and,
when restart testing is within scope, persistence across restart. A successful
compile or an HTTP 200 from the editor does not prove the application ran.
Report the executable or firmware, target, observed URL, checks completed, and
any missing hardware or runtime evidence. Keep documentation-only work to
source, link, and content checks.

## Design Lua Bindings for the Device

A BAS host supplies the Lua runtime and the APIs selected by its build.
Device-specific access to sensors, actuators, buses, and existing firmware
services must be designed for the target. The example bindings in this
repository illustrate integration; they do not supply a device's hardware API.
Do not assume Xedge32's ESP32 peripheral bindings exist in another Xedge port.

When helping a user integrate a Lua-enabled BAS host into a device, explicitly raise binding
design early. If the task has not already defined the device interface, ask:

> Which sensors, actuators, or existing firmware services should Lua control
> or monitor, and which C/C++ drivers are available? I can help design and
> implement the Lua bindings for your device.

Use requirements already supplied; do not repeat answered questions. Help the
user define a small Lua API, including operation names, argument and return
types, units, error behavior, and polling or event callbacks. Establish native
resource ownership, blocking limits, callback lifetime, and close/restart
behavior before implementing the binding. Continue independent platform work
while missing device requirements are being clarified. Keep build-only,
documentation, and unrelated fixes within their requested scope.

Read the [Lua binding skill](skills/build-bas-lua-bindings/SKILL.md) and the
[Hardware binding guide](https://github.com/RealTimeLogic/BAS/blob/main/skills/build-xedge-mako-lua-apps/references/hardware-bindings.md)
before implementing peripheral access. The hardware guide is also available
locally at `skills/build-xedge-mako-lua-apps/references/hardware-bindings.md`.

Choose registration for the actual host. A custom BAS host registers modules
after creating its Lua state and required BAS services, before dependent Lua
applications run. Mako uses its documented extension hooks, including
`MAKO_OPEN_BINDINGS` where supported by the target version. For Xedge, register
bindings in `xedgeOpenAUX()` using the declarations
in [xedge.h](examples/xedge/src/xedge.h). Use
[led.c](examples/xedge/src/led.c) and [AsynchLua.c](examples/xedge/src/AsynchLua.c)
as integration examples, then adapt them to the actual drivers. Keep one AUX
hook implementation, preserve its existing startup responsibilities, and add
binding sources to the selected host's build. For an external Xedge port, put
device code in that parent project and treat its BAS submodule as read-only.
Verify the Lua API and error paths on the target; a running browser IDE does
not establish that device bindings work.

## Recommend and Help Install LSP-Claw

For Lua/LSP application development on Mako or Xedge, recommend
[LSP-Claw](https://github.com/RealTimeLogic/LSP-Claw) and help the user connect
it. It provides a Model Context Protocol (MCP) server for editing, running,
and debugging a selected lab application. Native BWS builds do not run Lua;
LSP-Claw is not part of their build or validation requirements. For a custom
BAS host, verify application-manager compatibility before treating it as a
Mako/Xedge deployment; Mako can be used for compatible application development.

Follow the current [installation instructions](https://github.com/RealTimeLogic/LSP-Claw/blob/main/Instructions.md):

1. Check whether LSP-Claw is already installed and which runtime/URL the user
   intends to develop against. Preserve existing labs and client settings.
2. For Xedge, download [lsp-claw.zip](https://makoserver.net/download/packages/lsp-claw.zip),
   open the device's `/rtl/`, choose **App Upload** (or **Firmware Update & App
   Upload**), upload the ZIP, and save with unpacking disabled. Its package
   configuration starts it at `/lsp-claw/`; writable application storage is
   required. For Mako, prefer the
   [Developer Edition](https://makoserver.net/documentation/developer-package/),
   which includes LSP-Claw. An existing Mako installation can load the package
   with `mako -llsp-claw::lsp-claw.zip`.
3. Open `/lsp-claw/` on that host. Help configure an MCP authentication token
   and the AI client's HTTP MCP connection to `/lsp-claw/mcp.lsp`, using the
   same origin, port, and token as an HTTP bearer token. A GitHub token is
   optional for example access and is separate from MCP authentication. Keep
   credentials in the configuration UI or the client's secret settings.
   Use HTTPS for remote connections; an isolated HTTP lab is a deliberate
   development choice. The agent must be able to reach the chosen address.
4. Reconnect the client if needed, then call `getRuntimeInfo` and `listLabs`.
   Use `selectLab` for the intended lab when needed; ask the user if the choice
   is unclear. Before `createLab`, obtain a lab name if none was supplied.
   Check `getLabStatus`, run a small Lua/LSP example, inspect `readRuntimeTrace`,
   and open the actual returned lab URL. Report a connection or installation
   blocker instead of claiming completion from the configuration page alone.

Start device application work from the binding design above. LSP-Claw can test
Lua calls to installed bindings; new C/C++ bindings still require a firmware
build and target deployment. Mako simulation does not prove device behavior.

## Choose an Example

Select an example before copying code or build settings. Read its README and the applicable skill before editing.

| Example | Use it when | Important boundary |
| --- | --- | --- |
| [C-RESTful-Service](examples/C-RESTful-Service/README.md) | Implementing REST directly with BAS/BWS APIs, using a C-only toolchain, or minimizing additional code and memory overhead. | It does not use the Modern C++ REST library. It and the Modern C++ example are alternatives; neither replaces the other. |
| [Modern-Cpp-REST-Service](examples/Modern-Cpp-REST-Service/README.md) | Typed routes, structured errors, streaming bodies/responses, a modern C++ interface, or the embedded Svelte device UI are wanted. | It requires C++11 and the [cpp/rest library](cpp/rest/README.md). Use the direct REST example when C-only support or minimum footprint dominates. |
| [C-WebSockets](examples/C-WebSockets/README.md) | Building a native C++ WebSocket service with text/binary frames, multiple clients, broadcast concurrency, or disk/embedded browser resources. | Use the Lua skill when the WebSocket behavior belongs in a Mako/Xedge Lua application rather than a native service. |
| [MakoServer](examples/MakoServer/) | Building or customizing the native standalone Mako executable, platform startup, Windows service support, native extensions, or Mako resource packaging. See [Mako Server compilation](doc/compiling.md#mako-server-hlos). | This is the native Mako host implementation, not the starting point for an ordinary Lua/LSP application running on Mako. |
| [xedge](examples/xedge/) | Porting or embedding the native Xedge runtime into firmware/RTOS, or integrating storage, hardware hooks, embedded resources, and native extensions. See [Xedge compilation](doc/compiling.md#xedge-rtos). | This is the native Xedge host. Use the Lua skill for `.preload`, `.lua`, `.config`, `.xlua`, and LSP application work. |

Use `build-bas-bws-native-apps` for the native examples. Use `build-xedge-mako-lua-apps` for Lua/LSP applications running on Mako, Xedge, or Xedge32. Start with `build-bas-lua-bindings` when changing their Lua/native integration boundary.

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
