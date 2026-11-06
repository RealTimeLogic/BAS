# Device console for the C++ REST example

This Svelte application lets you try the [native REST service](../README.md)
from a browser. It displays server health, exercises simulated device and
GPIO operations, and tests request inspection, streaming, and concurrency.

To use the prebuilt console, follow the parent README's release-build
instructions. The steps below are for editing the interface. They require
Node.js 20.19 or newer in the 20.x series, or Node.js 22.12 or newer, plus npm.

## Develop against the native server

Start the native example on port 9357. In a second terminal, run these
commands from this `client` directory:

```sh
# Install dependencies and start Vite's development server.
npm install
npm run dev
```

Open [http://127.0.0.1:5173](http://127.0.0.1:5173). Vite forwards `/api`
requests to the native server. If the server uses another address or port,
set the optional URL string `BAS_REST_API_ORIGIN` before starting Vite.
Its default is `http://127.0.0.1:9357`.

PowerShell:

```powershell
# Match the port used by your native server.
$env:BAS_REST_API_ORIGIN = "http://127.0.0.1:9388"
npm run dev
```

POSIX shell:

```sh
# Match the port used by your native server.
BAS_REST_API_ORIGIN=http://127.0.0.1:9388 npm run dev
```

## Build static content

```sh
# Build the browser assets, then preview the generated files locally.
npm run build
npm run preview
```

The build writes HTML, CSS, and JavaScript to `dist/`, which is excluded
from Git. API requests use `/api` on the same origin as the page. The
preview serves the assets; use the native server for a deployed console.

To include your changes in the native executable, follow
[Committed embedded UI](../README.md#committed-embedded-ui). For another
BAS/BWS application, serve the assets from an embedded ZIP or a filesystem
through `HttpResRdr`.

The interface uses local CSS and SVG icons, with no external font or image
requests. Its charcoal background, green controls, and yellow highlights
keep status and actions visible on the device console.
