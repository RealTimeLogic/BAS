# BWS and BAS Amalgamated
## Embedded Web Server and IoT Toolkit C Code Libraries
- **BWS:** [Barracuda Embedded Web Server C library](https://realtimelogic.com/products/barracuda-web-server/)
- **BAS:** [Barracuda Embedded App Server C library](https://realtimelogic.com/products/barracuda-application-server/)
- **[Three license options](#license)**: GPLv2, free commercial license, and standard royalty-free commercial license.

<img align="left" width="200" height="200" alt="Barracuda App Server Logo" src="https://realtimelogic.com/ba/doc/logo.svg">

The BWS and BAS C code libraries support the following platforms: Embedded Linux, Zephyr, FreeRTOS+TCP or lwIP, VxWorks, QNX, INTEGRITY, Windows & Win CE, Azure RTOS, Nucleus RTOS, embOS, RTXC, SMX, MQX, RTOS-32, Mac, and UNIX/POSIX.

RTL has crafted the two amalgamated libraries from the full BAS SDK; **Amalgamated** indicates that we have combined all separate C files into one file, simplifying the compilation and integration process into your build. The products run on anything from tiny FPGA's to online cloud servers.

Both BWS and BAS are application servers that share the same porting layer modules, making their integration process almost identical. BAS includes all the C APIs available in BWS, but it goes a step further by focusing on the Lua language. This makes BAS handy for rapid development and offers an easier entry point for secure web and IoT app development. Check out the tutorial [Embedded Web Server vs. Embedded Application Server](https://realtimelogic.com/products/web-server-vs-application-server/) if you are new to application server technology.

## BWS

The Barracuda Embedded Web Server is a small embedded HTTP(S) and WebSocket C source code library pre-integrated with the SharkSSL TLS stack. The source code is optimized for compact, deeply embedded devices.

[![Barracuda Embedded Web Server](https://realtimelogic.com/GZ/images/BarracudaWebServer.svg)](https://realtimelogic.com/products/barracuda-web-server/)

- [Barracuda Embedded Web Server Product Page](https://realtimelogic.com/products/barracuda-web-server/)

## BAS

The Barracuda App Server, powered by BWS, is a powerful IoT toolkit that equips engineers with the necessary tools to create secure and efficient web and IoT products. While you can develop applications using C/C++ on BAS, it also features the powerful Lua engine. This engine is designed for ease of use, enabling developers to construct up to 80% of their embedded applications in Lua without compromising on efficiency.

[![Barracuda App Server Product Page](https://realtimelogic.com/GZ/images/BarracudaAppServerBLK.svg)](https://realtimelogic.com/products/barracuda-application-server/)

- [Barracuda App Server Product Page](https://realtimelogic.com/products/barracuda-application-server/)

When learning a new framework, it's not mastering the language that's time-consuming - it's understanding the API. The Lua IoT and Web APIs are considerably simpler and more intuitive than their lower-level C counterparts. For this reason, opting for Lua can significantly reduce your development time. Using Lua instead of C for network-enabled applications offers several compelling advantages:

- **Ease of Use:** Lua is renowned for its simplicity and elegance, significantly reducing the learning curve for new developers and speeding up the development process.
- **Flexibility:** Lua's lightweight and flexible nature makes it ideal for scripting customizable behaviors in network applications without the complexity often associated with C.
- **Rapid Prototyping:** Lua enables faster prototyping and iteration, which is crucial in IoT development.
- **Enhanced Security:** Lua provides a higher-level approach that abstracts many low-level operations, reducing the risk of security flaws that are more prevalent in direct system-level coding with C.
- **Efficiency:** Although Lua is an interpreted language, it runs efficiently on embedded systems, thanks to its well-optimized engine and the ability to offload performance-critical tasks back to C modules if necessary.
- [Lua Server Pages](https://realtimelogic.com/products/lua-server-pages/) (LSP for short) is a [Lua](https://www.lua.org/about.html) API that includes a compact and efficient Lua web framework, non-blocking asynchronous sockets, and many (I)IoT protocols.

### Are you already using Lua? Not a problem!

The Lua source code is included in BAS.c, but you can exclude this code by compiling with -DUSE_BA_LUA=0. You can compile and link BAS.c with your Lua version by excluding our version.



## C Code Components

Include the following in your build:

* Amalgamated C code library (amalgamation of many files); one of:
  - BWS.c: The Barracuda Web Server
  - BAS.c: The Barracuda App Server
* ThreadLib.c: Kernel porting layer
* SoDisp.c: TCP/IP porting layer (the socket dispatcher)
* BaFile.c: File system porting layer (optional)

## Who Can Use BAS Amalgamated

BAS Amalgamated runs efficiently on a Cortex M4 running @100Mhz and up; however, most microcontrollers will need external memory. See the Memory section in the [Porting Barracuda to an Embedded System](https://realtimelogic.com/ba/doc/en/introduction.html#porting) for details.

## Before Downloading the Source Code

The Barracuda App Server is a feature-rich product. To ease into its diverse options, you might find it helpful to start by downloading one of the [pre-compiled binaries with included Lua tutorials](https://realtimelogic.com/downloads/bas/), which are complemented with handy Lua tutorials, prior to accessing the source code.

## How To Compile

To compile BAS, include BAS.c, src/arch/XXX/ThreadLib.c, src/arch/NET/XXX/SoDisp.c, and optionally src/DiskIo/XXX/BaFile.c, where XXX is the required porting layer. In addition, the compiler's include path must include inc/arch/XXX, inc/arch/NET/XXX, and optionally inc/DiskIo/XXX

BAS is extremely flexible and can, for example, be used as follows:
* Compiled and integrated with RTOS powered monolithic firmware
* Compiled and run as a process on a High Level Operating System (HLOS) such as embedded Linux
* Compiled and integrated with a standard (desktop) application

BAS Amalgamated is very easy to compile and several command line compilation examples are shown below. You can add the code to any IDE and Makefile system.

## Examples

BAS and BWS are versatile C libraries designed for seamless integration into a wide range of devices and programs, from small ASIC-powered gadgets to Windows applications. To help you get started with the libraries, we provide three ready-to-use reference builds:

- The [Mako Server](#mako-server-hlos) (BAS)
- The [Xedge](#xedge-rtos) (BAS)
- The [C++ WebSocket Server Example](examples/C-WebSockets/README.md) (BWS)
- [Designing Embedded RESTful Services in C and C++](examples/C-RESTful-Service/README.md) (BWS)

* **BAS Examples:** The Mako Server is designed for HLOS, and Xedge is designed for RTOS. Both examples provide a [Lua foundation](https://realtimelogic.com/products/lua-server-pages/) enabling rapid interactive development of web, IoT, and business logic (high level logic).
* **BWS Example:** The C++ WebSocket Server Example shows how to implement everything using C code.

### Mako Server (HLOS)

The [Mako Server Example](https://realtimelogic.com/ba/doc/en/Mako.html) can be compiled for High Level Operating Systems (HLOS) such as Windows, (embedded) Linux, QNX, and [VxWorks](#vxworks).

![Mako Server Build Process](https://realtimelogic.com/ba/doc/en/examples/MakoServer/Build-Mako-Diagram.svg)


#### Windows:

Use the Visual Studio project file found in: BAS/examples/MakoServer/VcMake; however, make sure to download SQLite or disable the SQLite build in the project file. 

#### Linux:

See the tutorial [Embedded Linux Web Interface Design](https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design) for details; however, the server can be downloaded and compiled for Linux as follows:

```
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
```
For a more **robust compilation option**, use the [included Ansible script](Ansible.md). You may also [install the server as a systemd process](https://github.com/RealTimeLogic/MakoInstaller) using an Ansible script.

Download and cross-compile the code for (embedded) Linux as follows:

```
export CC=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
Change the above CC environment variable to your cross-compiler.
```

Download and cross-compile the code, including [Lua GPIO bindings](https://github.com/vsergeev/lua-periphery), for embedded Linux as follows:

```
export CROSS_COMPILE=/opt/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/RaspberryPiBuild.sh)
```


#### Details:

The following example builds the Mako Server without SQLite for HLOS. You can copy and paste the command into a Linux shell. Note that you can also download pre-compiled Mako Server versions for many operating systems on the [Mako Server Web Site](https://makoserver.net/).

```
gcc -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO -DUSE_SQL=0\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lpthread -lm -ldl
```

The above command works for any HLOS with a GCC compiler (and derivatives), including cross compiling for embedded Linux, compiling for QNX, and compiling for VxWorks 7. Simply replace 'gcc' with the applicable compiler. Note that VxWorks is a cross between HLOS and RTOS; thus, you may consider using Xedge instead (details below). BAS supports VxWorks 5 and up.

See the [Mako Server's Build Documentation](https://realtimelogic.com/ba/examples/MakoServer/readme.html) for details on macros and other important information, but note that the amalgamated version in the standard SDK works differently.

After running the above compilation command and after [building mako.zip using the mako script](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build), start the server as follows:
```
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

**Compiling for Windows**

The following example compiles and links the server as a 64 bit Windows application using the Visual Studio command line compiler (cl). Note that we include two additional C files. These two C files make it possible to run the server as a Windows service. Note that the [pre-compiled Mako Server for Windows](https://makoserver.net/download/windows/) is compiled as a 32 bit application.

```
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

The above command requires setting up the [command line for 64 bit compilation](https://docs.microsoft.com/en-us/cpp/build/how-to-enable-a-64-bit-visual-cpp-toolset-on-the-command-line?view=msvc-170). The above can be copied and pasted "as is" into a command window.

**Including SQLite in the Build**

The Mako Server can optionally be linked with the SQLite database engine. The following command shows how to include SQLite and activate the [Lua SQLite bindings](https://realtimelogic.com/ba/doc/en/lua/luasql.html). However, before running the compilation command, [download SQLite](https://www.sqlite.org/download.html) and copy sqlite3.c and sqlite3.h to the BAS/src directory.

```
gcc -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DLUA_USE_LINUX -DMAKO\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm -ldl
```

### Xedge (RTOS)

The Xedge is designed as a Lua foundation and an interactive development environment that enables the development of Lua code directly on an embedded device. When you are done with the development, the code enables several options for producing a release (final product). In the following instructions, we cover how to build the tool. See the [online Xedge documentation](https://realtimelogic.com/ba/doc/en/Xedge.html) for details on how to use this tool.

[![Xedge](https://realtimelogic.com/images/xedge/v1/Xedge.png)](https://simplemq.com/videos/Xedge-IDE-intro.mp4)

#### Compiling and Integrating Xedge

The Xedge IDE tool comprises a Single-Page Web Application (SPA) and supporting server-side code. These resources are crucial and must be included in the build.

![Xedge Build Process](https://realtimelogic.com/ba/doc/en/examples/xedge/Build-Xedge-Diagram.svg)

> Please note that the above compilation process is just an example  - the Barracuda App Server platform is flexible and can support any deployment strategy you require. For a quick overview of compiling Xedge, check out the tutorial [Rapid Firmware Development with the Barracuda App Server](https://realtimelogic.com/articles/Rapid-Firmware-Development-with-the-Barracuda-App-Server). This tutorial also shows how to replace the integrated IDE with a custom IoT application. While it uses an ESP32, the same method applies to any CPU.

Fetch the repos and build the resource ZIP file:

```
# Fetch the Barracuda App Server
git clone https://github.com/RealTimeLogic/BAS.git
# Fetch the Barracuda App Server's resources
git clone https://github.com/RealTimeLogic/BAS-Resources.git
# Go to the build directory
cd BAS-Resources/build/
# Run the build script. If on Windows, use Xedge.cmd
# When prompted, initially select n for OPC-UA, s for small cacert.shark,
# and n for compressing the files
. Xedge.sh
# The ZIP file is converted to a C array by the build script;
# copy this to the Xedge directory
cp XedgeZip.c ../../BAS/examples/xedge/
cd ../../BAS
```

The file **XedgeZip.c** embodies Xedge's resources, transformed into a C file. You can uncover this file's resources and detailed build instructions in the [BAS-Resources Repository](https://github.com/RealTimeLogic/BAS-Resources/tree/main/build).

To compile Xedge, include:

- **Required :** BAS.c, xedge.c, Xedge startup code (examples: Main.c and HostInit.c), XedgeZip.c, ThreadLib.c, SoDisp.c, and dlmalloc.c (required for most RTOS builds)
- **Optional :** BaFile.c and led.c

In the following build example, we use Linux to compile the server as a standalone application. For embedded devices, simply include the relevant source files in your build system instead.

```
gcc -o xedge -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    -DLUA_USE_LINUX\
    src/BAS.c\
    examples/HostInit/Main.c examples/HostInit/HostInit.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/xedge/src/xedge.c examples/xedge/src/led.c\
    examples/xedge/XedgeZip.c -lpthread -lm -ldl
```

- The files Main.c and HostInit.c are designed for a non-embedded host build. You need to study those two files and set up similar code for your RTOS environment. At a minimum, you need a dedicated thread to run the function barracuda(void), which does not return, with stack size BA_STACKSZ (in bytes).

- The file xedge.c is the Xedge's C startup code.

- The led.c file includes example [Lua bindings](https://realtimelogic.com/ba/doc/en/GettingStarted.html#UsingLSP) for device control. If you don't need these examples, you can exclude them by either defining -DNO_XEDGE_AUX or by removing the xedgeOpenAUX() call in xedge.c. That said, you'll eventually want to implement your own Lua APIs for device management, so we recommend keeping an empty xedgeOpenAUX() in your build as a starting point. The [led.c file](https://github.com/RealTimeLogic/BAS/blob/main/examples/xedge/src/led.c) contains several practical examples worth studying to guide your own implementation.

- For embedded, a recommendation is to initially tests the server without file system support. Remove the file BaFile.c from your build and compile with -DNO_BAIO_DISK, a macro used by the file xedge.c.

- If you get a link error that includes dlmalloc, include src/dlmalloc.c in your build and initialize the allocator as shown in the file examples/HostInit/Main.c.

- **Note:** The Xedge build instructions provided above are an excerpt from the [Xedge full SDK build guide](https://realtimelogic.com/ba/examples/xedge/readme.html), which includes additional details such as creating a release build. Please note that there are minor differences in file locations between the two.

**Cross Compiling Xedge for Embedded Systems**

Include the files as instructed above in your IDE or Makefile. Most embedded systems require an efficient allocator, which is included (dlmalloc). See, for example, the [FreeRTOS Readme File](src/arch/FreeRTOS/README.txt) for how to set up all required components. Most embedded RTOSs require the same setup.

**Pre-compiled binaries:** Try running the server on an ESP32 using FreeRTOS and lwIP, even if you plan to use a different RTOS or device. The ESP32 is user-friendly and great for learning about RTOS. Use the [pre-compiled ESP32 Xedge binaries](https://realtimelogic.com/downloads/bas/ESP32/) for this purpose.


## Enabling and Disabling Features

BAS Amalgamated (BAS.c) includes features that are by default not compiled. These features can be enabled by the following macros. The macros can be enabled on any platform, including RTOS, unless stated otherwise.

* USE_DBGMON=1: Include [Lua Debugger Support](https://makoserver.net/articles/Lua-and-LSP-Debugging).
* USE_REVCON=1: Enable the reverse connection if you plan on using the connection bridge feature in [SharkTrustX](https://realtimelogic.com/products/SharkTrustX/). Note that both Xedge and the Mako Server include the [Let's Encrypt plugins acmebot and acmedns](https://realtimelogic.com/ba/doc/en/Mako.html#acmebot).
* USE_OPCUA=1: The OPC-UA stack is implemented in Lua and can be found in mako.zip/.lua/opcua. The OPC-UA stack requires a C module. Enable this flag when compiling BAS if you plan on using OPC-UA.
* USE_FORKPTY=1: Enable the [advanced process management API](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#forkptylib), which is available for Linux and QNX. This API is required if you plan on using the [CGI plugin](https://github.com/RealTimeLogic/LSP-Examples/tree/master/CGI) or the [web shell](https://makoserver.net/articles/Linux-Web-Shell).
* USE_REDIRECTOR=1: Enable the [Reverse Proxy](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#reverseproxy)
* USE_UBJSON=1: Enable [Universal Binary JSON](https://realtimelogic.com/ba/doc/en/lua/auxlua.html#ubjson)

* LUA_NUMBER_INTEGER=1
    * Makes Lua use integer only and excludes floating point numbers. To re-enable floating point, compile the code with:

* LUA_NUMBER_INTEGER=0
  * See inc/arch/XXX/luaconf.h for details.

* NO_LDEBUG
  * Exclude the Lua "debug" module

* Mako Server Specifics:
  * NO_SHARKTRUST
    * Do no include tokengen.c; disable built-in SharkTrustX key
  * USE_LUAINTF
    * This macro enables loading [external Lua modules](https://makoserver.net/documentation/c-modules/). When not using a pre-built Mako Server and when using the source code you can instead choose to integrate additional [Lua bindings](https://realtimelogic.info/swig/) directly with your build.

* Xedge Specifics:
  * NO_SHARKTRUST
  * NO_ENCRYPTIONKEY
    * Do not include NewEncryptionKey.h; disable soft TPM
  * NO_XEDGE_AUX
    * Do not call xedgeOpenAUX() (your own Lua bindings)


# Porting Layers

[Contact Real Time Logic](https://realtimelogic.com/contactus/) should you have a problem with the porting layer when used with your version of the target APIs.

| OS+TCP | include | source |
| ----------- | ----------- | ----------- |
| Digi | inc/arch/Digi | src/arch/Digi/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| embOS | inc/arch/NET/embOSIP inc/arch/embOS | src/arch/embOS/ThreadLib.c src/arch/NET/embOSIP/SoDisp.c |
| FreeRTOS+lwIP | inc/arch/NET/LwIP inc/arch/FreeRTOS | src/arch/FreeRTOS/ThreadLib.c src/arch/NET/LwIP/SoDisp.c |
| FreeRTOS+TCP | inc/arch/FreeRTOS | src/arch/FreeRTOS/ThreadLib.c src/arch/FreeRTOS/SoDisp.c |
| INTEGRITY | inc/arch/NET/Posix inc/arch/INTEGRITY | src/arch/INTEGRITY/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| INtime | inc/arch/NET/INtime inc/arch/INtime | src/arch/INtime/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Linux+epoll | inc/arch/NET/epoll inc/arch/Posix | src/arch/Posix/ThreadLib.c src/arch/NET/epoll/SoDisp.c |
| MQX | inc/arch/NET/MQX inc/arch/MQX | src/arch/MQX/ThreadLib.c src/arch/NET/MQX/SoDisp.c |
| NuttX | inc/arch/NET/Posix inc/arch/Posix | src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Nucleus | inc/arch/NET/Nucleus inc/arch/Nucleus | src/arch/Nucleus/ThreadLib.c src/arch/NET/Nucleus/SoDisp.c |
| Posix (Linux, Mac, QNX) | inc/arch/NET/Posix inc/arch/Posix | src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Quadros | inc/arch/Quadros | src/arch/Quadros/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| SMX | inc/arch/NET/SMX-NET inc/arch/SMX | src/arch/SMX/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Azure RTOS | inc/arch/ThreadX | src/arch/ThreadX/ThreadLib.c src/arch/ThreadX/SoDisp.c |
| VxWorks | inc/arch/VxWorks | src/arch/VxWorks/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Windows | inc/arch/NET/Windows inc/arch/Windows | src/arch/Windows/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Windows CE | inc/arch/NET/CE inc/arch/Windows | src/arch/Windows/ThreadLib.c src/arch/NET/generic/SoDisp.c |
| Zephyr | inc/arch/Zephyr | src/arch/Zephyr/ThreadLib.c src/arch/NET/generic/SoDisp.c |

The generic directory 'inc' must also be in the include path.

# Additional Build Examples

## INTEGRITY

The following example shows how to compile Xedge for Green Hills INTEGRITY. You can also [download a ready-to-run Xedge GHS Multi IDE project](https://realtimelogic.com/downloads/bas/xedge-ghs-build.zip).

```
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



## VxWorks

We recommend [Xedge](#xedge-rtos) for VxWorks; however, The following example shows how to compile the Mako Server for VxWorks. See the [Barracuda App Server VxWorks Build Page](https://realtimelogic.com/downloads/bas/VxWorks/) for details.

```
wr-cc -o examples/MakoServer/mako -static -fmerge-all-constants -O3 -Os\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DBA_HAS_ANSI_IO -DMAKO -DUSE_SQL=0\
    -DLUA_NUMBER_INTEGER=0\
    -Iinc -Iinc/arch/VxWorks -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/VxWorks/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    -lnet
```

## Azure RTOS using IAR

The following example shows how to compile the generic BAS library for Azure RTOS using IAR for ARM. We have the following directories tx (ThreadX), nx (NetX), and BAS.

```
iccarm -e -c^
  -Itx -Inx^
  -IBAS/inc -IBAS/inc/arch/ThreadX^
  BAS/src/BAS.c
```

Using the above compilation settings, you would also need to include src/arch/ThreadX/ThreadLib.c, src/arch/ThreadX/SoDisp.c, and optionally src/DiskIo/FileX/BaFile.c.

## FreeRTOS

#### i.MX RT1020
[Download](https://realtimelogic.com/downloads/bas/?target=RT1020) a ready to compile and run project for [i.MX RT1020](https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1020-evaluation-kit:MIMXRT1020-EVK).

#### ESP32

![Xedge32](https://realtimelogic.com/images/xedge/finger-icon.png)

[Xedge32](https://github.com/RealTimeLogic/Xedge32) is an Xedge version for the ESP32 and ESP32-S3 microcontrollers.


# Barracuda App Server Tutorials

* [What is an Embedded Application Server?](https://realtimelogic.com/articles/What-is-an-Embedded-Application-Server)
* [Using Lua for IoT and Web Application Development](https://realtimelogic.com/articles/Lua-FastTracks-Embedded-Web-Application-Development)
* [Online Interactive Lua and LSP Tutorials](https://tutorial.realtimelogic.com/)
* [Modern Approach to Embedding a Web Server in a Device](https://realtimelogic.com/articles/Modern-Approach-to-Embedding-a-Web-Server-in-a-Device)
* [The Ultimate Guide to Embedded Web Server Technology](https://realtimelogic.com/articles/Embedded-Web-Server-Tutorials)
* [A Large List of Mako/BAS Tutorials](https://makoserver.net/tutorials/)
* [Ready To Run Examples (GitHub)](https://github.com/RealTimeLogic/LSP-Examples)


## Online Documentation

* [Introduction](https://realtimelogic.com/ba/doc/)
* [Lua APIs](https://realtimelogic.com/ba/doc/en/lua/lua.html)
* [BAS/BWS C/C++ API](https://realtimelogic.com/ba/doc/en/C/reference/html/index.html)
* [SharkSSL TLS Stack C/C++ API](https://realtimelogic.com/ba/doc/en/C/shark/index.html)

## License

BAS and BWS Amalgamated are licensed for use under the **GPLv2** or a
standard **commercial license**. For our users who cannot use BAS and BWS
Amalgamated under GPLv2,
[a commercial license is available](https://realtimelogic.com/contactus/license/).
A **free commercial license for small companies** is available.  See the
startup license page for details:
https://realtimelogic.com/startuplic/

**GPLv2 details:**

The following clarification is for users who choose to use GPLv2 and
to distribute derivative work. The GPLv2 license applies to all BAS and BWS APIs
including, but not limited to the BAS and BWS C APIs, BAS Lua APIs, and web
content hosted by a BAS and BWS derivative product. Web content hosted by the
server is considered part of the application as a whole.

GPLv2: https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html

## Export restrictions

This distribution includes
[cryptographic software](https://realtimelogic.com/products/sharkssl/). The
country in which you currently reside may have restrictions on the
import, possession, use, and/or re-export to another country, of
encryption software.  BEFORE using any encryption software, please
check your country's laws, regulations and policies concerning the
import, possession, or use, and re-export of encryption software, to
see if this is permitted.  See http://www.wassenaar.org/ for more
information.

The U.S. Government Department of Commerce, Bureau of Industry and
Security (BIS), has classified this software as Export Commodity
Control Number (ECCN) 5D002.C.1, which includes information security
software using or performing cryptographic functions with asymmetric
algorithms. The form and manner of this distribution makes it
eligible for export under the License Exception ENC Technology
Software Unrestricted (TSU) exception (see the BIS Export
Administration Regulations, Section 740.13) for both object code and
source code.

The following page provides details on the included cryptographic
software: https://github.com/RealTimeLogic/SharkSSL
