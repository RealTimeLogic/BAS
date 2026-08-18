---
name: bas-authentication-authorization
description: Use when designing, reviewing, debugging, or implementing authentication and authorization in BAS, Mako Server, Xedge, or LSP applications, including directory authentication, form/basic/digest login, JSON users, TPM-protected user databases, authorizers, role/path/method access control, request login/logout/session handling, WebDAV/WFS protection, OAuth/OpenID Connect, RADIUS, and WebAuthn/passkeys.
---

# BAS Authentication And Authorization Skill

Use this skill when designing authentication and authorization for BAS-derived web applications.

If you are new to BAS: BAS is the Barracuda App Server, an embedded/edge web application server. Mako Server, Xedge, and Xedge32 are BAS-derived runtimes. LSP means Lua Server Pages. BAS routes HTTP requests through a VFS (virtual file system) made from directory objects and resource readers; authentication is normally attached to those VFS boundaries.

The goal is to choose the safest BAS-native pattern for the protected resource, keep authentication and authorization separate, and verify that unauthenticated and unauthorized requests fail closed.

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

Use these references before inventing or adapting APIs:

- BAS API documentation: https://realtimelogic.com/downloads/basapi.md
- BAS tutorials: https://realtimelogic.com/downloads/tutorials.md
- Mako Server tutorials: https://makoserver.net/download/tutorials.md
- BAS OWASP general security skill: https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md
- OWASP Authentication Cheat Sheet: https://cheatsheetseries.owasp.org/cheatsheets/Authentication_Cheat_Sheet.html
- OWASP Authorization Cheat Sheet: https://cheatsheetseries.owasp.org/cheatsheets/Authorization_Cheat_Sheet.html
- OWASP Session Management Cheat Sheet: https://cheatsheetseries.owasp.org/cheatsheets/Session_Management_Cheat_Sheet.html
- OWASP CSRF Prevention Cheat Sheet: https://cheatsheetseries.owasp.org/cheatsheets/Cross-Site_Request_Forgery_Prevention_Cheat_Sheet.html

Reference priority:

1. Use `basapi.md` for exact API names, signatures, and behavior.
2. Use BAS tutorials for architecture, security patterns, and examples.
3. Use Mako Server tutorials for Mako-specific packaging, deployment, or server behavior.
4. Use OWASP for general web security controls that are not BAS-specific.

## Core Rule

Prefer protecting the VFS directory or resource reader boundary instead of checking authentication in individual pages.

Per-page authentication can work, but it is easier to miss one page, include, endpoint, or static resource. Use manual authentication only when the app has a shared request wrapper that makes omissions obvious.

Authentication is optional and can be installed on any BAS directory type that supports directory behavior, including ordinary directories, resource readers, WebDAV nodes, and Web File Server nodes. Use one authenticator for the whole app when possible, or install multiple authenticators when different subtrees need different realms or policies.

## Decision Guide

- Protect the whole application or resource reader with `dir:setauth(authenticator [, authorizer])`.
- Protect only one subtree by creating a named directory, assigning authentication to that directory, and inserting it before the resource reader with `dir:insertprolog(child, true)`.
- Use `ba.tpm.jsonuser(keyname, global)` for persistent local JSON user databases when TPM support is available and the database should not be stored as plaintext or encrypted with an application-managed key.
- Use `ba.create.jsonuser()` when a JSON user database is needed but TPM protection is not available, not appropriate, or the database is managed by another secure storage layer.
- Use a custom `ba.create.authuser(getpassword)` callback when credentials are checked by application code or an external service.
- Use `ba.create.authorizer(...)` or a JSON user authorizer when access depends on role, path, method, or resource constraints.
- Use `request:login()` when an external identity flow has already verified the user and the BAS session must be marked authenticated.
- Use `request:user()`, `request:session()`, and `request:logout()` for authenticated request state, session handling, and logout flows.
- Use a login tracker when available for internet-facing or password-based login surfaces that need brute-force protection.
- Use MFA, WebAuthn/passkeys, or an external identity provider for administrator, remote-management, and high-value accounts when the product can support it.
- Use OAuth/OpenID Connect, RADIUS, or WebAuthn only when the application actually needs that identity model.

## BAS Auth Primitives

Check `basapi.md` for the exact signatures before generating code.

