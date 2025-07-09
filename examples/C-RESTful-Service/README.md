# Embedded RESTful Services in C and C++

This directory contains the example source code for the tutorial [Designing Embedded RESTful Services in C and C++](https://realtimelogic.com/articles/Designing-Embedded-RESTful-Services-in-C-and-C), which shows how to implement a RESTful API using the [Barracuda Embedded Web Server](https://realtimelogic.com/products/barracuda-web-server/) library.

---

## 🛠️ Building the Project

You can build the project on both **Linux** and **Windows**:

- **Linux**: Use the provided `Makefile`
- **Windows**: Use the Visual Studio project file `RestService.vcxproj`
- To **cross-compile for a custom embedded target**, refer to the Barracuda [porting API documentation](https://github.com/RealTimeLogic/BAS#porting-layers) and use the appropriate thread and socket abstraction layers.

---

## 📂 File Overview

| File Path                                  | Description                                  |
|-------------------------------------------|----------------------------------------------|
| `src/RestService.c` / `RestService.cpp`   | Main RESTful service implementation (C and C++) |
| `src/RestJsonUtils.c`                     | Utility functions for JSON parsing/encoding |
| `../HostInit/Main.c`                      | Entry point for high-level OS (HLOS) targets |
| `../HostInit/HostInit.c`                  | Generic system initialization for HLOS      |
| `../../src/BWS.c`                         | Barracuda Embedded Web Server core library  |
| `../../src/arch/Posix/ThreadLib.c`        | POSIX thread abstraction layer              |
| `../../src/arch/NET/generic/SoDisp.c`     | Generic socket dispatcher (TCP layer)       |

## Testing the Service

A Python script is included to test the RESTful API implementation. Before running the script, ensure that the server is listening on **port 80** (on Linux run: sudo ./restservice).

To run the test:

```bash
python TestApi.py
```

