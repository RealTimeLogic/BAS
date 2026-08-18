---
name: build-lsp-web-interfaces
description: Select and implement an LSP browser-interface architecture using server-rendered pages and forms, htmx HTML fragments, Fetch/JSON, request-response RPC over WebSockets, or SMQ-driven real-time messaging. Use when building or refactoring Mako, Xedge, Xedge32, or Barracuda App Server dashboards, device interfaces, control panels, forms, partial-page navigation, browser RPC, or live multi-client UIs.
---

# Build LSP Web Interfaces

Choose the least complex interaction model that satisfies the browser workflow, then keep each endpoint's contract explicit.

## Standalone Use And Local Cache

This skill may be downloaded without the repository `AGENTS.md`. Resolve referenced public BAS documents and skills in this order:

1. A same-directory file with the referenced filename.
2. `./.agents/reference/rtl/` relative to the project.
3. The public source URL.

If a referenced file is missing and network access is available, download it from the public URL and save a local copy before using it. Record the source URL and download date in `./.agents/reference/rtl/manifest.md` or in a short header in the cached file.

## Source Of Truth

Use the following references:

- `basapi.md`: https://realtimelogic.com/downloads/basapi.md
- `VFS-skill.md`: https://realtimelogic.com/downloads/ai-skills/VFS-skill.md
- `Authentication-Authorization-Skill.md`: https://realtimelogic.com/downloads/ai-skills/Authentication-Authorization-Skill.md
- `OWASP-General-Security-Skill.md`: https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md
- `SMQ-Skill.md`: https://realtimelogic.com/downloads/ai-skills/SMQ-Skill.md

Use `basapi.md` for exact request, response, WebSocket, JSON, and LSP APIs. Use the VFS skill for routing, the authentication/OWASP skills for exposed inputs, and the SMQ skill for publish/subscribe or synchronized device/browser state.

## Choose The Interaction Model

| Need | Start with | Server returns |
| --- | --- | --- |
| Navigation, settings, simple forms | Full-page LSP | Complete HTML |
| User-triggered partial update | htmx + LSP | HTML fragment |
| Structured data for browser logic or other clients | Fetch | JSON |
| Many correlated calls plus pushed events on one connection | WebSocket RPC | Correlated messages |
| Topics, presence, devices, multiple synchronized clients | SMQ | Typed topic messages |

Use this escalation order:

1. Use full-page LSP and ordinary forms for navigation, configuration, and low-frequency actions.
2. Add htmx when user actions should replace HTML fragments without a full-page reload.
3. Use Fetch/JSON when browser code needs structured data reused across multiple views or non-HTML clients.
4. Use request-response RPC over one WebSocket when many correlated calls share a persistent connection and asynchronous server messages must use that same connection.
5. Use SMQ when topics, presence, discovery, one-to-one messages, device/browser interoperability, or multiple synchronized clients are central.

Do not choose WebSockets or SMQ merely to avoid a page reload. Do not build a JSON API when the only consumer needs an HTML fragment.

## Define The Contract Before Coding

For every interaction, specify:

- initiating event and expected latency;
- request method, URL or message name;
- input fields and validation;
- response format: full HTML, fragment HTML, JSON, binary, or event;
- success, validation-error, authorization-error, and transport-error behavior;
- idempotency and retry behavior;
- connection/reconnect behavior for persistent transports;
- whether multiple open browser instances must remain synchronized.

## Use Full-Page LSP

Use LSP to combine a template with page-specific content. Keep the application usable through normal URLs and form submissions. Prefer this model when actions are infrequent, direct links matter, JavaScript may be disabled, or a persistent connection offers no material benefit.

Keep device logic, authorization, input validation, and state changes in server-side Lua/LSP.

## Add Htmx

Use htmx for browser-initiated interactions such as button clicks, filters, navigation, and form submissions that should update one region.

```html
<button hx-get="/status.lsp" hx-target="#status">Refresh</button>
<section id="status"></section>
```

