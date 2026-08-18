# Fieldline device console

This Svelte application is the interactive client for the Modern C++ REST
service example. It deliberately uses a small dependency set and hand-written
CSS so its production bundle remains suitable for packaging as embedded
static content.

## Develop against the native server

Start the native example on port 9357, then run:

```text
npm install
npm run dev
```

Open `http://127.0.0.1:5173`. During development, Vite proxies `/api` to the
native server. Override its origin when the server uses another port:

PowerShell:

```powershell
$env:BAS_REST_API_ORIGIN = "http://127.0.0.1:9388"
npm run dev
```

POSIX shell:

```sh
BAS_REST_API_ORIGIN=http://127.0.0.1:9388 npm run dev
```

## Build static content

```text
npm run build
npm run preview
```

`npm run build` writes `dist/`, which is intentionally ignored. The generated
HTML, CSS, and JavaScript use relative same-origin `/api` requests. Package
the contents through the target's existing BAS/BWS static-resource mechanism,
such as an embedded ZIP/CSP image or a filesystem-backed `HttpResRdr`.

The interface follows the dark technical palette in `RTL-Colors.md`: charcoal
surfaces, restrained borders, green status/actions, and yellow secondary
emphasis. SVG icons inherit `currentColor`; no external font or image request
is required.
