# BAS/BWS Skills

These skills help an AI coding assistant build applications and native
extensions with Barracuda App Server (BAS) and Barracuda Web Server (BWS).
Choose the entry point that matches the work and ask the assistant to read its
`SKILL.md`. The repository's [AGENTS.md](../AGENTS.md) provides the same routing
for assistants working in this checkout.

## Choose a Skill

| Skill | Use it when |
| --- | --- |
| [Build BAS/BWS Native Apps](build-bas-bws-native-apps/SKILL.md) | Creating or modifying C/C++ servers, REST services, WebSockets, Virtual File System (VFS) resources, native Mako/Xedge hosts, or platform integration |
| [Build Xedge and Mako Lua Apps](build-xedge-mako-lua-apps/SKILL.md) | Writing Lua or Lua Server Pages (LSP) applications, routing requests, using databases and messaging, or packaging and deploying applications for Mako, Xedge, or Xedge32 |
| [Build BAS Lua Bindings](build-bas-lua-bindings/SKILL.md) | Exposing C/C++ functions, native objects, hardware, or callbacks to Lua, including ownership, garbage collection, and host registration |

A binding task starts with the binding skill, which links to native build
guidance and the detailed binding references. Add the Lua application skill
when the task also changes application lifecycle or Lua/LSP behavior.

## Use the Shared References

Keep the skill directories together when copying them to another checkout or
an assistant's skill directory. They use relative links to shared guidance;
copying only one `SKILL.md` omits those dependencies.

The Lua application skill holds the downloaded API references and specialist
guides, including [Lua bindings](build-xedge-mako-lua-apps/references/upstream/Lua-Binding-Skill.md)
and [hardware bindings](build-xedge-mako-lua-apps/references/hardware-bindings.md).
The binding skill uses these same files. Read only the references needed for
the task.

Use headers and build settings from the actual target checkout. These skills
do not require a particular workstation, private SDK, lab address, or board.
Provide the target's toolchain and hardware details when the task needs them.

To validate the downloaded references, run this from the BAS repository root
with Python 3:

```sh
# Check local snapshot integrity without downloading or changing files.
python skills/build-xedge-mako-lua-apps/scripts/update_components.py --check
```

For updates, follow the Lua skill's
[refresh procedure](build-xedge-mako-lua-apps/SKILL.md#refresh-upstream-components).
The binding guide contains maintained refinements that must be merged when
refreshing its upstream source.
