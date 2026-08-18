---
name: deploy-mako-server
description: Plan, configure, install, and verify Mako Server as a Windows service or Linux systemd service, including service identities, absolute paths, mako.conf, foreground smoke tests, logs, restart behavior, and ACME operating choices. Use for production or appliance deployment, VPS setup, boot-time startup, daemon/service troubleshooting, or moving a working interactive Mako application into unattended operation.
---

# Deploy Mako Server

Use a foreground-first workflow to turn a working Mako application into a repeatable unattended service.

## Standalone Use And Local Cache

This skill may be downloaded without the repository `AGENTS.md`. Resolve referenced public BAS documents and skills in this order:

1. A same-directory file with the referenced filename.
2. `./.agents/reference/rtl/` relative to the project.
3. The public source URL.

If a referenced file is missing and network access is available, download it from the public URL and save a local copy before using it. Record the source URL and download date in `./.agents/reference/rtl/manifest.md` or in a short header in the cached file.

## Source Of Truth

Use the following references:

- `basapi.md`: https://realtimelogic.com/downloads/basapi.md
- `Authentication-Authorization-Skill.md`: https://realtimelogic.com/downloads/ai-skills/Authentication-Authorization-Skill.md
- `OWASP-General-Security-Skill.md`: https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md

Use `basapi.md` for current option names, `mako.conf` fields, and ACME behavior. Treat installed-binary help and observed runtime behavior as the final check for that binary version. Do not copy a tutorial command blindly; tutorial examples may target an older release or assume a different operating system.

## Collect The Deployment Contract

Record:

- target OS and CPU architecture;
- Mako executable, `mako.zip`, configuration, application, writable-data, certificate, and log paths;
- service identity and the minimum filesystem/network permissions it needs;
- root application and any named applications;
- HTTP/HTTPS listeners, interfaces, reverse proxy, firewall, and DNS expectations;
- certificate validation mode and renewal connectivity;
- health URL and expected response;
- rollback unit: application archive, configuration, binary, and certificate state.

Stop and ask before changing service registration, privileged ports, firewall rules, DNS, or production certificates unless the user explicitly included that action.

## Build And Verify

1. Keep executable, configuration, applications, and writable state in explicitly chosen locations.
2. Use absolute paths in service configuration. Do not depend on an interactive shell's current directory, profile, mapped drive, or user environment.
3. Put application loading and server options in one `mako.conf`.
4. Run Mako in the foreground with the exact intended configuration.
5. Launch that test from a different working directory to expose hidden relative-path dependencies.
6. Verify application loading, HTTP behavior, HTTPS behavior if configured, write access, logging, and clean shutdown.
7. Install the OS service only after the foreground test passes.
8. Verify service identity, status, logs, health URL, stop/start, automatic restart policy, and boot-time startup.
9. Preserve the previously working binary, configuration, and application package until the deployed version passes verification.

Package completed applications as ZIP files when appropriate. Keep mutable databases, uploads, logs, and certificates outside immutable application archives.

## Deploy A Windows Service

A Windows service runs in a service account context, not the interactive user's context. Interactive-user files and mapped drives are normally unavailable.

Prefer one absolute `-c` configuration path as the service entry point. Use an absolute layout such as:

```text
C:\Mako\
  mako.exe
  mako.zip
  mako.conf
  apps\
  data\
  logs\
```

Confirm the current installation/removal flags with `basapi.md` or the installed binary's help. After registration, check the actual service identity and grant only the directories it must read or write. Verify from the Service Control Manager, event/Mako logs, and the health URL.

## Deploy A Linux Systemd Service

Prefer systemd with `Type=simple` and Mako's systemd-oriented foreground mode documented by the current `basapi.md`. Run as a dedicated unprivileged user.

Use a unit with this shape:

```ini
[Unit]
Description=Mako Server
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=mako
Group=mako
WorkingDirectory=/srv/mako
ExecStart=/usr/local/bin/mako -s -c /srv/mako/mako.conf
Restart=on-failure
RestartSec=3
NoNewPrivileges=true

[Install]
WantedBy=multi-user.target
```

Adapt paths and hardening to the target. If binding directly to ports below 1024, prefer an explicit narrow capability such as `CAP_NET_BIND_SERVICE` or place Mako behind a reverse proxy. Do not run the server as root merely for port access.

Use init.d only when the target has no systemd. Do not combine Mako's self-detaching daemon mode with `Type=simple`; the service manager must track the foreground process.

## Choose Certificate Operation

Choose the ACME challenge from reachability:

| Environment | Preferred challenge |
| --- | --- |
| Public server reachable on port 80 | HTTP-01 |
| Private/NAT server with automated DNS control | Automated DNS-01 |
| Mostly offline system with maintenance windows | Manual DNS-01 |

Use HTTP-01 only when the certificate authority can reach the server on public port 80. Start with the CA staging environment before production issuance.

For manual DNS-01, capture the requested TXT record, wait for DNS propagation, verify it independently, and then resume the ACME flow using the current API procedure. Define an operator, notification path, renewal window, and runbook because an unattended service may have no visible console.

Design for unattended renewal. Do not make recurring manual certificate replacement the normal production plan.

## Protect Production Systems

- Do not preserve tutorial sample credentials.
- Do not use cleartext or MD5-based credential examples as a modern password-storage design.
- Do not expose WebDAV, TraceLogger, an IDE, or administrative paths merely to simplify deployment.
- Load the authentication/authorization and OWASP skills when enabling accounts, remote administration, uploads, or public exposure.
- Keep private keys and service secrets out of source, logs, command history, and browser code.

## Verify And Roll Back

Verify:

- service identity and permissions;
- configuration and application paths;
- expected listeners only;
- health URL over every intended protocol;
- application writes to intended data paths;
- log availability without an interactive console;
- graceful stop and successful start;
- restart after a simulated process failure;
- startup after reboot;
- certificate chain, name, expiration, and renewal mode.

Rollback by stopping the service, restoring the previous binary/configuration/application package as one tested set, restoring certificate state only when required, and starting the service again. Never destroy the last known-good set before verification.

Report the installed paths, service identity, configuration source, listeners, health check, certificate mode, verification results, log location, restart behavior, and rollback procedure. Distinguish verified behavior from assumptions.

## Tutorial Provenance

Distilled from the Mako tutorial bundle downloaded on 2026-07-25:

- `Installing-the-Mako-Server-as-a-Windows-Service`
- `Installing-Mako-Server-as-a-Service-on-Linux`
- `Installing-Mako-Server-on-an-Oracle-Free-Tier-Instance`
- `Lets-Encrypt`

Exact Mako flags and configuration fields are intentionally not duplicated from the API reference.
