# C++ WebSocket Example

**A Barracuda Embedded Web Server Example**

![Barracuda Embedded Web Server](https://realtimelogic.com/GZ/images/BarracudaWebServer.svg)

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

* The Minnow Server is designed for resource constrained devices such as a Cortex-M0 and ESP8266. A Minnow Server application must be designed as an SPA and preferably use the [SPA amalgamator service](https://realtimelogic.com/articles/Creating-SinglePage-Apps-with-the-Minnow-Server#deploy) for converting the SPA assets into a C array.
* **This example**; The [Barracuda Web Server](https://realtimelogic.com/products/barracuda-web-server/)'s [WebSocket Server](https://realtimelogic.com/ba/doc/en/C/reference/html/group__WebSockets.html) enables hybrid server-side/client-side web applications to communicate using both HTTP(S) and (secure) WebSockets. Consider using this WebSocket server if you plan on using HTTP(S) services in addition to WS(S) services and/or require a dual WSS/WS server.
* The [Barracuda App Server's WebSocket Server](https://realtimelogic.com/ba/doc/?url=SockLib.html#WebSockets) provides the easiest to use solution, in which server side business logic can be designed in the easy to learn [Lua scripting language](https://realtimelogic.com/products/lua-server-pages/). With the Barracuda App Server, much of the device's high level logic can be implemented in Lua. Just as Lua is popular with rapid game development, now embedded systems can benefit from the same. The [SharkSSL ESP32 IDE](https://realtimelogic.com/downloads/sharkssl/ESP32/) includes a ready to compile and run Barracuda App Server project called the [LSP Application Manager](https://realtimelogic.com/ba/doc/?url=examples/lspappmgr/readme.html), which can be used as the foundation for your development. See the online tutorial for an [interactive WebSocket example](https://tutorial.realtimelogic.com/).

## How to compile and run the WebSocket chat example

This example illustrates how a basic chat client can be implemented in
JavaScript and how the backend can be implemented in C++.

Use as follows:

1. [Compile the C code](#how-to-compile-the-c-code)
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

## How to compile the C code

The chat example is designed with ease-of-use in mind. On Linux, testing is made super easy with the included makefile. We also include RTOS build instructions. Simply follow the steps, and you'll be ready to go in no time.

### Linux

``` shell
git clone https://github.com/RealTimeLogic/BAS.git
cd BAS/examples/C-WebSockets/
make
./ChatServer
```

### Windows

Open the project file in BAS\examples\C-WebSockets\VcMake

### RTOS

Add the following files to your build; (xxx) is the [porting layer](../../readme.md#porting-layers):

- BAS/src/BWS.c                  -- Amalgamated [Barracuda Embedded Web Server library](https://realtimelogic.com/products/barracuda-web-server/)
- BAS/src/arch/XXX/ThreadLib.c   -- RTOS porting layer
- BAS/src/arch/NET/XXX/SoDisp.c  -- TCP porting layer
- BAS/src/DiskIo/XXX/BaFile.c    -- Optional: add if you have a file system
- BAS/examples/C-WebSockets/src/ChatServer.cpp -- The chat server example
- In your code, create a thread and have this thread call function barracuda()

#### Option 1: Using embedded file system

If you included BaFile.c in your build, copy the HTML files to your embedded file system and add the compilation flag -DBAIO_DISK

#### Option 2: Embedding the HTML files in the firmware

If you did not add BaFile or if you want to embed the HTML files in your firmware, do the following:

``` shell
cd html
zip -D -r -9 ../html.zip *
cd ..
bin2c -z getZipReader html.zip html.zip.c
```

The bin2c source code can be found in BAS/tools. You must compile this tool before running the above command.

#### All porting layers, directories to include, and compiler flags:

See [Porting Layers](../../readme.md#porting-layers)

### Ready to run RTOS build

The [SharkSSL IDE](https://realtimelogic.com/downloads/sharkssl/ESP32/) includes a ready-to-use build for this chat example. The following screenshot shows how to use the web-based IDE, compile, upload the firmware, and navigate to the chat server running on the ESP32.


![ESP32 C++ WebSocket Example](https://realtimelogic.com/images/BWSChatServerESP32.gif)