- `ba.create.authuser(getpassword)` creates a user lookup object from an application callback.
- `ba.create.jsonuser(...)` creates a JSON-backed user database and can support roles and authorization constraints.
- `ba.tpm.jsonuser(keyname, global)` wraps a JSON user database with TPM-protected encrypted storage and exposes `getauth()` for use with `ba.create.authenticator()`.
- `ba.create.authenticator(authuser, options)` creates the authenticator used by directories or manual checks.
- `ba.create.authorizer(...)` creates an authorizer callback object for username, method, and path decisions.
- `dir:setauth(authenticator [, authorizer])` protects a directory/resource boundary.
- `dir:setauth()` with no arguments removes the currently installed authenticator and authorizer.
- `authenticator:authenticate(cmd, uri)` performs an explicit authentication check for manual flows.
- `request:login(...)` marks a session as logged in after application-controlled authentication.
- `request:logout()` logs out the current request/session where the authentication method supports it.
- `request:user()` returns the current authenticated user information.
- `request:session()` gives access to request session state.

## Authentication Methods

Choose the method based on the client and threat model:

- Set the authenticator `type` explicitly. If omitted, BAS Lua authentication defaults to Digest.
- Form authentication is flexible for browser apps and custom login pages.
- Basic and Digest authentication are useful for simple clients and WebDAV-style access, but browser credential caching affects logout and testing.
- The `dav` authenticator is designed for WebDAV clients and lets compatible clients use Basic or Digest.
- The `auth` authenticator is the mixed-client option. It supports Basic, Digest, and form auth, and is useful when both browser and non-browser clients access the same protected area. It requires a more careful response handler because client headers and resource type can influence whether BAS chooses HTTP auth or form auth.
- Secure form login (`sform`) should be used when form credentials must be protected without a normal TLS-only deployment model. It requires a response handler and login page designed for the BAS seed/hash fields.
- OAuth 2.0 and OpenID Connect are appropriate when an external identity provider owns login.
- RADIUS is appropriate when authentication must be delegated to an existing RADIUS server.
- WebAuthn/passkeys are appropriate when the app needs phishing-resistant browser authentication and can require HTTPS or a trusted origin.
- Custom directory functions or bearer-token checks can be used for specialized APIs, but they should still protect the directory boundary and fail closed.

For production systems, use TLS for credentials, tokens, cookies, and session-bearing traffic unless the selected BAS pattern explicitly protects the credential exchange.

Basic and form authentication are unsafe when credentials can cross the network in cleartext. If a protected endpoint may be reached over plain HTTP, redirect to TLS before accepting credentials. For Basic or Digest flows, clear any pending `Authorization` header before redirecting so the client does not continue sending credentials before the secure connection is established.

## OWASP Security Baseline

Apply these OWASP-derived controls in addition to BAS-specific API guidance:

- Return generic login, recovery, and account-creation responses. Do not reveal whether the username, password, account state, or lockout state caused failure, including through different status codes, redirects, timing, or URLs.
- Rate-limit or increasingly delay failed login attempts, but design lockout so attackers cannot easily deny service to other users. Log failures and alert on credential stuffing or brute-force patterns.
- Require reauthentication or stronger proof for sensitive changes such as password changes, account recovery, adding trusted devices, changing identity data, or switching into an administrator role.
- Enforce authorization on every protected request, including AJAX, REST, static files under protected paths, upload/download endpoints, WebDAV/WFS operations, and server-side includes.
- Prefer least privilege and deny by default. Grant access explicitly and review guessed or user-controlled object IDs so one user cannot access another user's resources by changing an ID or path.
- Protect all state-changing browser-authenticated requests against CSRF. Use server-validated CSRF tokens, SameSite cookies where applicable, origin/header checks, or another deliberate BAS-compatible CSRF design. Do not use GET for state-changing operations.
- For REST or API endpoints carrying credentials, session cookies, bearer tokens, or API keys, require HTTPS and validate authorization at each endpoint.

## User Database Storage

For persistent local user databases, prefer `ba.tpm.jsonuser(keyname, global)` when the target runtime supports it.

The TPM wrapper keeps the convenience of the JSON user database API while avoiding plaintext storage and avoiding application-managed encryption keys. Use `tju.getauth()` as the authenticator user database.

Provisioning or user-management code should persist the encrypted database returned by `tju.setuser(...)`. Runtime authentication code should load the encrypted database with `tju.setdb(...)` before creating the authenticator.

