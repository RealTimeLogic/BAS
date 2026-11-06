# Compiling Mako Server and Xedge

Build **Mako Server** to run Lua and web applications on a desktop, server, or embedded Linux system. Build **Xedge** to run them in device firmware, with a browser-based development environment.

Both use the Barracuda App Server (BAS) library in this repository. The build sections distinguish high-level operating systems (HLOS), such as Linux and Windows, from real-time operating systems (RTOS). Choose a build below, then refer to the shared source and porting details as needed.

## Choose Your Build Path

| Goal | Start here |
| --- | --- |
| Evaluate BAS or use a precompiled package | Select your platform on the [BAS download page](https://realtimelogic.com/downloads/bas/). |
| Build or customize Mako Server for an HLOS | See [Mako Server (HLOS)](#mako-server-hlos). |
| Integrate Xedge into an RTOS or firmware | See [Xedge (RTOS)](#xedge-rtos), [RTOS Build Examples](#rtos-build-examples), and [Porting Layers](#porting-layers). |

For native Barracuda Embedded Web Server (BWS), REST, WebSocket, and Modern C++ projects, use the example chooser in the repository [README](../README.md#examples). The README also explains the available licenses.

## Shared BAS source

Mako Server and Xedge both compile `src/BAS.c`. This is an amalgamation: the core library sources are combined into one C file.

- **Mako Server** adds command-line and service startup, disk resources, and optional native modules.
- **Xedge** adds the browser development environment, application resources, and hooks for device startup and hardware bindings.

BAS includes the BWS C APIs and adds Lua, [Lua Server Pages (LSP)](https://realtimelogic.com/products/lua-server-pages/), and higher-level web and IoT APIs. Application logic can be written in Lua while performance-critical or hardware-specific modules remain in C or C++.

> See [Embedded Web Server vs. Embedded Application Server](https://realtimelogic.com/products/web-server-vs-application-server/) if you are new to application server technology.

If you already use a compatible Lua 5.5 build, you can exclude the bundled Lua source by compiling with `-DBA_ENABLE_EMBEDDED_LUA=0`. Link BAS with that Lua build and verify that its configuration and application binary interface (ABI) match the BAS headers.

## C Code Components

Both hosts start with the following files:

- `src/BAS.c`: amalgamated Barracuda App Server.
- `src/arch/XXX/ThreadLib.c`: kernel porting layer.
- `src/arch/NET/XXX/SoDisp.c`: TCP/IP porting layer, also called the socket dispatcher.
- `src/DiskIo/XXX/BaFile.c`: optional file system porting layer.

Add the Mako Server or Xedge host sources and generated resources described in their sections below. See the memory section in [Porting Barracuda to an Embedded System](https://realtimelogic.com/ba/doc/en/introduction.html#porting) for target-sizing guidance.

## Basic Compile Model

Compile `src/BAS.c`, the host sources, and one thread and socket port from
the list above. Add a filesystem port if the application uses one.

Put `inc` and the selected OS and network header directories on the compiler's
include path. For example, a generic POSIX build uses `inc/arch/Posix` and
`inc/arch/NET/Posix`. Some ports combine both sets of headers in one directory;
use the [Porting Layers](#porting-layers) table to choose the right combination.

Add these files to your IDE or build system, or use the commands below.

### POSIX file offsets

On POSIX, `BA_FILE_OFFSET_BITS` is an integer bit width, either `32` (the
shared default) or `64`. Use the same value throughout the library and host.
For `64`, the early `BaConfigPosix.h` helper applies the native large-file
settings before system headers, and DiskIo uses `fseeko()` for 64-bit seeking.
Include `TargConfig.h` before system headers in application code. If a forced
include or third-party header must come first, supply `_FILE_OFFSET_BITS=64`
on the compiler command line for the whole build as well. Do not put system
header includes in `BaConfigOpts.h`; a 32-bit build with late file-offset
settings is rejected by the POSIX DiskIo size check.

## Mako Server (HLOS)

The [Mako Server example](https://realtimelogic.com/ba/doc/en/Mako.html) is the reference host for high-level operating systems such as Windows, embedded Linux, and QNX. Mako Server can also be adapted to environments such as VxWorks 7, but Xedge is the normal starting point for RTOS and firmware integration.

For a standard build, go to [Windows](#windows) or [Linux](#linux). The following dispatcher and assembly sections explain options used by the supplied build scripts.

![Mako Server Build Process](https://realtimelogic.com/ba/doc/en/examples/MakoServer/Build-Mako-Diagram.svg)

### Choose a socket dispatcher

Use **epoll on Linux** and **poll on QNX** for applications with many
simultaneous connections. `LinuxBuild.sh` and `RaspberryPiBuild.sh` select
epoll; `QNXBuild.sh` selects poll. Poll is also available on other compatible
POSIX targets, such as macOS.

For a QNX build using the generic select dispatcher, set `BA_USE_POLL=0`
when running `QNXBuild.sh`.

When invoking `mako.mk` directly, set the Boolean make option
`BA_USE_EPOLL=1` or `BA_USE_POLL=1`. These options accept `0` or `1` and
default to `0`, selecting generic select; the older `EPOLL=true` option also
selects epoll. Do not enable both dispatchers. Clean and rebuild all objects
when switching. The makefile selects the matching source and header for you.

Poll avoids the generic select dispatcher's descriptor limit. Allow enough
operating-system sockets, file descriptors, and memory for the desired
connections, and configure Mako's numeric `conncnt` setting accordingly.

### Automatic SharkSSL assembly with mako.mk

`mako.mk` automatically includes the packaged GCC/Clang assembly for supported
x86-64 and ARM64 compiler targets. It preprocesses `SharkSslTarget.c` and
compile-checks the backend sources with your selected compiler and flags.
These checks never execute target code, so they also work when cross-compiling.
Unsupported targets or failed automatic checks retain the C implementation.
At runtime, feature checks select the cipher paths supported by the CPU and OS.

Automatic selection enables AES-GCM/GHASH, ChaCha20, Poly1305 and the dedicated
X25519 hook. x64 also enables VAES when the assembler supports it. ARM64 enables
the SHA-256, SHA-384/SHA-512, BigInt, P-256 and P-384 hooks for configured algorithms.
Common C keeps the compiler's baseline CPU target.

Native AES assembly requires the integer options `SHARKSSL_AES_SOFTWARE_CT=0`
and `SHARKSSL_AES_GCM_SOFTWARE_CT=0`. The selector supplies these values only
when enabling AES assembly and only if you have not defined them. An explicit
value of `1` for either option retains constant-time (CT) software AES and disables automatic
AES assembly. Explicit per-algorithm settings are preserved. PSA consumers
leave acceleration to their separate provider.

On x86/x64,
CPUs without AES-NI use table-based C AES/GHASH; software AES outside GCM also
uses the non-CT implementation in this profile. To retain the software CT
defaults, use `SHARKSSL_ASM=off` and remove manual assembly and CT overrides.

From the repository root, inspect the selection before building:

```sh
# Display the selected backend, enabled hooks, and any failed compile checks.
make -f mako.mk sharkssl-info
make -f mako.mk
```

`SHARKSSL_ASM` is an optional string setting: `auto` (default), `off`,
`X64_GCC`, or `ARM64_GCC`. Explicit backend requests fail if the compiler target
does not match or required backend sources cannot compile. Automatic x64
selection omits VAES if the assembler cannot assemble that optional kernel.
Manually enabled assembly hooks must also be supported; failures are reported.

```sh
# Build entirely with C. Remove any manually enabled assembly macros as well.
make -f mako.mk SHARKSSL_ASM=off clean
make -f mako.mk SHARKSSL_ASM=off

# Cross-compile using the compiler target to select ARM64 assembly.
make -f mako.mk CC=aarch64-linux-gnu-gcc sharkssl-info
make -f mako.mk CC=aarch64-linux-gnu-gcc
```

Use a clean build after changing compiler or feature flags, or set the optional
directory path `ODIR` to a separate object directory for each configuration
(default: the repository root). The optional path `BASRESDIR` selects the
BAS-Resources checkout (default: `../BAS-Resources`). If you set `TARGET` to an
executable path outside the repository root, copy `mako.zip` beside it before
running the server.

The shared build rules are in [SharkSslAsm.mk](../SharkSslAsm.mk) and
[SharkSslBackends.mk](../SharkSslBackends.mk). See the
[x64](../src/crypto/X64_GCC/README.md) and
[ARM64](../src/crypto/ARM64_GCC/README.md) port guides for individual hooks and
CPU requirements. In an amalgamated build, retain `src/BAS.c` (or `src/BWS.c`
for BWS) where those guides refer to the SDK's `SharkSslCrypto.c`.
This automatic selection applies to `mako.mk`; IDE projects and other build
systems must add their backend files and definitions separately.

### Windows

Open [mako.sln](../examples/MakoServer/VcMake/mako.sln) and select Debug or Release,
with Win32 or x64. The project uses the installed Visual Studio C++ toolset and
Windows 10/11 SDK. All four configurations enable `BA_ENABLE_MDNS=1` and `BA_ENABLE_MDNS_LUA=1` and build SharkSSL
with C implementations only; no assembler setup is needed.

Executables are written to
`examples/MakoServer/obj/VcMake/<platform>/<configuration>/`, with separate
intermediate files for each configuration. Debug uses the debug C runtime and
retains assertions; Release enables optimization.

**SQLite is optional.** When `src/sqlite3.c` and `src/sqlite3.h` are absent,
all four configurations build without SQLite (`MAKO_SQL_BACKEND=MAKO_SQL_NONE`). Mako can run in
this configuration, but applications that require its SQLite binding cannot
use that binding.

To enable SQLite, copy the SQLite amalgamation files `sqlite3.c` and `sqlite3.h`
into this repository's `src` directory, reload the solution, and rebuild. The
project detects the files and includes SQLite and its Lua binding automatically
(`MAKO_SQL_BACKEND=MAKO_SQL_SQLITE`). No project-file edits are required.

The optional MSBuild boolean property `UseSqlite` overrides detection:

```bat
rem Run from the repository root in a Visual Studio Developer Command Prompt.
rem Build without SQLite even when its source files are present.
msbuild examples\MakoServer\VcMake\mako.sln /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:UseSqlite=false

rem Require SQLite; the build reports an error if either source file is missing.
msbuild examples\MakoServer\VcMake\mako.sln /t:Rebuild /p:Configuration=Release /p:Platform=x64 /p:UseSqlite=true
```

Place `mako.zip` beside the executable before running it. The project copies
the repository-root `mako.zip` if present. The optional MSBuild path property
`MakoZip` selects a resource archive elsewhere:

```bat
rem Run from the repository root in a Visual Studio Developer Command Prompt.
msbuild examples\MakoServer\VcMake\mako.sln /p:Configuration=Release /p:Platform=x64 /p:MakoZip=C:\resources\mako.zip
```

### Linux

For Linux and embedded Linux, see [Embedded Linux Web Interface Design](https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design). You can download and compile the server as follows:

```sh
# Run in Bash to download and execute the Linux build script.
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```

To manage source downloads, compilation, and installation together, use the [Ansible playbook](../Ansible.md). You can also [install the server as a systemd service](https://github.com/RealTimeLogic/MakoInstaller) using an Ansible script.

To cross-compile for embedded Linux, set `CC` to your cross-compiler:

```sh
# Replace this path with your installed cross-compiler.
export CC=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```

To cross-compile with [Lua GPIO bindings](https://github.com/vsergeev/lua-periphery), set `CROSS_COMPILE` to your toolchain prefix:

```sh
# Use the toolchain prefix, without the gcc suffix.
export CROSS_COMPILE=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/RaspberryPiBuild.sh)
```

### Generic HLOS Build

POSIX command-line builds compile `examples/MakoServer/src/MakoPosixMain.c` alongside `MakoMain.c`. The former supplies `main()` and calls `runMako()`. If your application supplies its own entry point, omit `MakoPosixMain.c` and call `runMako()` from your startup code. Windows uses the sources in `examples/MakoServer/src/Windows`.

The following command builds Mako Server without SQLite on Linux. For another POSIX target, select its compiler, Lua platform settings, and system libraries. See [VxWorks](#vxworks) for that target's example. Run from the repository root.

```sh
# Run from the repository root with the required port sources.
gcc -o examples/MakoServer/mako -fmerge-all-constants -Os\
    -DMAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0 -DBA_FILE_OFFSET_BITS=64 -DLUA_USE_LINUX -DMAKO -DMAKO_SQL_BACKEND=MAKO_SQL_NONE\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c examples/MakoServer/src/MakoPosixMain.c\
    -lpthread -lm -ldl
```

See the [Mako Server build documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html) for details on macros and other build options. The amalgamated version in this repository differs from the standard SDK layout.

After running the compilation command, [build `mako.zip`](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build) and place it beside the executable in `examples/MakoServer`. Start the server from that directory. A successful startup reports that `mako.zip` was mounted and that the configured HTTP and HTTPS listeners are active. Version numbers, addresses, and ports depend on the build and configuration.

### Windows Command-Line Build

Run the following command from the repository root in a [64-bit Visual C++ tools prompt](https://learn.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170). It builds Mako Server with Windows service support.

See the [current Mako Server for Windows page](https://makoserver.net/download/windows/) for Microsoft Store, ZIP, and source-build alternatives.

```bat
rem Run from the repository root in the x64 compiler prompt.
cl /O2^
 /DMAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0 /DBA_FILE_OFFSET_BITS=64 /DMAKO /DMAKO_SQL_BACKEND=MAKO_SQL_NONE^
 /Iinc /Iinc/arch/Windows /Iinc/arch/NET/Windows^
 src/BAS.c^
 src/arch/Windows/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/windows/BaFile.c^
 examples/MakoServer/src/MakoMain.c^
 examples/MakoServer/src/Windows/MakoWinMain.c examples/MakoServer/src/Windows/servutil.c^
 ws2_32.lib kernel32.lib User32.lib Gdi32.lib advapi32.lib ole32.lib oleaut32.lib shell32.lib^
 /link /machine:X64 /OUT:examples/MakoServer/mako.exe
```


### Including SQLite

Mako Server can optionally be linked with SQLite and the [Lua SQLite bindings](https://realtimelogic.com/ba/doc/en/lua/luasql.html). Before running the command below, [download SQLite](https://www.sqlite.org/download.html) and copy `sqlite3.c` and `sqlite3.h` to `src`.

```sh
# Run from the repository root with the required port sources.
gcc -o examples/MakoServer/mako -fmerge-all-constants -Os\
    -DMAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0 -DBA_FILE_OFFSET_BITS=64 -DLUA_USE_LINUX -DMAKO\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c examples/MakoServer/src/MakoPosixMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm -ldl
```

## Xedge (RTOS)

Xedge runs Lua and LSP applications on a device. Its browser-based development environment lets you edit and test applications, then package them for deployment. See the [online Xedge documentation](https://realtimelogic.com/ba/doc/en/Xedge.html) for application development and release options.

[![Xedge in Developer Mode](https://realtimelogic.com/images/xedge/v1/Xedge.png)](https://simplemq.com/videos/Xedge-IDE-intro.mp4)

See the [Xedge product page](https://realtimelogic.com/products/xedge/) for details.


### Compiling and Integrating Xedge

The Xedge development environment includes browser files and server-side code. Build these resources into a ZIP, then convert the ZIP to a C array to include it in the executable or firmware.

![Xedge Build Process](https://realtimelogic.com/ba/doc/en/examples/xedge/Build-Xedge-Diagram.svg)

The compilation process below shows the general integration pattern. For an ESP32-based walkthrough, see [Rapid Firmware Development with the Barracuda App Server](https://realtimelogic.com/articles/Rapid-Firmware-Development-with-the-Barracuda-App-Server). Other targets require their own startup code, porting layers, resource configuration, and build settings.

Fetch the repositories and build the resource ZIP file:

```sh
# Fetch the Barracuda App Server
git clone https://github.com/RealTimeLogic/BAS.git

# Fetch the Barracuda App Server resources
git clone https://github.com/RealTimeLogic/BAS-Resources.git

# Go to the build directory
cd BAS-Resources/build/

# Run the build script. If on Windows, use Xedge.cmd.
# When prompted, initially select n for OPC UA and n for minification.
# The native build below supplies the required embedded CA store.
. Xedge.sh

# The ZIP file is converted to a C array by the build script.
# Copy this file to the Xedge directory.
cp XedgeZip.c ../../BAS/examples/xedge/
cd ../../BAS
```

Standalone Xedge requires `BA_ENABLE_EMBEDDED_CA=1` for both `src/BAS.c` and the Xedge host. This enables the embedded certificate-authority (CA) trust store. Keep `inc/cacert.h` on the include path. To select a smaller or private trust set, use the SharkSSL SDK tool `SharkSSLParseCAList your-roots.pem > cacert.h`, replace the header, and rebuild. Updating the compiled roots requires a new native build. Explicit Lua certificate stores remain supported. Ship the executable or firmware and its matching resources together.

`XedgeZip.c` contains the Xedge resources converted to a C file. See the [BAS-Resources repository](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build) for details.

To compile Xedge, include:

- **Required for every target:** `src/BAS.c`, `examples/xedge/src/xedge.c`, `examples/xedge/XedgeZip.c`, the target's `ThreadLib.c`, and the matching `SoDisp.c`.
- **Host test builds:** `examples/HostInit/Main.c` and `examples/HostInit/HostInit.c` provide HLOS startup scaffolding. Replace them with target-specific startup code for RTOS and firmware builds.
- **Target-dependent:** Add an allocator such as `src/dlmalloc.c` if the target does not provide a suitable allocator. Add `BaFile.c` for supported file systems and `led.c` for the example Lua bindings.

The following example uses Linux to compile Xedge as a standalone application. For embedded devices, include the relevant source files in your build system instead.

```sh
# Build the host test after generating examples/xedge/XedgeZip.c.
gcc -o xedge -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    -DLUA_USE_LINUX\
    -DBA_ENABLE_EMBEDDED_CA=1\
    src/BAS.c\
    examples/HostInit/Main.c examples/HostInit/HostInit.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/xedge/src/xedge.c examples/xedge/src/led.c\
    examples/xedge/XedgeZip.c -lpthread -lm -ldl
```

Integration notes:

- `Main.c` and `HostInit.c` are designed for a non-embedded host build. For an RTOS build, study these files and set up similar startup code for your environment.
- At minimum, your RTOS build needs a dedicated thread that runs `barracuda(void)`, which does not return. Use stack size `BA_THREAD_STACK_SIZE` bytes.
- `xedge.c` is the Xedge C startup code.
- `led.c` includes example [Lua bindings](https://realtimelogic.com/ba/doc/en/GettingStarted.html#UsingLSP) for device control. If you do not need these examples, define `-DXEDGE_ENABLE_AUX=0` to omit the `xedgeOpenAUX()` call.
- For embedded builds, consider testing without file system support first. Remove `BaFile.c` from your build and compile with `-DXEDGE_ENABLE_DISK_IO=0`, which is used by `xedge.c`.
- If the target does not provide a suitable allocator, include `src/dlmalloc.c` and initialize it using the pattern in `examples/HostInit/Main.c`. Use the target's allocator instead when its behavior and integration requirements are known.

These Xedge instructions are excerpted from the [Xedge full SDK build guide](https://realtimelogic.com/ba/examples/xedge/readme.html), which includes additional release-build details. File locations differ slightly in this repository.

### Cross-Compiling Xedge

Include the files listed above in your IDE or Makefile. If the target does not provide a suitable allocator, BAS includes `src/dlmalloc.c` as one option. See the [FreeRTOS readme](../src/arch/FreeRTOS/README.txt) for an example of how to assemble the required components. Adapt the startup, allocator, and porting layers to the selected RTOS and network stack.

If you have an ESP32 board, the [precompiled Xedge binaries](https://realtimelogic.com/downloads/bas/ESP32/) let you try Xedge on FreeRTOS and lwIP before setting up a firmware build.

## Enabling and Disabling Features

Use `inc/BaConfigOpts.h` for application overrides; it ships with comments only. Shared defaults and validation are in `inc/BaConfig.h`. The selected `TargConfig.h` and `BaSocketConfig.h` supply port capabilities. Mako and Xedge host options are in `MakoConfig.h` and `XedgeConfig.h`.

Boolean switches accept numeric `0` or `1`. Options with several choices use the named constants in their headers. Apply shared definitions to every source file in the build.

- `BA_ENABLE_LUA_DEBUGGER=1`: Include [Lua debugger support](https://makoserver.net/articles/Lua-and-LSP-Debugging).
- `BA_ENABLE_REVERSE_CONNECTION=1`: Enable reverse connections for the connection bridge feature in [SharkTrustX](https://realtimelogic.com/products/SharkTrustX/).
- `BA_ENABLE_LUA_OPCUA=1`: Enable OPC UA support when the build also includes the required OPC UA Lua resources and C module.
- `BA_ENABLE_EMBEDDED_CA=1`: Required for a standalone Xedge build, in both BAS and the host sources. Uses `inc/cacert.h` without retaining a heap copy; the Xedge resource ZIP contains no `.certificate/cacert.shark`. Other BAS hosts select their own CA mode.
- `BA_ENABLE_LUA_FORKPTY=1`: Enable the [advanced process management API](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#forkptylib), available for Linux and QNX. This API is required for the [CGI plugin](https://github.com/RealTimeLogic/LSP-Examples/tree/master/CGI) and the [web shell](https://makoserver.net/articles/Linux-Web-Shell).
- `BA_ENABLE_REVERSE_PROXY=1`: Enable the [reverse proxy](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#reverseproxy).
- `BA_ENABLE_UBJSON=1`: Enable [Universal Binary JSON](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#ubjson).
- `BA_ENABLE_LUA_DEBUG_LIBRARY=0`: Exclude the Lua `debug` module.

### Mako Server Macros

- `BA_ENABLE_SHARKTRUST=0`: Do not include `tokengen.c`; disables the built-in SharkTrustX key.
- `MAKO_ENABLE_LUA_MODULE_INTERFACE=1`: Enable loading [external Lua modules](https://makoserver.net/documentation/c-modules/). When using source builds, you can alternatively integrate additional [Lua bindings](https://realtimelogic.info/swig/) directly into your build.

### Xedge Macros

- `BA_ENABLE_SHARKTRUST=0`: Disable SharkTrustX integration.
- `BA_ENABLE_SOFTTPM_KEY_MATERIAL=0`: Exclude native softTPM key material (the default for a PSA consumer). Native builds default to 1 and include `EncryptionKey.h`; set the string macro `XEDGE_SOFTTPM_KEY_HEADER` to a generated header name to replace it.
- `XEDGE_ENABLE_DISK_IO=0`: Build without local file-system integration (default: 1). When disk I/O is enabled, `XEDGE_USE_DEFAULT_DISK_IO_INIT` selects the supplied initializer; it defaults to 1 on Windows/POSIX and 0 on embedded targets, where the application supplies `xedgeInitDiskIo()`.
- `XEDGE_ENABLE_AUX=0`: Do not call `xedgeOpenAUX()`, which is where you typically register your own Lua bindings.

## Porting Layers

[Contact Real Time Logic](https://realtimelogic.com/contactus/) if an existing porting layer does not match your target operating system, network stack, or SDK.

| OS + TCP | Include directories | Source files |
| --- | --- | --- |
| Digi | `inc/arch/Digi` | `src/arch/Digi/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| embOS | `inc/arch/NET/embOSIP` `inc/arch/embOS` | `src/arch/embOS/ThreadLib.c` `src/arch/NET/embOSIP/SoDisp.c` |
| FreeRTOS + lwIP | `inc/arch/NET/LwIP` `inc/arch/FreeRTOS` | `src/arch/FreeRTOS/ThreadLib.c` `src/arch/NET/LwIP/SoDisp.c` |
| FreeRTOS + TCP | `inc/arch/FreeRTOS` | `src/arch/FreeRTOS/ThreadLib.c` `src/arch/FreeRTOS/SoDisp.c` |
| INTEGRITY | `inc/arch/NET/Posix` `inc/arch/INTEGRITY` | `src/arch/INTEGRITY/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| INtime | `inc/arch/NET/INtime` `inc/arch/INtime` | `src/arch/INtime/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Linux + epoll | `inc/arch/NET/epoll` `inc/arch/Posix` | `src/arch/Posix/ThreadLib.c` `src/arch/NET/epoll/SoDisp.c` |
| MQX | `inc/arch/NET/MQX` `inc/arch/MQX` | `src/arch/MQX/ThreadLib.c` `src/arch/NET/MQX/SoDisp.c` |
| NuttX | `inc/arch/NET/Posix` `inc/arch/Posix` | `src/arch/Posix/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Nucleus | `inc/arch/NET/Nucleus` `inc/arch/Nucleus` | `src/arch/Nucleus/ThreadLib.c` `src/arch/NET/Nucleus/SoDisp.c` |
| POSIX (Linux, Mac, QNX) | `inc/arch/NET/Posix` `inc/arch/Posix` | `src/arch/Posix/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| QNX and compatible POSIX targets + poll | `inc/arch/NET/poll` `inc/arch/Posix` | `src/arch/Posix/ThreadLib.c` `src/arch/NET/poll/SoDisp.c` |
| Quadros | `inc/arch/Quadros` | `src/arch/Quadros/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| SMX | `inc/arch/NET/SMX-NET` `inc/arch/SMX` | `src/arch/SMX/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| ThreadX + NetX Duo | `inc/arch/ThreadX` | `src/arch/ThreadX/ThreadLib.c` `src/arch/ThreadX/SoDisp.c` |
| VxWorks | `inc/arch/VxWorks` | `src/arch/VxWorks/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Windows | `inc/arch/NET/Windows` `inc/arch/Windows` | `src/arch/Windows/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Zephyr | `inc/arch/Zephyr` | `src/arch/Zephyr/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |

Add `inc` to every build's include path. Check the selected port against your SDK version, especially its network settings and APIs.

## HLOS Build Examples

See the [Mako Server download page](https://makoserver.net/download/overview/) for additional platform and compile examples.

## RTOS Build Examples

### Zephyr

For Xedge on Zephyr, use the [Xedge4Zephyr project](https://github.com/RealTimeLogic/Xedge4Zephyr), which provides the Zephyr-specific build and integration files.

[![Zephyr](https://upload.wikimedia.org/wikipedia/commons/thumb/6/64/Zephyr_RTOS_logo_2015.svg/500px-Zephyr_RTOS_logo_2015.svg.png)](https://github.com/RealTimeLogic/Xedge4Zephyr)

### NuttX

NuttX includes an Xedge demonstration in its application examples. See the [Xedge NuttX documentation](https://nuttx.apache.org/docs/latest/applications/examples/xedge_demo/index.html) for the current build and configuration steps.

[![NuttX](https://nuttx.apache.org/assets/themes/apache/img/logo.png)](https://nuttx.apache.org/docs/latest/applications/examples/xedge_demo/index.html)

### INTEGRITY

The following example compiles Xedge for Green Hills INTEGRITY. You can also [download a ready-to-run Xedge GHS Multi IDE project](https://realtimelogic.com/downloads/bas/xedge-ghs-build.zip).

```sh
# Adjust the SDK path and BSP for your INTEGRITY installation.
ccintarm -G -c99 -os_dir C:/ghs/int1144 -bsp simarm -o xedge\
    -non_shared -DXEDGE_THREAD_POOL_MODE=XEDGE_THREAD_POOL_SHARED_FIXED -DXEDGE_THREAD_COUNT=3 -DBA_ENABLE_EMBEDDED_CA=1\
    -Iinc -Iinc/arch/INTEGRITY -Iinc/arch/NET/Posix\
    --diag_suppress=111,188,550,549,546,223\
    src/BAS.c\
    src/arch/NET/generic/SoDisp.c\
    src/arch/INTEGRITY/ThreadLib.c\
    src/DiskIo/posix/BaFile.c\
    examples/HostInit/Main.c examples/HostInit/HostInit.c\
    examples/xedge/src/xedge.c\
    examples/xedge/XedgeZip.c\
    examples/xedge/src/XedgeInitDiskIo.c\
    examples/xedge/src/led.c\
    -lnet -livfs -lsocket\
```

### VxWorks

Xedge is the recommended starting point for new VxWorks integrations. The following example builds Mako Server for projects that require its standalone process-style host. See the [Barracuda App Server VxWorks build page](https://realtimelogic.com/downloads/bas/VxWorks/) for details.

```sh
# Run from the repository root with the VxWorks compiler on PATH.
wr-cc -o examples/MakoServer/mako -static -fmerge-all-constants -Os\
    -DMAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0 -DBA_FILE_OFFSET_BITS=64 -DBA_HAVE_STDIO=1 -DMAKO -DMAKO_SQL_BACKEND=MAKO_SQL_NONE\
    -Iinc -Iinc/arch/VxWorks -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/VxWorks/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c examples/MakoServer/src/MakoPosixMain.c\
    -lnet
```

### FreeRTOS

#### i.MX RT1020

Download a ready-to-compile and ready-to-run project for [i.MX RT1020](https://realtimelogic.com/downloads/bas/rt1020/).

#### ESP32

![Xedge32](https://realtimelogic.com/images/xedge/finger-icon.png)

[Xedge32](https://github.com/RealTimeLogic/Xedge32) provides Xedge firmware and peripheral bindings for ESP32 devices. Its README lists supported boards and build instructions.

## Tutorials

- [What is an Embedded Application Server?](https://realtimelogic.com/articles/What-is-an-Embedded-Application-Server)
- [Using Lua for IoT and Web Application Development](https://realtimelogic.com/articles/Lua-FastTracks-Embedded-Web-Application-Development)
- [Online Interactive Lua and LSP Tutorials](https://tutorial.realtimelogic.com/)
- [Modern Approach to Embedding a Web Server in a Device](https://realtimelogic.com/articles/Modern-Approach-to-Embedding-a-Web-Server-in-a-Device)
- [The Ultimate Guide to Embedded Web Server Technology](https://realtimelogic.com/articles/Embedded-Web-Server-Tutorials)
- [Mako/BAS Tutorials](https://makoserver.net/tutorials/)
- [Ready-to-Run Examples](https://github.com/RealTimeLogic/LSP-Examples)

## Online Documentation

- [Introduction](https://realtimelogic.com/ba/doc/)
- [Lua APIs](https://realtimelogic.com/ba/doc/en/lua/lua.html)
- [BAS/BWS C/C++ API](https://realtimelogic.com/ba/doc/en/C/reference/html/index.html)
- [SharkSSL TLS Stack C/C++ API](https://realtimelogic.com/ba/doc/en/C/shark/index.html)
