# C++ WebSocket Example

This WebSocket example shows how to use the
[C WebSocket Server](https://realtimelogic.com/ba/doc/en/C/reference/html/group__WebSockets.html)
included with the
[Barracuda Embedded Web Server](https://realtimelogic.com/products/barracuda-web-server/)
and the
[Barracuda App Server](https://realtimelogic.com/products/barracuda-application-server/). The
example is designed for HLOS, but can easily be ported to a monolithic
RTOS system. The example includes a Makefile for Linux and a Visual
Studio project file for Windows. The makefile and project file are using the [amalgamated Barracuda Embedded Web Server C source code library](https://raw.githubusercontent.com/RealTimeLogic/BAS/main/src/BWS.c); however, you can also link the example code with the [amalgamated Barracuda App Server C source code library](https://raw.githubusercontent.com/RealTimeLogic/BAS/main/src/BAS.c).

The Barracuda Web & App Server's C WebSocket server is one of three
WebSocket server implementations provided by Real Time Logic.

* The Minnow Server is designed for resource constrained devices such as a Cortex-M0 and ESP8266. A Minnow Server application must be designed as an SPA and preferably use the [SPA amalgamator service](https://realtimelogic.com/articles/Creating-SinglePage-Apps-with-the-Minnow-Server#deploy) for converting the SPA assets into a C array. The [SharkSSL ESP8266 IDE](https://realtimelogic.com/downloads/sharkssl/ESP8266/) and the [SharkSSL ESP32 IDE](https://realtimelogic.com/downloads/sharkssl/ESP32/) include a ready to compile and run Minnow Server example.
* The [Barracuda Web Server](https://realtimelogic.com/products/barracuda-web-server/)'s [WebSocket Server](https://realtimelogic.com/ba/doc/en/C/reference/html/group__WebSockets.html) enables hybrid server-side/client-side web applications to communicate using both HTTP(S) and (secure) WebSockets. The [SharkSSL ESP32 IDE](https://realtimelogic.com/downloads/sharkssl/ESP32/) includes a ready to compile and run Barracuda Web Server WebSocket example. Consider using this WebSocket server if you plan on using HTTP(S) services in addition to WS(S) services and/or require a dual WSS/WS server.
* The [Barracuda App Server's WebSocket Server](https://realtimelogic.com/ba/doc/?url=SockLib.html#WebSockets) provides the easiest to use solution, in which server side business logic can be designed in the easy to learn [Lua scripting language](https://realtimelogic.com/products/lua-server-pages/). With the Barracuda App Server, much of the device's high level logic can be implemented in Lua. Just as Lua is popular with rapid game development, now embedded systems can benefit from the same. The [SharkSSL ESP32 IDE](https://realtimelogic.com/downloads/sharkssl/ESP32/) includes a ready to compile and run Barracuda App Server project called the [LSP Application Manager](https://realtimelogic.com/ba/doc/?url=examples/lspappmgr/readme.html), which can be used as the foundation for your development. See the online tutorial for an [interactive WebSocket example](https://tutorial.realtimelogic.com/).

## Howto

This example illustrates how a basic chat client can be implemented in
JavaScript and how the backend can be implemented in C++.

Use as follows:

1. Compile the C code
2. Start the server
3. Open at least two browser windows and navigate to http(s)://localhost:portno
4. Enter text in any Window (text input dialog at bottom of screen)
5. Drag and drop JPEG images into any of the browser windows

The drag and drop feature illustrates how to use binary
frames. Dropping a JPEG image (max size 0xFFFF) onto the chat client
running in the browser makes the JavaScript code send a binary frame
to the server, which is then distributed to all connected clients
where the image is shown. The images dropped into the browser window
must be JPEG, and the image sizes must be less than 65,535 bytes.