Choose the `global` argument intentionally:

- `global = false` makes the encrypted database device-specific. Prefer this when the user database should not be copied to another computer.
- `global = true` allows the encrypted database to be moved between devices that use the same `keyname`. Use this only when migration, backup restore, or a shared deployment model requires it.

Use an application-specific `keyname` and do not reuse it for unrelated databases. Treat stored encrypted user databases, backups, and migration copies as sensitive security material.

When using custom `ba.create.authuser(getpassword)` callbacks, avoid plaintext password storage. Return HA1/hash values where appropriate or delegate to an external authenticator. If the callback receives the command environment, it can inspect the request and deny or redirect insecure login attempts before credentials are accepted.

For password-based local accounts, allow long passphrases, do not silently truncate passwords, and avoid composition rules that reduce usability without improving security. Block common or known-compromised passwords when practical. If password verification is delegated outside BAS, use a modern salted password-hashing/KDF scheme such as Argon2id, bcrypt, scrypt, or PBKDF2 rather than raw hashes.

## Authorization

Authentication answers "who is this user?" Authorization answers "what may this user do?"

Keep these checks separate:

- Put coarse protection on the directory or resource boundary.
- Use authorizer rules or application checks for role, method, path, and resource-specific decisions.
- Default to denying access when a user, role, method, or path is unknown.
- If `request:login()` is used with an authorizer, make sure the logged-in username maps to the expected user record or role constraints.
- Test unauthorized users independently from unauthenticated users.

## VFS Protection Patterns

Whole application/resource reader protection:

```lua
local tju = ba.tpm.jsonuser("myapp-users", false)
local encdb = rwdb() -- App helper that reads the encrypted user DB bytes

if not encdb then error("user database not provisioned") end

local ok, err = tju.setdb(encdb)
if not ok then error(err) end

local authenticator = ba.create.authenticator(tju.getauth(), {
   type = "form",
   response = loginresponse
})

dir:setauth(authenticator)
```

Subtree protection before a resource reader:

```lua
local admin = ba.create.dir("admin")
admin:setauth(authenticator, authorizer)
dir:insertprolog(admin, true)
```

Use the subtree pattern when only part of a static or LSP-backed resource reader should require login, such as `/admin/`.

Authenticators can be installed, replaced, or removed while the system is running. If code creates a directory and inserts it as a root directory, keep a Lua reference to the directory so it is not garbage collected.

## Login Response Handlers

Keep login pages and assets reachable before authentication. If a protected login page needs CSS, JavaScript, or images, place those resources under a public path or serve them from an unprotected directory.

For form authentication, a response handler can forward to an LSP login page. The form must submit `ba_username` and `ba_password`. The response handler should send login, failure, or tracker-denied content, but it must not set HTTP authentication headers; those are managed by the authenticator.

For Basic or Digest authentication, do not rely on forwarding after authentication headers have been set. Use an include or direct response so required authentication headers are preserved.

Be aware that Basic and Digest credentials may be cached by the browser. A test that changes users may require a fresh browser session.

For `sform`, the login page must include the BAS seed fields expected by the authenticator and JavaScript that hashes the password before submission. `sform` does not work when JavaScript is disabled and is not suitable for external authenticators that keep the password secret from BAS. When HA1 password hashes are used, include the authenticator realm field expected by the BAS form code.

## Sessions And Login Tracking

Built-in BAS authenticators create a session when authentication succeeds. Sessions are maintained by an HTTP cookie or by a session ID embedded in the URL, and the built-in session binding locks the session to the client's IP address. If roaming IP addresses or a different trust model are required, use a custom directory function or custom authentication design intentionally.

Do not create session objects for anonymous users unless the application needs anonymous state. BAS authenticators avoid creating the session until the user is authenticated.

A login tracker can record failed and successful logins and ban clients after repeated failures. It is useful against dictionary and brute-force attacks, but the default tracker must be enabled by the C/C++ startup code before Lua code can use the `ba.tracker` interface.

Prefer cookie-based sessions for browser applications. If URL session IDs are required for WebDAV or special clients, treat them as bearer credentials, keep lifetimes short, and avoid exposing them in logs, referrers, screenshots, or documentation. Invalidate sessions server-side on logout and after account-risk events; when changing a password or roles, consider `request:logout(true)` or equivalent session termination for all active sessions.

