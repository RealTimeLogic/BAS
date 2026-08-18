---
name: bas-owasp-general-security
description: Use when designing, implementing, reviewing, or testing general OWASP-style web and application security controls in BAS, Mako Server, Xedge, Xedge32, or LSP applications, including input validation, output encoding, authentication, authorization, session handling, CSRF, REST APIs, SQL/database access, file upload, WebDAV/WFS, SMQ/WebSocket endpoints, TLS, secrets, logging, error handling, SSRF, resource limits, and secure code review.
---

# BAS OWASP General Security Skill

Use this skill when reviewing or designing general application security for BAS-derived web applications.

This skill maps OWASP Cheat Sheet Series guidance to BAS, Mako Server, Xedge, Xedge32, LSP, Lua, browser JavaScript, SMQ, WebDAV/WFS, REST, and SQLite work.

If you are new to BAS: a BAS application can combine VFS routing, server-side Lua/LSP pages, browser JavaScript, REST callbacks, SMQ/WebSocket-style messaging, WebDAV/WFS file services, and SQLite-backed state. Use this skill as the cross-cutting security review layer for those surfaces; use `basapi.md` for exact BAS API syntax.

## Standalone Use And Local Cache

This skill may be downloaded without
`https://realtimelogic.com/downloads/ai-skills/AGENTS.md`. When this file
references another public BAS document or BAS AI skill, resolve it in this order:

1. A same-directory file with the referenced filename.
2. `./.agents/reference/rtl/` relative to this file.
3. The public source URL.

If a referenced file is missing and network access is available, download it
from the public URL and save a local copy before using it. Record the source URL
and download date in `./.agents/reference/rtl/manifest.md` or in a short header
in the cached file.

Use local copies for normal work. Re-fetch only when the user asks for
current/latest guidance, the local copy is missing, or the local copy conflicts
with observed API/runtime behavior.

## Source Of Truth

Use BAS documentation for exact API syntax and OWASP for general security controls:

- BAS API bundle: https://realtimelogic.com/downloads/basapi.md
- BAS tutorials bundle: https://realtimelogic.com/downloads/tutorials.md
- Mako Server tutorials: https://makoserver.net/download/tutorials.md
- OWASP Cheat Sheet Series index: https://cheatsheetseries.owasp.org/index.html

Use these OWASP cheat sheets when relevant:

- Secure Code Review: https://cheatsheetseries.owasp.org/cheatsheets/Secure_Code_Review_Cheat_Sheet.html
- Authentication: https://cheatsheetseries.owasp.org/cheatsheets/Authentication_Cheat_Sheet.html
- Authorization: https://cheatsheetseries.owasp.org/cheatsheets/Authorization_Cheat_Sheet.html
- Session Management: https://cheatsheetseries.owasp.org/cheatsheets/Session_Management_Cheat_Sheet.html
- Password Storage: https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html
- CSRF Prevention: https://cheatsheetseries.owasp.org/cheatsheets/Cross-Site_Request_Forgery_Prevention_Cheat_Sheet.html
- Input Validation: https://cheatsheetseries.owasp.org/cheatsheets/Input_Validation_Cheat_Sheet.html
- XSS Prevention: https://cheatsheetseries.owasp.org/cheatsheets/Cross_Site_Scripting_Prevention_Cheat_Sheet.html
- SQL Injection Prevention: https://cheatsheetseries.owasp.org/cheatsheets/SQL_Injection_Prevention_Cheat_Sheet.html
- REST Security: https://cheatsheetseries.owasp.org/cheatsheets/REST_Security_Cheat_Sheet.html
- File Upload: https://cheatsheetseries.owasp.org/cheatsheets/File_Upload_Cheat_Sheet.html
- Logging: https://cheatsheetseries.owasp.org/cheatsheets/Logging_Cheat_Sheet.html
- Error Handling: https://cheatsheetseries.owasp.org/cheatsheets/Error_Handling_Cheat_Sheet.html
- Secrets Management: https://cheatsheetseries.owasp.org/cheatsheets/Secrets_Management_Cheat_Sheet.html
- Transport Layer Security: https://cheatsheetseries.owasp.org/cheatsheets/Transport_Layer_Security_Cheat_Sheet.html
- WebSocket Security: https://cheatsheetseries.owasp.org/cheatsheets/WebSocket_Security_Cheat_Sheet.html
- SSRF Prevention: https://cheatsheetseries.owasp.org/cheatsheets/Server_Side_Request_Forgery_Prevention_Cheat_Sheet.html

