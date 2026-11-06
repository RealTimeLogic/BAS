# Modern C++ REST service example

Build a device API and try it from a browser. This example uses the
[C++ REST library](../../cpp/rest/README.md) for typed route parameters,
request-body handling, and responses. It includes a Svelte console for
exercising the API, with simulated device operations you can replace with
your own code.

You need a C++11 compiler. For a C-only toolchain or direct control over
routing, use the [C REST example](../C-RESTful-Service/README.md). Both
examples use the Barracuda Embedded Web Server (BWS) and its JSON APIs.

## Build and run

To try the complete example on Linux, run the following from the repository
root. The release build includes a prebuilt browser interface, so you don't
need Node.js or npm:

```sh
# Build the API server with the included device console.
cd examples/Modern-Cpp-REST-Service
make release
./restservice-release
```

Open [http://127.0.0.1:9357](http://127.0.0.1:9357). On Windows, build
`Release|x64` in `RestService.vcxproj`, then run the executable from
`build/x64/Release` and open the same address.

### POSIX/Linux Makefile

The Makefile compiles the library and application as C++11 and links with
`g++`. Choose a server-only build for client development, or a release build
with the browser interface included:

| Command | Includes Embedded Device UI | Requires npm |
| --- | --- | --- |
| `make` or `make server` | No | No |
| `make release` | Yes, from committed `src/EmbeddedUi.c` | No |
| `make regenerate-embedded-ui` | Regenerates committed `src/EmbeddedUi.c`; does not build the server | Yes |

Use the server-only build while developing the Svelte client:

```sh
# Run from this example directory.
make server
./restservice
```

The server-only build omits `src/EmbeddedUi.c`. It enables optimization
and disables exceptions and run-time type information (RTTI). The
executable is written to the example directory as `restservice`;
intermediate objects remain below `build/posix`. Run `make clean` to
remove the executable and intermediate files. Set `BUILD_DIR` to move
the intermediate files, for example:

```sh
# Keep object files outside the source directory.
make BUILD_DIR=/tmp/modern-rest-service
```

### Visual C++ project

Open `RestService.vcxproj` in Visual Studio and select `Debug|x64` or
`Release|x64`. The project adds `cpp/rest/src/Rest.cpp`, and uses
`cpp/rest/inc` for the public header. It also disables exceptions and
RTTI. Output is written below `build/x64/<Configuration>`.

The Windows configurations match the two Linux workflows:

- `Debug|x64` does not define `BAS_REST_EMBEDDED_UI`. Run the Svelte client
  with `npm run dev`; Vite serves the UI and proxies `/api` to this server.
- `Release|x64` defines `BAS_REST_EMBEDDED_UI` and uses the committed
  `src/EmbeddedUi.c`. It serves the prebuilt UI directly at `/` and does not
  require npm on the Windows build machine.

To test the prebuilt UI in a Debug build, enable it in the project settings. Add the macro `BAS_REST_EMBEDDED_UI` to **Configuration
Properties > C/C++ > Preprocessor > Preprocessor Definitions** for
`Debug|x64`, then rebuild. The macro activates the generated payload in
`src/EmbeddedUi.c` and the corresponding
[Virtual File System (VFS)](https://realtimelogic.com/ba/doc/en/VirtualFileSystem.html)
mounting code in `RestExample.cpp`. Without the macro, `EmbeddedUi.c`
compiles as an empty translation unit and contributes no UI data to
the Debug executable.  Remove the macro to return to the Vite
development workflow.

The project can also be built from a Visual Studio developer command prompt:

```bat
rem Run from this example directory in a Visual Studio Developer Command Prompt.
msbuild RestService.vcxproj /p:Configuration=Release /p:Platform=x64
```

## Svelte Device UI

The `client` directory contains the Svelte application. Use it to check
server health, try simulated device and GPIO operations, inspect requests,
and test streaming, payload checksums, and concurrent requests.

To change the interface, install Node.js and npm, start the native server,
and run the development client from the repository root. See the
[client README](client/README.md) for Node.js requirements.

```sh
# Start the client development server in a second terminal.
cd examples/Modern-Cpp-REST-Service/client
npm install
npm run dev
```

Open `http://127.0.0.1:5173`. Vite proxies relative `/api` requests to
`http://127.0.0.1:9357`, so both the page and API requests use the same origin. To
use another native-server port, set `BAS_REST_API_ORIGIN` before starting
Vite (default: `http://127.0.0.1:9357`), for example `http://127.0.0.1:9388`.

Run `npm run build` to create deployable static files in
`client/dist`. The client uses same-origin `/api` URLs in a production
build.

### Committed embedded UI

`src/EmbeddedUi.c` contains a ZIP of a previously built production UI and is
committed to Git. This lets Linux, Windows, and embedded builds compile the
standalone device console using only their native C/C++ toolchain.

The `make release` command uses this file as it stands. It does not rebuild
the Svelte application.

The release compilation defines `BAS_REST_EMBEDDED_UI` and compiles
`src/EmbeddedUi.c` as C. The startup code obtains its generated [ZipReader](https://realtimelogic.com/ba/doc/en/C/reference/html/structZipReader.html)
and mounts the ZIP through persistent [ZipIo](https://realtimelogic.com/ba/doc/en/C/reference/html/structZipIo.html) and [HttpResRdr](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpResRdr.html) objects. The
device console is served at `/`, while the REST service remains at `/api`.

After changing the Svelte application, regenerate the embedded copy from
this example directory:

```sh
# Update the embedded UI and inspect the generated change.
make regenerate-embedded-ui
git diff -- src/EmbeddedUi.c
```

The regeneration command:

1. runs the Vite production build, producing `client/dist`;
2. creates `build/posix-release/device-console.zip` with `index.html` and
   `assets/...` at the ZIP root;
3. compiles `../../tools/bin2c.c` as the host utility
   `build/posix-release/bin2c`;
4. runs `bin2c -z getEmbeddedUiZipReader` and wraps its output in
   `#ifdef BAS_REST_EMBEDDED_UI`;
5. updates `src/EmbeddedUi.c` only when the generated content differs.

Regeneration requires Node.js, npm, `zip`, and a host
C compiler. For example, if you use `nvm`, select a compatible Node.js version
before regenerating:

```sh
# Select Node.js for rebuilding the Svelte assets.
nvm install 22
nvm use 22
make regenerate-embedded-ui
```

## Example routes

The service is mounted at `/api` and includes:

- `GET /api/health`: small JSON response;
- `POST /api/gpio/{pin}/set/{state}`: signed/unsigned typed parameters and a
  simulated device operation inside `withServerUnlocked()`;
- `GET /api/inspect?mode=...`: query, header, cookie, and authenticated-user
  access;
- `GET /api/slow/{milliseconds}`: nested scoped unlock and concurrent-worker
  behavior;
- `GET /api/stream`: incremental response output and flush;
- `POST /api/body`: bounded streaming body consumption with `RestBodyReader`;
- `GET /api/device/status` and `GET /api/device/{id}`: static-over-typed route
  precedence.

`RestExample.cpp` supplies the `barracuda()` entry point called by
`examples/HostInit/Main.c`. It configures the server, starts three request
workers, and opens one HTTP listener. The example compiles `src/BWS.c`.

## Integrate the example

Compile these C sources using the target's C compiler:

```text
src/BWS.c
src/arch/<OS>/ThreadLib.c
src/arch/NET/<NET>/SoDisp.c
examples/HostInit/Main.c       # Desktop/server startup only
examples/HostInit/HostInit.c   # Desktop/server startup only
```

Compile these as C++11 or newer:

```text
cpp/rest/src/Rest.cpp
examples/Modern-Cpp-REST-Service/src/RestExample.cpp
```

Include `cpp/rest/inc`, `inc`, and the selected OS/network include
directories. Link with the C++ linker and the platform socket/thread
libraries. See [Porting Layers](../../doc/compiling.md#porting-layers)
for the available ports. A real-time operating system (RTOS) integration should replace HostInit and
the Windows/POSIX listener startup with its normal network
initialization and BAS/BWS task.

The default HTTP port is 9357. Define `HTTP_PORT` at compile time to select a
different non-privileged test port.

Each route writes an `HttpTrace` entry containing the method, API path,
operation details, and intended HTTP status. The `/api/inspect` handler uses
the locked trace writer so its length-delimited query, header, cookie, and
authenticated-user values can be printed directly in one complete line.
Request body contents are summarized by byte count and checksum.