Set server-side idle timeouts appropriate to the value of the protected data. For high-value applications, also design an absolute session lifetime or forced reauthentication flow.

## External Identity Flows

For external identity providers, the external flow verifies identity first, then the BAS app establishes the local session:

- Validate anti-forgery state and callback parameters.
- Keep client secrets and tokens out of browser-visible pages.
- Store tokens only as long as needed.
- Map external users to local users, roles, and authorization constraints.
- Call `request:login()` only after the external identity proof has succeeded.
- For OpenID Connect, validate issuer, audience, signature, and expiration before trusting an ID token.
- Test failure paths such as invalid state, denied consent, expired tokens, network failures, and unknown users.

## WebDAV And WFS

WebDAV and WFS endpoints must be protected in production.

Browser login flows are not always usable by WebDAV clients. If the app exposes WebDAV or WFS, verify the selected authentication method with both a browser and the intended WebDAV client. If session URLs are used, treat them as bearer credentials and protect them accordingly.

## Common Anti-Patterns

- Protecting only visible pages while leaving JSON, upload, WebDAV, or static endpoints open.
- Mixing authentication and authorization in one ad hoc page check.
- Trusting client-side JavaScript to enforce authorization.
- Hardcoding production passwords, OAuth secrets, or demo users in LSP pages.
- Storing persistent local user databases as plaintext when TPM-protected JSON storage is available.
- Using form login without protecting credentials in transit.
- Returning different login errors, redirects, status codes, or timings for unknown user, wrong password, disabled account, or locked account.
- Letting authenticated browser sessions perform state-changing actions without CSRF protection.
- Passing session IDs or bearer tokens in URLs for normal browser application flows.
- Assuming `request:logout()` fully clears browser-managed Basic or Digest credentials.
- Changing a Digest/form realm without updating stored HA1 hashes.
- Returning a successful login before roles or authorization constraints have been loaded.

## Example References

Use the public LSP-Examples repository for concrete patterns:

- Directory and manual authentication patterns: https://github.com/RealTimeLogic/LSP-Examples/tree/master/authentication
- Form authentication with JSON users and public assets: https://github.com/RealTimeLogic/LSP-Examples/tree/master/Dynamic-Nav-Menu
- JSON users, authorization constraints, Digest auth, `request:login()`, WFS, and WebDAV: https://github.com/RealTimeLogic/LSP-Examples/tree/master/JSON-File-Server
- TPM-protected user database management pattern: https://github.com/RealTimeLogic/LSP-Examples/tree/master/Light-Dashboard
- Microsoft Entra OpenID Connect SSO with WFS/WebDAV session URLs: https://github.com/RealTimeLogic/LSP-Examples/tree/master/fs-sso
- RADIUS-backed authentication callback: https://github.com/RealTimeLogic/LSP-Examples/tree/master/RADIUS
- GitHub OAuth 2.0 starter flow: https://github.com/RealTimeLogic/LSP-Examples/tree/master/oauth
- WebAuthn/passkey authentication: https://github.com/RealTimeLogic/LSP-Examples/tree/master/WebAuthn
- Simple authenticated file server/WebDAV pattern: https://github.com/RealTimeLogic/LSP-Examples/tree/master/File-Server

## Verification Checklist

Before considering the auth design complete:

- Unauthenticated requests to protected paths are denied or redirected.
- Public login assets still load before authentication.
- Protected subtrees apply only to the intended paths.
- Authenticated users can access expected resources.
- Authenticated users without the required role or ACL are denied.
- Unsafe methods such as write, upload, delete, or WebDAV mutation are separately tested.
- Logout and session expiration behavior are tested.
- Basic/Digest browser credential caching is accounted for.
- Login failures use generic user-facing messages and do not expose account existence through status code, redirect, URL, timing, or response shape.
- Brute-force throttling, lockout, logging, and alerting are tested without creating an easy denial-of-service path.
- CSRF protection is tested on login-adjacent and authenticated state-changing browser requests.
- Password changes, role changes, account recovery, and other high-risk events terminate or refresh affected sessions.
- External identity callbacks reject invalid state, missing credentials, failed token exchange, and unknown users.
- OIDC callbacks reject invalid issuer, audience, signature, and expired tokens.
- WebDAV/WFS access is tested with the actual client type when applicable.