Reference priority:

1. Use BAS documentation for API names, object behavior, and runtime constraints.
2. Use existing project conventions and local BAS helper modules when present.
3. Use OWASP guidance for security properties, review coverage, and anti-patterns.

## Related BAS Skills

Use the focused skill when the task is primarily about that domain:

- https://realtimelogic.com/downloads/ai-skills/Authentication-Authorization-Skill.md for BAS authenticators, authorizers, sessions, login handlers, JSON users, TPM-protected user databases, WebAuthn, RADIUS, OAuth, and OpenID Connect.
- https://realtimelogic.com/downloads/ai-skills/VFS-skill.md for routing, resource readers, directory callbacks, WebDAV/WFS mounting, hidden resources, and authentication boundaries.
- https://realtimelogic.com/downloads/ai-skills/SQLite-Skill.md for SQLite write serialization, transactions, database paths, and read/write connection patterns.
- https://realtimelogic.com/downloads/ai-skills/SMQ-Skill.md for SMQ broker design, topic authorization, device authentication, and real-time command routing.

## Review Method

Trace each external input through the BAS application:

1. Identify the source: HTTP form, query string, JSON body, upload, cookie, session ID, SMQ message, WebSocket frame, WebDAV method, MQTT/OPC UA/Modbus input, file, database row, environment variable, or configuration.
2. Identify the trust boundary: browser, device, authenticated user, anonymous user, administrator, LAN client, cloud service, or local service account.
3. Identify the sink: LSP output, HTML, JavaScript, JSON, redirect URL, SQL, filesystem path, shell command, SMQ publish, WebDAV/WFS mutation, hardware control, log entry, or outbound HTTP request.
4. Verify validation, authorization, encoding, error handling, logging, and resource limits at the boundary.

Prefer explicit allowlists, small state machines, and narrow directory/API boundaries over broad catch-all handlers.

## BAS Security Principles

- Keep authority on the server. Browser JavaScript, request parameters, cookies, SMQ payloads, uploaded files, and device messages are untrusted.
- Put coarse access control at VFS directory boundaries, then enforce fine-grained authorization at the action/resource level.
- Deny by default. Unknown methods, paths, roles, message types, object IDs, file types, and protocol states should fail closed.
- Validate before doing work. Reject malformed input before database access, file I/O, outbound requests, hardware changes, or SMQ publication.
- Encode output for the exact sink. HTML, JavaScript, JSON, URL, SQL, shell, logs, and protocol messages have different rules.
- Avoid one large request handler that combines routing, validation, authorization, and business logic without clear boundaries.
- Treat mounted WebDAV/WFS, upload directories, SMQ endpoints, WebSocket endpoints, and admin tools as network-exposed attack surfaces.

## Input Validation

Validate all request data server-side, even when browser JavaScript already validates it.

- Use allowlists for methods, paths, command names, state transitions, enum values, topic names, and file extensions.
- Enforce length, size, numeric range, character set, and required-field limits before processing.
- Parse structured input with JSON/form parsers instead of ad hoc string splitting.
- Normalize and validate filesystem paths before file access. Reject traversal, absolute paths, unexpected separators, hidden resources, and alternate encodings.
- Validate object IDs against the authenticated user's authorization, not only against syntax.
- Validate state transitions, not just individual fields. For example, a device command may be well-formed but illegal for the current device state.

## Output Encoding

Never write untrusted data directly into an LSP response, HTML attribute, script block, URL, JSON response, log line, shell command, SQL statement, or SMQ command.

- Encode untrusted text for HTML body output.
- Encode separately for HTML attributes, JavaScript contexts, URLs, and JSON.
- Prefer JSON encoders for JSON responses instead of manual concatenation.
- Set the correct response content type for JSON, HTML, plain text, downloads, and errors.
- Avoid reflecting raw request data in error pages, redirects, logs, or diagnostic output.
- Do not render user-supplied HTML unless the application has a deliberate sanitizer and trust model.

## Authentication And Authorization

Use https://realtimelogic.com/downloads/ai-skills/Authentication-Authorization-Skill.md for BAS implementation details.

Apply these general rules:

