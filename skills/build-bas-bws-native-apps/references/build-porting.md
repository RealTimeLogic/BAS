# Build and Porting Guide

## Contents

- [Public package build model](#public-package-build-model)
- [Source selection](#source-selection)
- [Choosing a REST language layer](#choosing-a-rest-language-layer)
- [Host builds](#host-builds)
- [RTOS and firmware](#rtos-and-firmware)
- [Feature and resource macros](#feature-and-resource-macros)
- [Host-generated embedded web UI](#host-generated-embedded-web-ui)
- [C and C++ linking](#c-and-c-linking)
- [Build review](#build-review)

## Public package build model

The public amalgamated repository is designed around one library source plus porting layers:

```text
src/BWS.c or src/BAS.c
src/arch/<OS>/ThreadLib.c
src/arch/NET/<dispatcher-or-stack>/SoDisp.c
src/DiskIo/<filesystem>/BaFile.c       optional
application and generated resources
```

Include paths normally contain:

```text
inc
inc/arch/<OS>
inc/arch/NET/<target>
```

Add filesystem or target SDK include paths only when the chosen `BaFile.c` or TCP layer requires them.

Never infer a source path from the full internal SDK. Run `rg --files src/arch src/DiskIo` in the active public package because layouts differ.

## Source selection

- Select `src/BWS.c` for native server features with no Lua/LSP dependency.
- Select `src/BAS.c` when the product creates a BAS Lua state, runs LSP/Lua applications, hosts Mako/Xedge behavior, or exposes native functions into Lua.
- Do not link both.
- Do not add separate Lua, zlib, SharkSSL, JSON, HTTP, WebSocket, or BAS component sources that the selected amalgamation already contains.
- Add optional external modules only when the current build manifest and symbols prove they are not amalgamated.

Inspect generated resource symbols with `rg`/`dumpbin`/`nm` when duplicate or missing symbols are suspected. A dependency used by an amalgamation tool is not necessarily embedded in its generated output.

## Choosing a REST language layer

The direct C BAS/BWS API and an optional C++ REST helper solve different deployment constraints:

- choose direct C for a C-only toolchain, minimum additional code, or targets where the C++ runtime/toolchain is unacceptable;
- choose the C++ helper for typed routing, structured validation/errors, and easier application code when a modern C++ compiler is available;
- compile and compare map files on the real target when footprint decides the choice; do not infer the final cost from source size alone.

For a packaged helper beneath `cpp/rest/inc/bas/rest`, add `cpp/rest/inc` to the include search path and include `<bas/rest/Rest.h>`. Never place the checkout's Git-root directory name in public includes.

## Host builds

Representative POSIX BWS build:

```sh
gcc -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix \
  src/BWS.c \
  src/arch/Posix/ThreadLib.c \
  src/arch/NET/generic/SoDisp.c \
  examples/HostInit/Main.c examples/HostInit/HostInit.c \
  app.c -o app -lpthread -lm -ldl
```

Add `src/DiskIo/posix/BaFile.c` and the appropriate macro/include settings when using `DiskIo`.

For Linux scale beyond `select`, inspect the `epoll` port and its required includes/macros. Do not merely add `-DUSE_EPOLL` while compiling the generic dispatcher.

Representative Windows project composition:

```text
src/BWS.c or src/BAS.c
src/arch/Windows/ThreadLib.c
src/arch/NET/generic/SoDisp.c
src/DiskIo/windows/BaFile.c             when DiskIo is used
examples/HostInit/Main.c
examples/HostInit/HostInit.c
application sources
ws2_32.lib
```

Use `inc/arch/Windows` and the target's NET include directory. Check `examples/C-RESTful-Service/RestService.vcxproj` and `examples/C-WebSockets/VcMake/example.vcxproj` as project-shape examples, then update toolset/platform settings for the installed Visual Studio.

Call the platform network initialization required by the selected `HostInit`. On Windows this includes Winsock initialization before socket use.

## RTOS and firmware

Do not copy HLOS `main`, filesystem paths, signal handling, process exit, or service logic into firmware.

For the selected target:

1. choose the target's `ThreadLib.c`;
2. choose the matching network stack/dispatcher `SoDisp.c`;
3. choose `BaFile.c` only when a usable filesystem exists;
4. initialize clock, entropy, network stack, storage, and TLS prerequisites;
5. create a dedicated task/thread that calls `barracuda()`;
6. size stack, heap, HTTP commands, connections, TLS buffers, and upload buffers from target measurements;
7. package web resources as embedded ZIP/CSP when disk is not guaranteed;
8. test on hardware, including reboot, network loss, resource exhaustion, and time validity.

Never call BAS/BWS from an ISR. Move driver/ISR events into a queue consumed by a normal task.

The public package includes porting directories for targets such as FreeRTOS, ThreadX, VxWorks, Nucleus, Zephyr, embOS, MQX, and others. A directory name does not prove compatibility with the user's SDK version; compile against the actual SDK and resolve target configuration explicitly.

## Feature and resource macros

Derive macros from the selected example, headers, and build rather than guessing. Common categories include:

- byte order and word size;
- OS/socket implementation;
- large-file support;
- debug and tracing;
- disk versus embedded/network resources;
- TLS feature configuration;
- BAS-only Lua/LSP modules.

Resource choices commonly appear as:

- `BAIO_DISK`: filesystem-backed resources and a compiled `BaFile.c`;
- `BAIO_EZIP`: embedded ZIP/CSP generated into C;
- `BAIO_ZIP`: external ZIP readers;
- `BAIO_NET`: network-backed development resources.

These are example build conventions, not universal runtime switches. Verify each macro in the current source before using it.

Do not enable a feature macro without providing every required symbol. Do not compile an external module merely because a macro name exists; first determine whether the selected amalgamation exports the module's entry point.

## Host-generated embedded web UI

For a browser UI that is developed with Node.js but deployed as an embedded ZIP, keep three build concerns independent:

| Workflow | Native server | Embedded UI source | Host web tools |
| --- | --- | --- | --- |
| server/development | builds without the embedded-UI feature macro | omitted or an empty guarded translation unit | no npm requirement; run the UI development server separately |
| release | defines the feature macro and compiles a committed generated C source | consumed as-is | no npm, ZIP, or generator requirement |
| regenerate | does not need to build the native server | rebuilds the committed C source through a temporary file | Node.js/npm, ZIP, and a host C compiler for `bin2c` or equivalent |

Use these rules:

- Commit the dependency lock file and generated C source. Ignore `node_modules`, the web bundler's output directory, native objects, ZIP intermediates, and generator binaries.
- Declare and check the frontend's supported Node.js engine before invoking the bundler. `npm ci` should consume the committed lock file; update the manifest and lock file together.
- Compile a shipped `bin2c.c` or similar generator with a host compiler before running it. A cross compiler for the firmware target cannot normally produce a utility that executes on the build host.
- Make ZIP input deterministic: stable file order, normalized timestamps, and metadata options that omit host-specific fields. Generate to a temporary file and replace the committed source only when content differs.
- Guard optional generated data and its mounting code with the same application feature macro. A Windows project may always list the committed generated C file if the disabled form is a valid empty translation unit.
- Do not make the release target silently invoke npm when the committed source is missing. Fail clearly; regeneration is an explicit maintainer action.
- Cleaning release intermediates must not delete the committed generated source.
- In WSL, prefer a matching Linux `node`/`npm` pair for Linux frontend dependencies. Avoid accidentally selecting a Windows npm shell launcher with no Linux `node`, and do not reuse platform-specific `node_modules` across Windows and Linux. If Windows npm is deliberately invoked from WSL, isolate that path and validate it separately.
- Verify the generated ZIP has the expected root layout (normally `index.html` and `assets/...`), validate its reader, compile/link the release, and request the mounted root URL from the running server.

Node.js/npm and ZIP tools are maintainer dependencies for regeneration, not firmware or release-build dependencies. Document that boundary prominently for users who only build the committed server source.

## C and C++ linking

Compile C sources as C. Compile C++ application files as C++, and link with the C++ linker or explicitly include the C++ runtime:

```sh
gcc -c ... src/BWS.c
g++ -c ... app.cpp
g++ -o app ... -lpthread -lm -ldl
```

Keep C-callable entry points in headers with guards:

```c
#ifdef __cplusplus
extern "C" {
#endif

void barracuda(void);
void installVirtualDir(HttpServer* server);

#ifdef __cplusplus
}
#endif
```

Do not throw exceptions through callbacks invoked by C. Decide whether exceptions and RTTI are available on the target before relying on them.

## Build review

Before accepting a build:

- list every compiled source and generated resource;
- prove exactly one amalgamation is present;
- inspect duplicate/missing symbol errors instead of adding random libraries;
- verify include paths match the chosen `ThreadLib` and `SoDisp`;
- verify debug/release runtime libraries and architecture agree;
- require a clean compile/link exit code;
- run the executable or firmware and exercise the real listener/protocol;
- use `nm`, `objdump`, `dumpbin`, map files, or linker diagnostics to verify optional features;
- report any platform not actually compiled, especially QNX/RTOS targets.
