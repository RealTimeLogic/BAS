# Embedded REST services in C and C++

This example implements a small user-management API with the [Barracuda Embedded Web Server (BWS)](https://realtimelogic.com/products/barracuda-web-server/). It shows how to route requests, parse JSON, and send responses using the native C APIs. The accompanying tutorial, [Designing Embedded RESTful Services in C and C++](https://realtimelogic.com/articles/Designing-Embedded-RESTful-Services-in-C-and-C), explains the code.

Use this example with a C-only toolchain or when you want direct control over request handling. If you prefer typed route parameters and reusable response helpers, try the [Modern C++ REST Service](../Modern-Cpp-REST-Service/README.md), which requires C++11 and the included `cpp/rest` library. Both use the same BWS server and JSON APIs.

## Build and run

On Linux, run from the repository root:

```sh
# Build the C implementation with the supplied Makefile.
cd examples/C-RESTful-Service
make
./restservice
```

On Windows, open `RestService.vcxproj` in Visual Studio and build the project.

Check the startup output for the listening port. The test script expects port 80. On Linux, `sudo ./restservice` allows the example to bind to that port; if you run it on another port, update `BASE_URL` in `TestApi.py`.

For an embedded target, select the appropriate [thread and network ports](../../doc/compiling.md#porting-layers) and replace the shared host startup with your device's startup code.

## Try the API

Keep the server running and open another terminal in this example directory. The test script needs Python and the `requests` package:

```sh
# Install the test dependency in your Python environment.
python -m pip install requests
python TestApi.py
```

The script creates, fetches, and deletes users, and prints the responses to invalid requests and a full user table. It displays results for inspection; it does not assert that each response is correct.

## Find your way around the code

| File | Purpose |
| --- | --- |
| `src/RestService.c` / `src/RestService.cpp` | C and C++ versions of the service. |
| `src/RestJsonUtils.c` | JSON parsing and encoding helpers. |
| `../HostInit/Main.c` | Entry point for desktop and server operating systems. |
| `../HostInit/HostInit.c` | Shared host initialization. |
| `../../src/BWS.c` | Barracuda Embedded Web Server library. |
| `../../src/arch/Posix/ThreadLib.c` | POSIX thread port. |
| `../../src/arch/NET/generic/SoDisp.c` | Socket dispatcher. |
