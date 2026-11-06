# Barracuda Web Server and App Server

Add a browser interface, REST API, or real-time connection to your device with Real Time Logic's embedded server libraries. Write the application in C/C++, or use Lua for web pages and application logic while keeping hardware access in native code.

This repository contains two C libraries:

- **[Barracuda Embedded Web Server (BWS)](https://realtimelogic.com/products/barracuda-web-server/)** provides HTTP, HTTPS, and WebSockets for C/C++ applications, with SharkSSL handling TLS.
- **[Barracuda App Server (BAS)](https://realtimelogic.com/products/barracuda-application-server/)** includes BWS and adds Lua, [Lua Server Pages (LSP)](https://realtimelogic.com/products/lua-server-pages/), and APIs for web and IoT applications.

Each library is *amalgamated*: its core source is combined into one file. Compile `src/BWS.c` or `src/BAS.c`, then add the thread, network, and optional filesystem files for your target. Both libraries use the same [porting layers](doc/compiling.md#porting-layers), with ports for Linux, Windows, QNX, VxWorks, FreeRTOS, Zephyr, ThreadX, and other systems.

## Get started

For a first look at BAS, choose a [precompiled package for your platform](https://realtimelogic.com/downloads/bas/) or try the [Lua and LSP tutorials in your browser](https://tutorial.realtimelogic.com/).

To build from source, pick an example below. Each native example has its own build instructions. For **Mako Server** on a desktop, server, or embedded Linux system, or **Xedge** in device firmware, see [Compiling Mako Server and Xedge](doc/compiling.md). Mako and Xedge both run Lua/LSP applications on BAS; Xedge also includes a browser-based development environment.

## Examples

| Example | What you can build |
| --- | --- |
| [C RESTful Service](examples/C-RESTful-Service/README.md) | A REST API using the BWS C APIs directly. Start here for a C-only toolchain or when you want to manage routing and memory use yourself. |
| [Modern C++ REST Service](examples/Modern-Cpp-REST-Service/README.md) | A REST API with typed route parameters, response helpers, and a Svelte device console. Requires C++11 or newer and the included `cpp/rest` library. |
| [C++ WebSockets](examples/C-WebSockets/README.md) | A browser chat application that exchanges text and JPEG images. Shows multiple clients, broadcasts, and web content served from disk or embedded in the executable. |
| [SharkTrustX](examples/SharkTrustX/readme.html) | A C web server that obtains and renews its HTTPS certificate. Try Let's Encrypt staging or connect it to your own SharkCA portal. |
| [Mako Server](examples/MakoServer/) | The standalone BAS host. Use these sources to customize startup, native extensions, Windows service support, or resource packaging. See the [Mako build guide](doc/compiling.md#mako-server-hlos). |
| [Xedge](examples/xedge/) | The BAS host for firmware and real-time operating systems (RTOS). Use it to integrate storage, hardware bindings, and embedded application resources. See the [Xedge build guide](doc/compiling.md#xedge-rtos). |

The two REST examples use the same BWS server. Choose between direct C APIs and the C++ routing library according to your toolchain and application needs.

For Lua/LSP application code to run on Mako or Xedge, browse the [LSP examples](https://github.com/RealTimeLogic/LSP-Examples).

## AI-assisted development

If you use an AI coding assistant, point it to [AGENTS.md](AGENTS.md). The repository includes a [native C/C++ skill](skills/build-bas-bws-native-apps/SKILL.md) and a [Mako/Xedge Lua skill](skills/build-xedge-mako-lua-apps/SKILL.md) with API guidance, examples, and build instructions. Native Lua bindings use both.

## License

BAS and BWS Amalgamated are available under three license options:

- **[GPLv2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)**
- **Free commercial license for small companies:** https://realtimelogic.com/startuplic/
- **Standard royalty-free commercial license:** https://realtimelogic.com/contactus/license/

Commercial licensing is available for users who cannot use BAS or BWS under GPLv2.

### GPLv2 Clarification

BAS and BWS are available under [GPLv2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) or, alternatively, under a commercial license from Real Time Logic.

When BAS or BWS is used under GPLv2, the standard terms and conditions of GPLv2 apply. No additional restrictions or conditions beyond GPLv2 are imposed by this clarification.

Applications that incorporate, link with, or otherwise form a derivative work of BAS or BWS must comply with GPLv2 when distributed under the GPLv2 licensing option. This includes application code using the BAS or BWS C APIs and server-side Lua/LSP application code that uses BAS-specific APIs and executes as part of a BAS application.

Independent data or content does not become subject to GPLv2 merely because it is stored, transmitted, or served by BAS or BWS.

Users who wish to distribute BAS or BWS, or derivative works based on BAS or BWS, under terms incompatible with GPLv2 may obtain an applicable commercial license from Real Time Logic.


### Export Restrictions

This distribution includes [cryptographic software](https://realtimelogic.com/products/sharkssl/). The country in which you reside may restrict the import, possession, use, or re-export of encryption software. Before using any encryption software, check your country's laws, regulations, and policies. See http://www.wassenaar.org/ for more information.

The U.S. Government Department of Commerce, Bureau of Industry and Security (BIS), has classified this software as Export Commodity Control Number (ECCN) 5D002.C.1. This includes information security software using or performing cryptographic functions with asymmetric algorithms. The form and manner of this distribution makes it eligible for export under License Exception ENC Technology Software Unrestricted (TSU), as described in BIS Export Administration Regulations, Section 740.13, for both object code and source code.

Details on the included cryptographic software are available here:

https://github.com/RealTimeLogic/SharkSSL
