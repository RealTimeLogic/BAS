# Modern C++ REST service example

This example uses the
[Modern C++ REST library](../../cpp/rest/README.md) to provide concise
routing, typed parameters, and structured error handling. It is a more
advanced alternative to the
[direct C REST example](../C-RESTful-Service/README.md).

## Choosing between the REST examples

| Example | Use it when | Requirements and tradeoffs |
| --- | --- | --- |
| [C RESTful Service](../C-RESTful-Service/README.md) | The target supports only C, or code and memory overhead must be kept to an absolute minimum. | Uses the BAS/BWS C APIs directly and does not require the C++ REST library. Route parsing and error handling are implemented explicitly by the application. |
| Modern C++ REST Service (this example) | Ease of development, readable route definitions, typed parameters, and reusable REST handling are more important. | Requires a modern C++ compiler and the `cpp/rest` library. |

Both examples use the existing native BWS server, dispatcher, command thread
pool, virtual resource hierarchy, and native JSON facilities. Choose according
to the target's toolchain and resource limits rather than treating one example
as the successor to the other.

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

`RestExample.cpp` exports `barracuda()` for the existing
`examples/HostInit/Main.c` startup. It configures `HttpServerConfig` before
constructing `HttpServer`, creates three bounded command workers, opens one
plain HTTP listener, and compiles `src/BWS.c` rather than `src/BAS.c` because
the example needs no Lua/LSP facilities.

## Build composition

Compile these C sources using the target's C compiler:

```text
src/BWS.c
src/arch/<OS>/ThreadLib.c
src/arch/NET/<NET>/SoDisp.c
examples/HostInit/Main.c       # HLOS example only
examples/HostInit/HostInit.c   # HLOS example only
```

Compile these as C++11 or newer:

```text
cpp/rest/src/Rest.cpp
examples/Modern-Cpp-REST-Service/src/RestExample.cpp
```

Include `cpp/rest/inc`, `inc`, and the selected OS/network include
directories. Link with the C++ linker and the platform socket/thread
libraries. See the native skill's
[build and porting guide](../../skills/build-bas-bws-native-apps/references/build-porting.md)
for the porting-layer model. An RTOS integration should replace HostInit and
the Windows/POSIX listener startup with its normal network
initialization and BAS/BWS task.

The default HTTP port is 9357. Define `HTTP_PORT` at compile time to select a
different non-privileged test port.

Each route writes a concise `HttpTrace` entry containing the method, API path,
operation details, and intended HTTP status. The `/api/inspect` handler uses
the locked trace writer so its length-delimited query, header, cookie, and
authenticated-user values can be printed directly in one complete line.
Request body contents are summarized by byte count and checksum.

## 🛠️ Building the Project

