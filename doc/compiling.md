# Compiling Mako Server and Xedge

This guide explains how to build the two Barracuda App Server (BAS) reference hosts included in this repository: Mako Server for high-level operating systems (HLOS) and Xedge for real-time operating systems (RTOS) and embedded firmware. It covers the shared BAS source, porting layers, feature macros, generated resources, and cross-compilation details required by those hosts.

## Choose Your Build Path

| Goal | Start here |
| --- | --- |
| Evaluate BAS or use a precompiled package | Select your platform on the [BAS download page](https://realtimelogic.com/downloads/bas/). |
| Build or customize Mako Server for an HLOS | See [Mako Server (HLOS)](#mako-server-hlos). |
| Integrate Xedge into an RTOS or firmware | See [Xedge (RTOS)](#xedge-rtos), [RTOS Build Examples](#rtos-build-examples), and [Porting Layers](#porting-layers). |

For native Barracuda Embedded Web Server (BWS), REST, WebSocket, and Modern C++ projects, use the example chooser in the repository [README](../README.md#examples). The README also explains the available licenses.

## Shared BAS Foundation

Mako Server and Xedge both compile `src/BAS.c`, the amalgamated Barracuda App Server library. "Amalgamated" means that many separate C files have been combined into one primary source file, making compilation and integration easier.

- **Mako Server** adds HLOS startup, command-line/service behavior, disk resources, and optional native modules around BAS.
- **Xedge** adds the embedded application environment, resource package, target startup integration, and optional hardware/native bindings around BAS.

BAS includes the BWS C APIs and adds Lua, [Lua Server Pages (LSP)](https://realtimelogic.com/products/lua-server-pages/), and higher-level web and IoT APIs. Application logic can be written in Lua while performance-critical or hardware-specific modules remain in C or C++.

> See [Embedded Web Server vs. Embedded Application Server](https://realtimelogic.com/products/web-server-vs-application-server/) if you are new to application server technology.

If you already use a compatible Lua 5.5 build, you can exclude the bundled Lua source by compiling with `-DUSE_BA_LUA=0`. Link BAS with that Lua build and verify that its configuration and application binary interface (ABI) match the BAS headers.

## C Code Components

Both hosts start with the following files:

- `src/BAS.c`: amalgamated Barracuda App Server.
- `src/arch/XXX/ThreadLib.c`: kernel porting layer.
- `src/arch/NET/XXX/SoDisp.c`: TCP/IP porting layer, also called the socket dispatcher.
- `src/DiskIo/XXX/BaFile.c`: optional file system porting layer.

Add the Mako Server or Xedge host sources and generated resources described in their sections below. See the memory section in [Porting Barracuda to an Embedded System](https://realtimelogic.com/ba/doc/en/introduction.html#porting) for target-sizing guidance.

## Basic Compile Model

To compile either host, add the required porting layers to `src/BAS.c`:

- Add the required `ThreadLib.c`.
- Add the required `SoDisp.c`.
- Optionally add `BaFile.c` when using file system support.

Replace `XXX` with the required porting layer.

The compiler include path must include:

- `inc`
- `inc/arch/XXX`
- `inc/arch/NET/XXX`
- `inc/DiskIo/XXX`, when using a file system porting layer

BAS can be used in several deployment models. This guide focuses on:

- Mako Server as a process on desktop/server or embedded HLOS targets.
- Xedge integrated into RTOS-powered or other embedded firmware.

The amalgamated source can be compiled from the command line or added to an IDE, Makefile, CMake project, or vendor build system.

## Mako Server (HLOS)

The [Mako Server example](https://realtimelogic.com/ba/doc/en/Mako.html) is the reference host for high-level operating systems such as Windows, embedded Linux, and QNX. Mako Server can also be adapted to environments such as VxWorks 7, but Xedge is the normal starting point for RTOS and firmware integration.

![Mako Server Build Process](https://realtimelogic.com/ba/doc/en/examples/MakoServer/Build-Mako-Diagram.svg)

### Windows

Use the Visual Studio project file in `examples/MakoServer/VcMake`. Download SQLite before building, or disable the SQLite build in the project file.

### Linux

For Linux and embedded Linux, see [Embedded Linux Web Interface Design](https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design). You can download and compile the server as follows:

```sh
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```

For a more robust compilation option, use the [included Ansible script](../Ansible.md). You can also [install the server as a systemd service](https://github.com/RealTimeLogic/MakoInstaller) using an Ansible script.

To cross-compile for embedded Linux, set `CC` to your cross-compiler:

```sh
export CC=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```

To cross-compile with [Lua GPIO bindings](https://github.com/vsergeev/lua-periphery), set `CROSS_COMPILE` to your toolchain prefix:

```sh
export CROSS_COMPILE=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/RaspberryPiBuild.sh)
```

### Generic HLOS Build

The following example builds Mako Server without SQLite for a high-level operating system. It works with GCC and compatible compilers, including cross-compilers for embedded Linux and QNX. Replace `gcc` with the compiler for your target. See [VxWorks](#vxworks) for the RTOS-specific example.

```sh
gcc -o examples/MakoServer/mako -fmerge-all-constants -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO -DUSE_SQL=0\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lpthread -lm -ldl
```

See the [Mako Server build documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html) for details on macros and other build options. The amalgamated version in this repository differs from the standard SDK layout.

After running the compilation command, [build `mako.zip`](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build) and place it beside the executable in `examples/MakoServer`. Start the server from that directory. A successful startup reports that `mako.zip` was mounted and that the configured HTTP and HTTPS listeners are active. Version numbers, addresses, and ports depend on the build and configuration.

### Windows Command-Line Build

The following example compiles and links Mako Server as a 64-bit Windows application using the Visual Studio command-line compiler. It includes two additional files that make it possible to run the server as a Windows service.

See the [current Mako Server for Windows page](https://makoserver.net/download/windows/) for Microsoft Store, ZIP, and source-build alternatives.

```bat
cl /O2^
 /DUSE_EMBEDDED_ZIP=0 /DBA_FILESIZE64 /DMAKO /DUSE_SQL=0^
 /Iinc /Iinc/arch/Windows /Iinc/arch/NET/Windows^
 src/BAS.c^
 src/arch/Windows/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/windows/BaFile.c^
 examples/MakoServer/src/MakoMain.c^
 examples/MakoServer/src/Windows/MakoWinMain.c examples/MakoServer/src/Windows/servutil.c^
 ws2_32.lib kernel32.lib User32.lib Gdi32.lib advapi32.lib ole32.lib oleaut32.lib shell32.lib^
 /link /machine:X64 /OUT:examples/MakoServer/mako.exe
```

This command requires the [64-bit Visual C++ toolset command line](https://learn.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170).

### Including SQLite

Mako Server can optionally be linked with SQLite and the [Lua SQLite bindings](https://realtimelogic.com/ba/doc/en/lua/luasql.html). Before running the command below, [download SQLite](https://www.sqlite.org/download.html) and copy `sqlite3.c` and `sqlite3.h` to `src`.

```sh
gcc -o examples/MakoServer/mako -fmerge-all-constants -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm -ldl
```

## Xedge (RTOS)

Xedge is the BAS-based host for developing and running Lua and LSP applications on RTOS and firmware targets. It includes a browser-based development environment and supports packaging applications for deployment. See the [online Xedge documentation](https://realtimelogic.com/ba/doc/en/Xedge.html) for application development and release options.

[![Xedge in Developer Mode](https://realtimelogic.com/images/xedge/v1/Xedge.png)](https://simplemq.com/videos/Xedge-IDE-intro.mp4)

See the [Xedge product page](https://realtimelogic.com/products/xedge/) for details.


### Compiling and Integrating Xedge

The Xedge IDE includes a single-page web application and supporting server-side code. These resources must be included in the build.

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
# When prompted, initially select n for OPC UA, s for small cacert.shark,
# and n for compressing the files.
. Xedge.sh

# The ZIP file is converted to a C array by the build script.
# Copy this file to the Xedge directory.
cp XedgeZip.c ../../BAS/examples/xedge/
cd ../../BAS
```

`XedgeZip.c` contains the Xedge resources converted to a C file. See the [BAS-Resources repository](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build) for details.

To compile Xedge, include:

- **Required for every target:** `src/BAS.c`, `examples/xedge/src/xedge.c`, `examples/xedge/XedgeZip.c`, the target's `ThreadLib.c`, and the matching `SoDisp.c`.
- **Host test builds:** `examples/HostInit/Main.c` and `examples/HostInit/HostInit.c` provide HLOS startup scaffolding. Replace them with target-specific startup code for RTOS and firmware builds.
- **Target-dependent:** Add an allocator such as `src/dlmalloc.c` if the target does not provide a suitable allocator. Add `BaFile.c` for supported file systems and `led.c` for the example Lua bindings.

The following example uses Linux to compile Xedge as a standalone application. For embedded devices, include the relevant source files in your build system instead.

```sh
gcc -o xedge -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    -DLUA_USE_LINUX\
    src/BAS.c\
    examples/HostInit/Main.c examples/HostInit/HostInit.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/xedge/src/xedge.c examples/xedge/src/led.c\
    examples/xedge/XedgeZip.c -lpthread -lm -ldl
```

Integration notes:

- `Main.c` and `HostInit.c` are designed for a non-embedded host build. For an RTOS build, study these files and set up similar startup code for your environment.
- At minimum, your RTOS build needs a dedicated thread that runs `barracuda(void)`, which does not return. Use stack size `BA_STACKSZ` bytes.
- `xedge.c` is the Xedge C startup code.
- `led.c` includes example [Lua bindings](https://realtimelogic.com/ba/doc/en/GettingStarted.html#UsingLSP) for device control. If you do not need these examples, define `-DNO_XEDGE_AUX` or remove the `xedgeOpenAUX()` call in `xedge.c`.
- For embedded builds, consider testing without file system support first. Remove `BaFile.c` from your build and compile with `-DNO_BAIO_DISK`, which is used by `xedge.c`.
- If the target does not provide a suitable allocator, include `src/dlmalloc.c` and initialize it using the pattern in `examples/HostInit/Main.c`. Use the target's allocator instead when its behavior and integration requirements are known.

These Xedge instructions are excerpted from the [Xedge full SDK build guide](https://realtimelogic.com/ba/examples/xedge/readme.html), which includes additional release-build details. File locations differ slightly in this repository.

### Cross-Compiling Xedge

Include the files listed above in your IDE or Makefile. If the target does not provide a suitable allocator, BAS includes `src/dlmalloc.c` as one option. See the [FreeRTOS readme](../src/arch/FreeRTOS/README.txt) for an example of how to assemble the required components. Adapt the startup, allocator, and porting layers to the selected RTOS and network stack.

For a quick RTOS trial, run Xedge on an ESP32 using FreeRTOS and lwIP, even if your final target uses another RTOS or device. The ESP32 is a practical learning target. Use the [precompiled ESP32 Xedge binaries](https://realtimelogic.com/downloads/bas/ESP32/).

## Enabling and Disabling Features

Feature selection is controlled by the build, the target's `TargConfig.h`, and macros used by `src/BAS.c` and the host sources. Defaults vary by platform, so verify the effective configuration before adding or removing a macro.

- `USE_DBGMON=1`: Include [Lua debugger support](https://makoserver.net/articles/Lua-and-LSP-Debugging).
- `USE_REVCON=1`: Enable reverse connections for the connection bridge feature in [SharkTrustX](https://realtimelogic.com/products/SharkTrustX/).
- `USE_OPCUA=1`: Enable OPC UA support when the build also includes the required OPC UA Lua resources and C module.
- `USE_FORKPTY=1`: Enable the [advanced process management API](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#forkptylib), available for Linux and QNX. This API is required for the [CGI plugin](https://github.com/RealTimeLogic/LSP-Examples/tree/master/CGI) and the [web shell](https://makoserver.net/articles/Linux-Web-Shell).
- `USE_REDIRECTOR=1`: Enable the [reverse proxy](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#reverseproxy).
- `USE_UBJSON=1`: Enable [Universal Binary JSON](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#ubjson).
- `NO_LDEBUG`: Exclude the Lua `debug` module.

### Mako Server Macros

- `NO_SHARKTRUST`: Do not include `tokengen.c`; disables the built-in SharkTrustX key.
- `USE_LUAINTF`: Enable loading [external Lua modules](https://makoserver.net/documentation/c-modules/). When using source builds, you can alternatively integrate additional [Lua bindings](https://realtimelogic.info/swig/) directly into your build.

### Xedge Macros

- `NO_SHARKTRUST`: Disable SharkTrustX integration.
- `NO_ENCRYPTIONKEY`: Exclude the encryption-key support used by the software Trusted Platform Module (softTPM). Without this macro, Xedge includes `EncryptionKey.h` or a generated `NewEncryptionKey.h`, depending on the build configuration.
- `NO_BAIO_DISK`: Build Xedge without the local file system integration. Use this for targets without a file system or for an initial minimal port.
- `NO_XEDGE_AUX`: Do not call `xedgeOpenAUX()`, which is where you typically register your own Lua bindings.

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
| Quadros | `inc/arch/Quadros` | `src/arch/Quadros/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| SMX | `inc/arch/NET/SMX-NET` `inc/arch/SMX` | `src/arch/SMX/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Azure RTOS | `inc/arch/ThreadX` | `src/arch/ThreadX/ThreadLib.c` `src/arch/ThreadX/SoDisp.c` |
| VxWorks | `inc/arch/VxWorks` | `src/arch/VxWorks/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Windows | `inc/arch/NET/Windows` `inc/arch/Windows` | `src/arch/Windows/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Windows CE | `inc/arch/NET/CE` `inc/arch/Windows` | `src/arch/ce/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Zephyr | `inc/arch/Zephyr` | `src/arch/Zephyr/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |

The generic `inc` directory must also be in the include path. The presence of a porting directory does not prove compatibility with every version of the target SDK. Compile against the actual SDK and confirm its configuration and networking APIs.

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
ccintarm -G -c99 -os_dir C:/ghs/int1144 -bsp simarm -o xedge\
    -non_shared -DMAXTHREADS=3\
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
wr-cc -o examples/MakoServer/mako -static -fmerge-all-constants -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DBA_HAS_ANSI_IO -DMAKO -DUSE_SQL=0\
    -Iinc -Iinc/arch/VxWorks -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/VxWorks/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lnet
```

### FreeRTOS

#### i.MX RT1020

Download a ready-to-compile and ready-to-run project for [i.MX RT1020](https://realtimelogic.com/downloads/bas/rt1020/).

#### ESP32

![Xedge32](https://realtimelogic.com/images/xedge/finger-icon.png)

[Xedge32](https://github.com/RealTimeLogic/Xedge32) is an Xedge version for ESP32 and ESP32-S3 microcontrollers.

## Tutorials

- [What is an Embedded Application Server?](https://realtimelogic.com/articles/What-is-an-Embedded-Application-Server)
- [Using Lua for IoT and Web Application Development](https://realtimelogic.com/articles/Lua-FastTracks-Embedded-Web-Application-Development)
- [Online Interactive Lua and LSP Tutorials](https://tutorial.realtimelogic.com/)
- [Modern Approach to Embedding a Web Server in a Device](https://realtimelogic.com/articles/Modern-Approach-to-Embedding-a-Web-Server-in-a-Device)
- [The Ultimate Guide to Embedded Web Server Technology](https://realtimelogic.com/articles/Embedded-Web-Server-Tutorials)
- [A Large List of Mako/BAS Tutorials](https://makoserver.net/tutorials/)
- [Ready-to-Run Examples](https://github.com/RealTimeLogic/LSP-Examples)

## Online Documentation

- [Introduction](https://realtimelogic.com/ba/doc/)
- [Lua APIs](https://realtimelogic.com/ba/doc/en/lua/lua.html)
- [BAS/BWS C/C++ API](https://realtimelogic.com/ba/doc/en/C/reference/html/index.html)
- [SharkSSL TLS Stack C/C++ API](https://realtimelogic.com/ba/doc/en/C/shark/index.html)