Make `/status.lsp` return the smallest stable fragment. When one route must support both direct and fragment navigation, branch on the htmx request header:

```lua
if request:header("hx-request") then
   -- render page fragment
else
   -- render complete template plus fragment
end
```

Preserve ordinary links and forms where practical so the interface retains a coherent full-page path.

Htmx is strongest for client-initiated request/response flows. Polling or an htmx WebSocket extension can cover simple live views, but backend-driven synchronized state is a signal to evaluate a purpose-built WebSocket or SMQ design.

## Use Fetch And JSON

Use Fetch when browser JavaScript owns a view model, when several widgets reuse the same data, or when the API has non-browser consumers.

- Use JSON only when structured data is the actual contract.
- Check HTTP status before decoding.
- Validate `Content-Type`, request size, fields, and methods on the server.
- Render untrusted values with safe DOM/text APIs rather than HTML concatenation.

Avoid converting an HTML-fragment interaction into JSON followed by client-side HTML reconstruction unless that separation has a concrete consumer or maintainability benefit.

## Use Request-Response WebSocket RPC

WebSockets carry messages; they do not automatically match a response to its request. Multiplex correlated calls with a small envelope:

```json
{"rpcID":123,"service":"math/add","args":[2,3]}
{"rpcID":123,"rsp":5,"err":null}
```

The browser must:

1. allocate a unique `rpcID`;
2. register a Promise/callback and timeout;
3. send the request;
4. match the response by `rpcID`;
5. settle and delete the pending entry;
6. reject remaining entries when the socket closes.

Reserve a distinct message shape for unsolicited events. Bound message sizes and outstanding requests. Add authorization, schema validation, timeouts, and backpressure.

## Use SMQ

Use the dedicated SMQ skill rather than inventing a topic protocol when the application needs publish/subscribe, presence, discovery, one-to-one messages, browser/device peers, or synchronized state across clients.

## Manage Persistent Connections

A full-page navigation normally tears down browser connections. A stable outer template can keep one connection alive while htmx swaps inner fragments.

Choose one owner for each connection. If a fragment creates a page-local socket, close it on the appropriate htmx pre-swap event. Otherwise revisiting the fragment can accumulate connections and duplicate handlers.

## Verify The User Workflow

Test:

- initial full-page load and direct navigation to deep URLs;
- ordinary form fallback where supported;
- fragment targeting and history behavior;
- validation and authorization failures;
- refresh, back/forward navigation, and duplicate submission;
- disconnect, reconnect, timeout, and stale-message behavior;
- two simultaneous browser instances when state synchronization matters;
- trace output and browser-console errors.

Do not treat a successful LSP HTTP response as proof that a WebSocket or SMQ handshake works; exercise the intended browser client.

## Avoid Unsafe Tutorial Patterns

- Do not use "JavaScript ran" or an encrypted hidden field as proof that a human submitted a form.
- Do not store server secrets in browser code or HTML.
- Do not trust client-side validation.
- Do not expose raw Lua errors or stack traces to clients.
- Route authentication, CSRF, upload, CORS, and public-form decisions through the dedicated security references.

Report the selected interaction model, endpoint/message contracts, files changed, fallback behavior, security controls, and verification performed. Explain why a more complex transport was or was not needed.

## Tutorial Provenance

Distilled from the tutorial bundles downloaded on 2026-07-25:

- BAS bundle: `LSP-htmx-A-Powerful-Duo-for-Embedded-Web-Apps`
- BAS bundle: `Designing-RESTful-Services-in-Lua`
- Mako bundle: `How-to-Build-an-Interactive-Dashboard-App`
- Mako bundle: `AJAX-over-WebSockets`
- Mako bundle: `Ajax-for-Beginners`
- Mako bundle: `HTML-Forms-and-LSP-for-Beginners`

Exact BAS APIs are intentionally not duplicated from the API reference.
