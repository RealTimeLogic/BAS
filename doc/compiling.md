# Compiling BAS and BWS

This guide explains how to build and integrate the amalgamated BAS and BWS C libraries. It provides generic source integration details for firmware, embedded Linux, RTOS targets, cross-compilation, custom BAS builds, and BWS builds.

If you plan to use BAS, the fastest path is usually the BAS download page:

https://realtimelogic.com/downloads/bas/

That page includes a drop-down menu for selected operating systems and opens purpose-built instructions for each platform, including precompiled packages and platform-specific build options. Use this generic guide when your operating system is not covered there, when you need lower-level source integration details, or when you plan to use BWS.

See [README.md](README.md) for licensing.

## Choose Your Path

- **BAS, quickest path:** Use https://realtimelogic.com/downloads/bas/ and select your operating system. The platform pages are easier to follow than these generic instructions when your target is listed.
- **BAS on a high-level operating system:** See [Mako Server (HLOS)](#mako-server-hlos).
- **BAS on RTOS or embedded firmware:** See [Xedge (RTOS)](#xedge-rtos), [RTOS Build Examples](#rtos-build-examples), and [Porting Layers](#porting-layers).
- **BWS:** Follow the [Basic Compile Model](#basic-compile-model) and include `src/BWS.c` plus the required porting layers.
- **Custom BAS builds:** Follow the [Basic Compile Model](#basic-compile-model) and include `src/BAS.c` plus the required porting layers.

## What Is Included

The repository contains two amalgamated libraries created from the full BAS SDK. "Amalgamated" means that many separate C files have been combined into one primary source file, making compilation and integration easier. The libraries can run on targets ranging from compact FPGA-based systems to cloud-hosted services.

- **BWS.c:** Barracuda Embedded Web Server.
- **BAS.c:** Barracuda App Server. BAS includes all BWS C APIs and adds Lua, [Lua Server Pages](https://realtimelogic.com/products/lua-server-pages/), and higher-level web and IoT APIs.

BWS and BAS share the same porting layer modules, so their integration process is almost identical. BAS is often the easiest entry point for secure web and IoT application development because much of the application logic can be written in Lua while performance-critical code can remain in C or C++.

> See [Embedded Web Server vs. Embedded Application Server](https://realtimelogic.com/products/web-server-vs-application-server/) if you are new to application server technology.

### BWS

The [Barracuda Embedded Web Server](https://realtimelogic.com/products/barracuda-web-server/) is a small embedded HTTP(S) and WebSocket C source code library pre-integrated with the SharkSSL TLS stack. The source code is optimized for compact, deeply embedded devices.

[![Barracuda Embedded Web Server](https://realtimelogic.com/GZ/images/BarracudaWebServer.svg)](https://realtimelogic.com/products/barracuda-web-server/)

### BAS

The [Barracuda App Server](https://realtimelogic.com/products/barracuda-application-server/) is powered by BWS and adds the Lua engine and Lua Server Pages. You can build applications in C or C++, but BAS is designed so a large part of an embedded application can be developed in Lua without giving up the option to write performance-critical or hardware-specific modules in C.

[![Barracuda App Server Product Page](https://realtimelogic.com/GZ/images/BarracudaAppServerBLK.svg)](https://realtimelogic.com/products/barracuda-application-server/)

Lua can reduce development time for network-enabled applications because its APIs are higher level than the corresponding C APIs. BAS includes:

- [Lua Server Pages](https://realtimelogic.com/products/lua-server-pages/), a compact Lua web framework.
- Non-blocking asynchronous sockets.
- Web, IoT, and industrial protocol support.
- The option to add [custom Lua bindings in C/C++](https://tutorial.realtimelogic.com/Lua-Bindings.lsp).

If you already use Lua, you can exclude the bundled Lua source by compiling with `-DUSE_BA_LUA=0` and linking BAS with your Lua version.

## C Code Components

Include the following files in your build:

- One amalgamated library:
  - `src/BWS.c` for the Barracuda Web Server.
  - `src/BAS.c` for the Barracuda App Server.
- `src/arch/XXX/ThreadLib.c`: kernel porting layer.
- `src/arch/NET/XXX/SoDisp.c`: TCP/IP porting layer, also called the socket dispatcher.
- `src/DiskIo/XXX/BaFile.c`: optional file system porting layer.

BAS and BWS Amalgamated can run efficiently on a Cortex M4 running at 100 MHz and up, but most microcontrollers need external memory. See the memory section in [Porting Barracuda to an Embedded System](https://realtimelogic.com/ba/doc/en/introduction.html#porting) for details.

## Basic Compile Model

To compile BAS or BWS, select one amalgamated library and add the required porting layers:

- Use `src/BAS.c` for the Barracuda App Server.
- Use `src/BWS.c` for the Barracuda Embedded Web Server.
- Add the required `ThreadLib.c`.
- Add the required `SoDisp.c`.
- Optionally add `BaFile.c` when using file system support.

Replace `XXX` with the required porting layer.

The compiler include path must include:

- `inc`
- `inc/arch/XXX`
- `inc/arch/NET/XXX`
- `inc/DiskIo/XXX`, when using a file system porting layer

BAS and BWS can be used in several deployment models:

- Integrated into RTOS-powered monolithic firmware.
- Built and run as a process on a high-level operating system such as embedded Linux.
- Integrated into a standard desktop application.

The amalgamated source is easy to compile from the command line, but you can also add it to any IDE, Makefile, CMake project, or vendor build system.

## Reference Builds and Examples

BAS and BWS are designed for a wide range of devices and programs, from small ASIC-powered gadgets to Windows applications. The main reference builds are:

- [Mako Server](#mako-server-hlos), a BAS build for high-level operating systems.
- [Xedge](#xedge-rtos), a BAS build for RTOS and embedded systems.
- [C++ WebSocket Server Example](examples/C-WebSockets/README.md), a BWS example.
- [Designing Embedded RESTful Services in C and C++](examples/C-RESTful-Service/README.md), a BWS example.

BAS examples such as Mako Server and Xedge provide a [Lua foundation](https://realtimelogic.com/articles/Using-Lua-for-Embedded-Development-vs-Traditional-C-Code) for rapid development of web, IoT, and business logic. BWS examples show how to implement services directly in C or C++.

## Mako Server (HLOS)

The [Mako Server example](https://realtimelogic.com/ba/doc/en/Mako.html) can be compiled for high-level operating systems such as Windows, embedded Linux, QNX, and VxWorks.

![Mako Server Build Process](https://realtimelogic.com/ba/doc/en/examples/MakoServer/Build-Mako-Diagram.svg)

### Windows

Use the Visual Studio project file in `examples/MakoServer/VcMake`. Download SQLite before building, or disable the SQLite build in the project file.

### Linux

For Linux and embedded Linux, see [Embedded Linux Web Interface Design](https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design). You can download and compile the server as follows:

```sh
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```

For a more robust compilation option, use the [included Ansible script](Ansible.md). You can also [install the server as a systemd service](https://github.com/RealTimeLogic/MakoInstaller) using an Ansible script.

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

The following example builds Mako Server without SQLite for a high-level operating system. It works for GCC and derivatives, including cross-compiling for embedded Linux, QNX, and VxWorks 7. Replace `gcc` with the applicable compiler.

```sh
gcc -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO -DUSE_SQL=0\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lpthread -lm -ldl
```

See the [Mako Server build documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html) for details on macros and other build options. The amalgamated version in this repository differs from the standard SDK layout.

After running the compilation command and [building `mako.zip`](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build), start the server:

```text
$examples/MakoServer/mako

Mako Server. Version 3.7
BAS lib 4920. Build date: Dec  2 2021
Copyright (c) Real Time Logic.

Mounting /tmp/BAS/examples/MakoServer/mako.zip
Server listening on IPv6 port 9357
Server listening on IPv4 port 9357
Loading certificate MakoServer
SharkSSL server listening on IPv6 port 9443
SharkSSL server listening on IPv4 port 9443
```

### Windows Command-Line Build

The following example compiles and links Mako Server as a 64-bit Windows application using the Visual Studio command-line compiler. It includes two additional files that make it possible to run the server as a Windows service.

The [precompiled Mako Server for Windows](https://makoserver.net/download/windows/) is compiled as a 32-bit application.

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

This command requires the [64-bit Visual C++ toolset command line](https://docs.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170).

### Including SQLite

Mako Server can optionally be linked with SQLite and the [Lua SQLite bindings](https://realtimelogic.com/ba/doc/en/lua/luasql.html). Before running the command below, [download SQLite](https://www.sqlite.org/download.html) and copy `sqlite3.c` and `sqlite3.h` to `src`.

```sh
gcc -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm -ldl
```

## Xedge (RTOS)

Xedge is a Lua foundation and interactive development environment for developing Lua code directly on an embedded device. After development, Xedge provides several release options for final products. See the [online Xedge documentation](https://realtimelogic.com/ba/doc/en/Xedge.html) for details.

**Xedge in Developer Mode (video)**
[![Xedge](https://realtimelogic.com/images/xedge/v1/Xedge.png)](https://simplemq.com/videos/Xedge-IDE-intro.mp4)

See the [Xedge product page](https://realtimelogic.com/products/xedge/) for details.


### Compiling and Integrating Xedge

The Xedge IDE includes a single-page web application and supporting server-side code. These resources must be included in the build.

![Xedge Build Process](https://realtimelogic.com/ba/doc/en/examples/xedge/Build-Xedge-Diagram.svg)

The compilation process below is an example. BAS can support the deployment strategy required by your target. For a quick overview, see [Rapid Firmware Development with the Barracuda App Server](https://realtimelogic.com/articles/Rapid-Firmware-Development-with-the-Barracuda-App-Server). The tutorial uses an ESP32, but the same method applies to other CPUs.

Fetch the repositories and build the resource ZIP file:

```sh
# Fetch the Barracuda App Server
git clone https://github.com/RealTimeLogic/BAS.git

# Fetch the Barracuda App Server resources
git clone https://github.com/RealTimeLogic/BAS-Resources.git

# Go to the build directory
cd BAS-Resources/build/

# Run the build script. If on Windows, use Xedge.cmd.
# When prompted, initially select n for OPC-UA, s for small cacert.shark,
# and n for compressing the files.
. Xedge.sh

# The ZIP file is converted to a C array by the build script.
# Copy this file to the Xedge directory.
cp XedgeZip.c ../../BAS/examples/xedge/
cd ../../BAS
```

`XedgeZip.c` contains the Xedge resources converted to a C file. See the [BAS-Resources repository](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build) for details.

To compile Xedge, include:

- **Required:** `BAS.c`, `xedge.c`, Xedge startup code such as `Main.c` and `HostInit.c`, `XedgeZip.c`, `ThreadLib.c`, `SoDisp.c`, and `dlmalloc.c` for most RTOS builds.
- **Optional:** `BaFile.c` and `led.c`.

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

Important integration notes:

- `Main.c` and `HostInit.c` are designed for a non-embedded host build. For an RTOS build, study these files and set up similar startup code for your environment.
- At minimum, your RTOS build needs a dedicated thread that runs `barracuda(void)`, which does not return. Use stack size `BA_STACKSZ` bytes.
- `xedge.c` is the Xedge C startup code.
- `led.c` includes example [Lua bindings](https://realtimelogic.com/ba/doc/en/GettingStarted.html#UsingLSP) for device control. If you do not need these examples, define `-DNO_XEDGE_AUX` or remove the `xedgeOpenAUX()` call in `xedge.c`.
- For embedded builds, consider testing without file system support first. Remove `BaFile.c` from your build and compile with `-DNO_BAIO_DISK`, which is used by `xedge.c`.
- If you get a link error that mentions `dlmalloc`, include `src/dlmalloc.c` and initialize the allocator as shown in `examples/HostInit/Main.c`.

These Xedge instructions are excerpted from the [Xedge full SDK build guide](https://realtimelogic.com/ba/examples/xedge/readme.html), which includes additional release-build details. File locations differ slightly in this repository.

### Cross-Compiling Xedge

Include the files listed above in your IDE or Makefile. Most embedded systems require an efficient allocator, and `dlmalloc` is included for this purpose. See the [FreeRTOS readme](src/arch/FreeRTOS/README.txt) for an example of how to set up the required components. Most embedded RTOS ports use the same pattern.

For a quick RTOS trial, run Xedge on an ESP32 using FreeRTOS and lwIP, even if your final target uses another RTOS or device. The ESP32 is a practical learning target. Use the [precompiled ESP32 Xedge binaries](https://realtimelogic.com/downloads/bas/ESP32/).

## Enabling and Disabling Features

`src/BAS.c` includes optional features that are not compiled by default. Enable features with the following macros. Unless stated otherwise, the macros can be used on any platform, including RTOS targets.

- `USE_DBGMON=1`: Include [Lua debugger support](https://makoserver.net/articles/Lua-and-LSP-Debugging).
- `USE_REVCON=1`: Enable reverse connections for the connection bridge feature in [SharkTrustX](https://realtimelogic.com/products/SharkTrustX/). Xedge and Mako Server include the Let's Encrypt plugins `acmebot` and `acmedns`.
- `USE_OPCUA=1`: Enable OPC-UA support. The OPC-UA stack is implemented in Lua and is found in `mako.zip/.lua/opcua`. It requires a C module.
- `USE_FORKPTY=1`: Enable the [advanced process management API](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#forkptylib), available for Linux and QNX. This API is required for the [CGI plugin](https://github.com/RealTimeLogic/LSP-Examples/tree/master/CGI) and the [web shell](https://makoserver.net/articles/Linux-Web-Shell).
- `USE_REDIRECTOR=1`: Enable the [reverse proxy](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#reverseproxy).
- `USE_UBJSON=1`: Enable [Universal Binary JSON](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#ubjson).
- `NO_LDEBUG`: Exclude the Lua `debug` module.

### Mako Server Macros

- `NO_SHARKTRUST`: Do not include `tokengen.c`; disables the built-in SharkTrustX key.
- `USE_LUAINTF`: Enables loading [external Lua modules](https://makoserver.net/documentation/c-modules/). When using source builds, you can alternatively integrate additional [Lua bindings](https://realtimelogic.info/swig/) directly into your build.

### Xedge Macros

- `NO_SHARKTRUST`: Disable SharkTrustX integration.
- `NO_ENCRYPTIONKEY`: Do not include `NewEncryptionKey.h`; disables the soft TPM.
- `NO_XEDGE_AUX`: Do not call `xedgeOpenAUX()`, which is where you typically register your own Lua bindings.

## Porting Layers

[Contact Real Time Logic](https://realtimelogic.com/contactus/) if you have a problem with a porting layer on your target APIs.

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
| Windows CE | `inc/arch/NET/CE` `inc/arch/Windows` | `src/arch/Windows/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |
| Zephyr | `inc/arch/Zephyr` | `src/arch/Zephyr/ThreadLib.c` `src/arch/NET/generic/SoDisp.c` |

The generic `inc` directory must also be in the include path.

## HLOS Build Examples

See the [Mako Server download page](https://makoserver.net/download/overview/) for additional platform and compile examples.

## RTOS Build Examples

### Zephyr

Zephyr RTOS, with the Xedge port, provides a lightweight real-time environment for IoT and edge computing. Xedge adds support for networking, secure communication, and remote device management. See the [Xedge Zephyr documentation](https://github.com/RealTimeLogic/Xedge4Zephyr) for details.

[![Zephyr](https://upload.wikimedia.org/wikipedia/commons/thumb/6/64/Zephyr_RTOS_logo_2015.svg/500px-Zephyr_RTOS_logo_2015.svg.png)](https://github.com/RealTimeLogic/Xedge4Zephyr)

### NuttX

NuttX is a standards-compliant RTOS with a small footprint, scalable from 8-bit to 64-bit MCUs. With POSIX/ANSI APIs and familiar Unix/VxWorks extensions, it brings powerful functionality to deeply embedded systems. Paired with Xedge, NuttX provides a foundation for secure, connected IoT devices. See the [Xedge NuttX documentation](https://nuttx.apache.org/docs/latest/applications/examples/xedge_demo/index.html) for details.

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
    examples/xedge/src/xedgeInitDiskIo.c\
    examples/xedge/src/led.c\
    -lnet -livfs -lsocket\
```

### VxWorks

Xedge is recommended for VxWorks. The following example shows how to compile Mako Server for VxWorks. See the [Barracuda App Server VxWorks build page](https://realtimelogic.com/downloads/bas/VxWorks/) for details.

```sh
wr-cc -o examples/MakoServer/mako -static -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DBA_HAS_ANSI_IO -DMAKO -DUSE_SQL=0\
    -Iinc -Iinc/arch/VxWorks -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/VxWorks/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lnet
```

### Azure RTOS Using IAR

The following example compiles the generic BAS library for Azure RTOS using IAR for ARM. The example assumes the directories `tx`, `nx`, and `BAS`.

```bat
iccarm -e -c^
  -Itx -Inx^
  -IBAS/inc -IBAS/inc/arch/ThreadX^
  BAS/src/BAS.c
```

With these compilation settings, also include `src/arch/ThreadX/ThreadLib.c`, `src/arch/ThreadX/SoDisp.c`, and optionally `src/DiskIo/FileX/BaFile.c`.

### FreeRTOS

#### i.MX RT1020

[Download](https://realtimelogic.com/downloads/bas/?target=RT1020) a ready-to-compile and ready-to-run project for [i.MX RT1020](https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1020-evaluation-kit:MIMXRT1020-EVK).

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