- Protect all admin, upload, WebDAV/WFS, device-control, configuration, and data-export surfaces.
- Do not rely on hidden links, JavaScript UI state, route naming, or client-side checks for authorization.
- Enforce authorization on every protected request, including AJAX, REST, includes, downloads, uploads, WebDAV methods, and SMQ/WebSocket actions.
- Use least privilege. Separate browser, device, admin, service, and maintenance roles.
- Use generic login and account-recovery responses. Do not reveal account existence through message text, status code, redirect, URL, response shape, or timing.
- Require reauthentication or stronger proof before password changes, role changes, credential/token creation, device provisioning, firmware updates, or account recovery.
- Use MFA, WebAuthn/passkeys, or external identity providers for administrator and high-value remote-management accounts when the product can support it.

## Sessions And CSRF

- Prefer cookie-based sessions for browser applications.
- Treat URL session IDs and session URLs as bearer credentials. Use them only when required by client constraints such as WebDAV, keep lifetimes short, and avoid logging or sharing them.
- Invalidate sessions on logout, password changes, role changes, account recovery, and other risk events.
- Use idle timeouts appropriate to the protected data. Use absolute timeouts or forced reauthentication for high-value applications.
- Protect all state-changing browser-authenticated requests against CSRF. Use server-validated tokens, SameSite cookies where applicable, Origin/Referer checks, Fetch Metadata checks, or another deliberate BAS-compatible design.
- Do not use GET for state-changing operations.

## REST, APIs, And Protocol Endpoints

- Require HTTPS for REST/API endpoints carrying credentials, session cookies, bearer tokens, API keys, or private data.
- Enforce method allowlists. Return safe errors for unsupported methods.
- Require `Content-Type` and parse body formats intentionally.
- Validate authorization per endpoint and per object, not only at login.
- Rate-limit login, token, upload, export, device-command, and expensive query endpoints.
- Keep API error responses generic externally but log enough server-side context for diagnosis.
- For WebSocket or SMQ-like persistent channels, authenticate during connection setup and continue authorizing each message or publish action.
- Bound message size, queue depth, timeouts, and reconnect behavior.

## SQL And Database Access

Use https://realtimelogic.com/downloads/ai-skills/SQLite-Skill.md for BAS SQLite write patterns.

Security rules:

- Do not concatenate untrusted input into SQL.
- Use parameter binding or strict allowlists for dynamic identifiers where binding is not possible.
- Validate and authorize before reads as well as writes.
- Keep schema migrations and admin SQL out of ordinary request paths.
- Enable database integrity controls such as foreign keys when supported and appropriate.
- Avoid logging complete SQL statements when they include secrets, tokens, personal data, or customer data.
- Keep database files in a runtime writable area with appropriate filesystem permissions, not in a public resource tree.

## Files, Uploads, WebDAV, And WFS

Use https://realtimelogic.com/downloads/ai-skills/VFS-skill.md for BAS VFS and mounting details.

- Authenticate and authorize upload, WebDAV, and WFS nodes before exposing them on a network.
- Enforce upload size limits, file count limits, storage quotas, and timeout limits.
- Validate extension, content type, and file signature when the product depends on file type.
- Store uploaded files outside executable/LSP/module directories unless the product explicitly requires editable application content.
- Generate server-side names or canonicalize user names before storage.
- Reject path traversal, hidden-resource names, control characters, reserved names, and unexpected separators.
- Do not serve uploaded files as active LSP, Lua, HTML, JavaScript, or configuration unless that is the intended and authorized workflow.
- Scan or quarantine files when risk and product environment justify it.

## SMQ, WebSocket, And Device Messages

Use https://realtimelogic.com/downloads/ai-skills/SMQ-Skill.md for SMQ protocol design.

- Treat every message payload as untrusted, including messages from authenticated browsers and provisioned devices.
- Authenticate devices and browsers separately.
- Authorize topic/subtopic creation, subscriptions, publishes, and direct commands.
- Validate payload shape and command state before touching hardware, configuration, database state, or files.
- Keep browser privileges separate from device privileges.
- Require TLS and trusted peer verification for deployed device clients where supported.
- Rate-limit or quarantine repeated failed device authentication attempts.
- Keep firmware binaries and large payloads out of SMQ/WebSocket control channels; use authenticated HTTPS/REST download paths.

## Secrets And Configuration

