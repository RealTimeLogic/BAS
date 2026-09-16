# BWS and BAS - [Embedded Web Servers](https://realtimelogic.com/articles/What-Is-an-Embedded-Web-Server)

This repository contains the amalgamated C source code for Real Time Logic's embedded web and application server libraries:

- **BWS:** [Barracuda Embedded Web Server](https://realtimelogic.com/products/barracuda-web-server/), a compact HTTP(S) and WebSocket C library integrated with the SharkSSL TLS stack.
- **BAS:** [Barracuda App Server](https://realtimelogic.com/products/barracuda-application-server/), an application server and IoT toolkit built on BWS. BAS adds Lua, [Lua Server Pages](https://realtimelogic.com/products/lua-server-pages/), and higher-level APIs for rapid embedded web and IoT development.

BWS and BAS share the same porting layer model and support a broad range of targets, including embedded Linux, Zephyr, FreeRTOS, lwIP, VxWorks, QNX, INTEGRITY, Windows, Azure RTOS, Nucleus RTOS, embOS, Mac, and other POSIX systems.

## AI-Assisted Development

The [BAS/BWS Agent Guide](AGENTS.md) routes humans and AI assistants to the included skills and examples. Use the [native C/C++ skill](skills/build-bas-bws-native-apps/SKILL.md) for BAS/BWS applications, REST services, WebSockets, VFS layouts, generated resources, uploads, WebDAV, and native integration. Use the [Mako/Xedge Lua skill](skills/build-xedge-mako-lua-apps/SKILL.md) for Lua and Lua Server Pages applications, including application lifecycle, routing, browser interfaces, authentication, SMQ, SQLite, deployment, and Lua bindings. Use both skills when a native Mako/Xedge host exposes or extends Lua APIs.

## Examples

Choose the example that matches where you want to implement the application logic. The first three examples use native C or C++. Mako Server and Xedge provide native hosts for Lua and Lua Server Pages (LSP) applications.

| Example | Start here when | Approach and requirements |
| --- | --- | --- |
| [C-RESTful-Service](examples/C-RESTful-Service/README.md) | You need a native REST service for a C-only target or want to minimize code and memory overhead. | Uses the BAS/BWS C APIs directly, with routing and error handling implemented by the application. It does not require `cpp/rest`. |
| [Modern-Cpp-REST-Service](examples/Modern-Cpp-REST-Service/README.md) | You want concise route definitions, typed parameters, structured errors, streaming, reusable REST handling, or the included Svelte device UI. | Uses the same native BWS foundation as the C REST example. Requires C++11 or newer and the `cpp/rest` library. |
| [C-WebSockets](examples/C-WebSockets/README.md) | You need a native C++ WebSocket service with text and binary frames, multiple clients, concurrent broadcasts, or embedded browser resources. | Uses the native BAS/BWS WebSocket API with disk-based or embedded web content. For WebSocket behavior implemented in Lua, use the Mako/Xedge Lua skill. |
| [MakoServer](examples/MakoServer/) | You want to build or customize the standalone Mako executable, platform startup, native extensions, services, or resource packaging. | Provides the native host for high-level operating systems. For ordinary Lua/LSP applications, use the [Mako/Xedge Lua skill](skills/build-xedge-mako-lua-apps/SKILL.md). See [Mako Server compilation](doc/compiling.md#mako-server-hlos). |
| [xedge](examples/xedge/) | You want to port or embed Xedge in firmware or an RTOS and integrate storage, hardware hooks, embedded resources, or native extensions. | Provides the native Xedge host for RTOS and firmware targets. Use the [Mako/Xedge Lua skill](skills/build-xedge-mako-lua-apps/SKILL.md) for `.preload`, `.config`, `.xlua`, and LSP application development. See [Xedge compilation](doc/compiling.md#xedge-rtos). |

### Next Steps

- To start with **BAS**, choose your operating system on the [BAS download page](https://realtimelogic.com/downloads/bas/). The page provides instructions, precompiled packages, and platform-specific build options for supported operating systems.
- To build or customize the native **Mako Server** or integrate **Xedge** into an RTOS/firmware target, see [Compiling Mako Server and Xedge](doc/compiling.md).


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
