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

1. The
   [Minnow Server](https://realtimelogic.com/products/sharkssl/minnow-server/)
   is designed for resource constrained devices such as a Cortex-M0. A
   Minnow Server application must be designed as an SPA, and
   preferably use the SPA amalgamator service for converting the SPA
   assets into a C array.

2. The Barracuda Embedded Web Server's and Barracuda App Server's C
   WebSocket Server (used in this example) enables hybrid
   server-side/client-side web applications to communicate using both
   HTTP and WebSockets.

3. The
   [Barracuda App Server's Lua WebSocket Server](https://realtimelogic.com/ba/doc/?url=SockLib.html#WebSockets)
   provides the easiest to use solution, where server side business
   logic can be designed in the easy to learn Lua scripting
   language. With the Barracuda App Server, much of the device's high
   level logic (business logic) can be implemented in Lua.
   [Lua is popular in games](https://www.makeuseof.com/why-is-lua-popular-for-game-development-how-to-use/),
   and now
   [embedded systems can take a similar approach to rapid device code design](https://realtimelogic.com/articles/Rapid-Firmware-Development-with-the-Barracuda-App-Server).
   See the online
   [Interactive Lua WebSocket Server Tutorial](https://tutorial.realtimelogic.com/WebSockets.lsp)
   for details.

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



