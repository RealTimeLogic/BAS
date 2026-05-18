# BWS and BAS - [Embedded Web Servers](https://realtimelogic.com/articles/What-Is-an-Embedded-Web-Server)

This repository contains the amalgamated C source code for Real Time Logic's embedded web and application server libraries:

- **BWS:** [Barracuda Embedded Web Server](https://realtimelogic.com/products/barracuda-web-server/), a compact HTTP(S) and WebSocket C library integrated with the SharkSSL TLS stack.
- **BAS:** [Barracuda App Server](https://realtimelogic.com/products/barracuda-application-server/), an application server and IoT toolkit built on BWS. BAS adds Lua, [Lua Server Pages](https://realtimelogic.com/products/lua-server-pages/), and higher-level APIs for rapid embedded web and IoT development.

BWS and BAS share the same porting layer model and support a broad range of targets, including embedded Linux, Zephyr, FreeRTOS, lwIP, VxWorks, QNX, INTEGRITY, Windows, Azure RTOS, Nucleus RTOS, embOS, Mac, and other POSIX systems.

## Getting Started

- If you plan to use **BAS**, start here and select your operating system: https://realtimelogic.com/downloads/bas/. This page includes a drop-down menu that opens purpose-built instructions for selected operating systems, including precompiled packages and platform-specific build options.
- If your operating system is not listed there, if you need the generic source integration details, or if you plan to use **BWS**, see the [generic compilation instructions](compiling.md).

## License

BAS and BWS Amalgamated are available under three license options:

- **[GPLv2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)**
- **Free commercial license for small companies:** https://realtimelogic.com/startuplic/
- **Standard royalty-free commercial license:** https://realtimelogic.com/contactus/license/

Commercial licensing is available for users who cannot use BAS or BWS under GPLv2.

### GPLv2 Clarification

The following clarification applies to users who choose GPLv2 and distribute derivative work. The GPLv2 license applies to all BAS and BWS APIs, including but not limited to the BAS and BWS C APIs, BAS Lua APIs, and web content hosted by a BAS or BWS derivative product. Web content hosted by the server is considered part of the application as a whole.

GPLv2: https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html

### Export Restrictions

This distribution includes [cryptographic software](https://realtimelogic.com/products/sharkssl/). The country in which you reside may restrict the import, possession, use, or re-export of encryption software. Before using any encryption software, check your country's laws, regulations, and policies. See http://www.wassenaar.org/ for more information.

The U.S. Government Department of Commerce, Bureau of Industry and Security (BIS), has classified this software as Export Commodity Control Number (ECCN) 5D002.C.1. This includes information security software using or performing cryptographic functions with asymmetric algorithms. The form and manner of this distribution makes it eligible for export under License Exception ENC Technology Software Unrestricted (TSU), as described in BIS Export Administration Regulations, Section 740.13, for both object code and source code.

Details on the included cryptographic software are available here:

https://github.com/RealTimeLogic/SharkSSL