- Do not hardcode production passwords, tokens, API keys, TLS private keys, OAuth secrets, database passwords, or device credentials in LSP pages, browser JavaScript, examples, or logs.
- Keep secrets out of public resource readers and zip/app assets served to browsers.
- Use deployment-specific configuration, protected home/disk IO, OS secret stores, TPM-protected data, or environment-specific provisioning where appropriate.
- Use separate credentials for development, test, staging, production, devices, administrators, and service accounts.
- Rotate credentials after suspected exposure and when changing ownership or deployment scope.
- When logging configuration, redact secrets and token-like values.

## TLS And Network Exposure

- Require TLS for credentials, cookies, bearer tokens, admin tools, device provisioning, firmware updates, uploads, and private data.
- Use browser-trusted certificates for normal browser users on network-reachable systems. Do not train users to accept certificate warnings.
- Prefer loopback or SSH port forwarding for development tools and local admin surfaces.
- Before exposing Mako Developer Edition, Xedge, LSP-Claw, `/rtl/`, `/lsp-claw/`, WebDAV/WFS, or admin endpoints, verify bind address, firewall/router exposure, passwords, tokens, and TLS expectations.
- Avoid placing sensitive tokens in URLs because URLs leak through logs, history, referrers, screenshots, and bookmarks.

## Logging And Error Handling

Log enough to diagnose attacks and failures:

- authenticated user or device identity when known,
- source IP or peer identity,
- endpoint, method, route, topic, or command,
- authorization decision and reason category,
- validation failures,
- rate-limit and lockout events,
- upload and file mutation events,
- admin and configuration changes.

Do not log secrets, passwords, cookies, bearer tokens, session IDs, OAuth codes, private keys, full authorization headers, full prompt-like private data, or large request/response bodies.

Return safe external errors. Avoid stack traces, Lua exceptions, filesystem paths, source paths, SQL text, config details, version banners, or environment variables in browser/API responses.

Use `trace(...)` only for non-secret diagnostic data and remove temporary trace instrumentation after debugging.

## Outbound Requests And SSRF

When BAS code makes outbound HTTP(S), MQTT, WebSocket, mail, or other network requests based on user-controlled data:

- Prefer fixed upstream hosts or allowlists.
- Validate scheme, host, port, path, and redirect behavior.
- Reject loopback, link-local, private, metadata-service, and internal management addresses unless the feature explicitly requires them.
- Re-resolve and re-check addresses after redirects and DNS changes when risk justifies it.
- Set connection and response timeouts.
- Do not forward user-supplied headers, cookies, bearer tokens, or client certificates unless explicitly required.

## Resource Limits And Availability

- Limit request body size, upload size, JSON depth, SMQ/WebSocket message size, database query cost, response size, and execution time.
- Use backpressure, bounded queues, and timeouts for long-running work.
- Do not hold SQLite write transactions, mutexes, sockets, or session locks while waiting on slow network, user interaction, or external tools.
- Rate-limit expensive unauthenticated endpoints and authentication attempts.
- Fail closed when resource limits are exceeded, but use errors that do not disclose internals.

## Secure Code Review Checklist

Before considering a BAS security-sensitive change complete:

- Every external input has a validation point.
- Every protected action has server-side authorization.
- Every untrusted output is encoded for its sink.
- State-changing browser requests have CSRF protection.
- SQL uses binding or strict allowlists.
- Uploads and WebDAV/WFS nodes are authenticated, authorized, size-limited, and path-safe.
- SMQ/WebSocket/device messages are authenticated, authorized, validated, and bounded.
- Secrets are not in source, browser-visible assets, logs, screenshots, or public examples.
- TLS expectations are explicit for every network-reachable credential or admin path.
- Error responses are safe and logs contain useful non-secret evidence.
- Rate limits, size limits, and timeouts are in place for high-risk paths.
- Tests or manual probes cover unauthenticated, unauthorized, malformed, oversized, replayed, and cross-user access attempts.

## Common Anti-Patterns

- Trusting browser-side checks, hidden links, CSS visibility, or disabled buttons.
- Scattering authorization checks across individual pages instead of protecting VFS boundaries and action handlers.
- Writing request data directly into LSP/HTML/JavaScript/JSON/SQL/logs.
- Returning detailed login, authorization, SQL, filesystem, or stack-trace errors to clients.
- Mounting WebDAV/WFS or upload directories without authentication.
- Exposing local development tools on all interfaces without passwords, tokens, firewall review, and TLS expectations.
- Logging cookies, bearer tokens, OAuth codes, session URLs, passwords, or private keys.
- Letting user input choose outbound request destinations, file paths, SQL identifiers, SMQ topics, or hardware commands without allowlists.