> Note that both the POSIX and Visual C++ projects use a **precompiled Svelte HTML user interface** (`src/EmbeddedUi.c`) by default as an easy way to test the REST service. This makes it easy to build the complete test program without requiring npm. See the [Svelte Device UI](#svelte-device-ui) for details.



### POSIX/Linux Makefile

The Makefile follows the same composition as
[examples/C-RESTful-Service](../C-RESTful-Service/README.md), while
compiling the REST layer and application as C++11 and using `g++` for
the final link. It provides two independent server builds and one
maintainer-only UI regeneration target:

| Command | Includes Embedded Device UI | Requires npm |
| --- | --- | --- |
| `make` or `make server` | No | No |
| `make release` | Yes, from committed `src/EmbeddedUi.c` | No |
| `make regenerate-embedded-ui` | Regenerates committed `src/EmbeddedUi.c`; does not build the server | Yes |

Use the server-only build while developing the Svelte client:

```sh
cd examples/Modern-Cpp-REST-Service
make server
./restservice
```

This build does not define `BAS_REST_EMBEDDED_UI`, does not include
`src/EmbeddedUi.c`, and does not invoke npm, ZIP, or [bin2c](https://realtimelogic.com/downloads/bin2c/). It
defaults to an optimized build with exceptions and RTTI disabled. The
executable is written to the example directory as `restservice`;
intermediate objects remain below `build/posix`. Run `make clean` to
remove the executable and intermediate files. Set `BUILD_DIR` to move
the intermediate files, for example:

```sh
make BUILD_DIR=/tmp/modern-rest-service
```

### Visual C++ project

Open `RestService.vcxproj` in Visual Studio and select `Debug|x64` or
`Release|x64`. The project adds `cpp/rest/src/Rest.cpp`, and uses
`cpp/rest/inc` for the public header. It also disables exceptions and
RTTI. Output is written below `build/x64/<Configuration>`

The configurations intentionally support the same two workflows as the Linux
Makefile:

- `Debug|x64` does not define `BAS_REST_EMBEDDED_UI`. Run the Svelte client
  with `npm run dev`; Vite serves the UI and proxies `/api` to this server.
- `Release|x64` defines `BAS_REST_EMBEDDED_UI` and uses the committed
  `src/EmbeddedUi.c`. It serves the prebuilt UI directly at `/` and does not
  require npm on the Windows build machine.

A Windows developer may also test the prebuilt UI with the Debug
configuration. Add the macro `BAS_REST_EMBEDDED_UI` to **Configuration
Properties > C/C++ > Preprocessor > Preprocessor Definitions** for
`Debug|x64`, then rebuild. No source or project-file change is
needed. The macro activates the generated payload in
`src/EmbeddedUi.c` and the corresponding
[VFS](https://realtimelogic.com/ba/doc/en/VirtualFileSystem.html)
mounting code in `RestExample.cpp`. Without the macro, `EmbeddedUi.c`
compiles as an empty translation unit and contributes no UI data to
the Debug executable.  Remove the macro to return to the Vite
development workflow.

The project can also be built from a Visual Studio developer command prompt:

```text
msbuild RestService.vcxproj /p:Configuration=Release /p:Platform=x64
```

## Svelte Device UI

The `client` directory contains a lightweight Svelte single-page
application. It uses all eight public example routes for live health,
device verification, GPIO control, request inspection, streaming,
payload checksums, and the concurrency test.  It has no
component-library or runtime service dependency beyond Svelte.

Start the native server on its default port, then run the development client:

```text
cd examples/Modern-Cpp-REST-Service/client
npm install
npm run dev
```

Open `http://127.0.0.1:5173`. Vite proxies relative `/api` requests to
`http://127.0.0.1:9357`, so the browser does not require a CORS exception. To
use another native-server port, set `BAS_REST_API_ORIGIN` before starting
Vite, for example `http://127.0.0.1:9388`.

Run `npm run build` to create deployable static files in
`client/dist`. The client uses same-origin `/api` URLs in a production
build.

### Committed embedded UI

`src/EmbeddedUi.c` contains a ZIP of a previously built production UI and is
committed to Git. This lets Linux, Windows, and embedded builds compile the
standalone device console using only their native C/C++ toolchain.

The normal release build consumes this committed file; it never regenerates
it:

```sh
make release
./restservice-release
```

The release compilation defines `BAS_REST_EMBEDDED_UI` and compiles
`src/EmbeddedUi.c` as C. The startup code obtains its generated [ZipReader](https://realtimelogic.com/ba/doc/en/C/reference/html/structZipReader.html)
and mounts the ZIP through persistent [ZipIo](https://realtimelogic.com/ba/doc/en/C/reference/html/structZipIo.html) and [HttpResRdr](https://realtimelogic.com/ba/doc/en/C/reference/html/structHttpResRdr.html) objects. The
device console is served at `/`, while the REST service remains at `/api`.

Only a maintainer changing the Svelte application needs to regenerate the
committed file:

```sh
make regenerate-embedded-ui
git diff -- src/EmbeddedUi.c
```

That explicit command:

1. runs the Vite production build, producing `client/dist`;
2. creates `build/posix-release/device-console.zip` with `index.html` and
   `assets/...` at the ZIP root;
3. compiles `../../tools/bin2c.c` as the host utility
   `build/posix-release/bin2c`;
4. runs `bin2c -z getEmbeddedUiZipReader` and wraps its output in
   `#ifdef BAS_REST_EMBEDDED_UI`;
5. updates `src/EmbeddedUi.c` only when the generated content differs.

Regeneration requires Node.js, npm, `zip`, and a host
C compiler. These tools are maintainer dependencies, not release-build
dependencies. For example, an `nvm` user can select a compatible Linux version
before regenerating:

```sh
nvm install 22
nvm use 22
make regenerate-embedded-ui
```
