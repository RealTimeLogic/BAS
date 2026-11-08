---
name: build-bas-lua-bindings
description: Create, review, and integrate C or C++ Lua bindings for Barracuda App Server, Mako Server, Xedge, and Xedge32. Use for native modules, userdata ownership, native-to-Lua callbacks, peripheral bindings, or BAS directory and I/O objects exposed to Lua. Use the Lua application skill for ordinary Lua/LSP code that needs no native changes.
---

# Build BAS Lua Bindings

Expose native C/C++ functionality to Lua in Barracuda App Server (BAS) or a
BAS host. Define the Lua interface, native ownership, and callback lifecycle
before selecting an integration hook.

## Select the Binding and Host

Identify the target BAS checkout, Lua version, host, operating system, and
binding source. Read the nearest `AGENTS.md` and the selected host's build
instructions. Use public headers from that target; do not depend on a private
SDK, a particular workstation path, or another board's driver behavior.

Choose the smallest binding category that fits:

| Binding category | Use it for |
| --- | --- |
| Generic Lua module | Parsers, calculations, or native objects that need no BAS runtime state |
| BAS-aware module | Functions that need the BAS environment, dispatcher mutex, timers, I/O, or request helpers |
| BAS directory object | An `HttpDir`-derived object that participates in URL routing and directory ownership |

Read the applicable sections of the shared
[BAS Lua binding guide](../build-xedge-mako-lua-apps/references/upstream/Lua-Binding-Skill.md).
It contains the binding templates, metatable rules, reference helpers, and
Mako/Xedge installation patterns. For GPIO or other peripherals, also read
[hardware bindings](../build-xedge-mako-lua-apps/references/hardware-bindings.md).

Use the [native BAS/BWS skill](../build-bas-bws-native-apps/SKILL.md) for the
native build, source formatting, callback declarations, and porting layers.
Compile `src/BAS.c` when integrating with the BAS Lua runtime. Use the
[Lua application skill](../build-xedge-mako-lua-apps/SKILL.md) when the task
also changes application startup, routing, packaging, or unload behavior.

## Check Headers and Reference Versions

Verify Lua declarations in `inc/lua.h` and `inc/lauxlib.h`, BAS helpers in
`inc/balua.h`, and host hooks in the selected Mako/Xedge headers and startup
source. Source paths in the shared guide are relative to the target BAS tree.
In an amalgamated checkout, search `src/BAS.c` for implementations whose
separate source files are absent.

The shared guide is a maintained upstream snapshot. Read the
[target-header compatibility notes](../build-xedge-mako-lua-apps/SKILL.md#target-headers-and-snapshot-compatibility)
before copying snippets with configuration macros or registration hooks.
Current target declarations take precedence over older snippets.

The reference files remain in the sibling Lua application skill so there is
one maintained copy. Keep the sibling skill directories together. From the
BAS repository root, check the binding snapshot with:

```sh
# Verify the shared reference against its recorded integrity manifest.
python skills/build-xedge-mako-lua-apps/scripts/update_components.py --check --component Lua-Binding-Skill.md
```

Follow the Lua skill's [refresh procedure](../build-xedge-mako-lua-apps/SKILL.md#refresh-upstream-components)
when maintaining that snapshot; preserve its recorded local refinements.

## Define Ownership and Error Behavior

Specify argument and return types, units, accepted ranges, ownership, and
whether each operation can block. Define which object owns native handles,
callback references, buffers, and pending work, including partial-construction
failure, explicit close, garbage collection, and host shutdown.

Follow the shared guide's reference and metatable rules and the
[API misuse versus operational failure contract](../build-xedge-mako-lua-apps/SKILL.md#distinguish-api-misuse-from-operational-failure).
Preserve established error returns and additional error details. Document each
helper's stack effect, mutex requirements, and ownership changes beside it.

Never call the Lua C API while the BAS dispatcher mutex is released. Retain
buffers and native objects for the entire blocking or asynchronous operation,
and account for close or reentry while the mutex is released. Deliver native
callbacks through the documented BAS thread mechanism. An interrupt service
routine must queue native event data for a normal task; it must not call Lua.

## Keep Lua Stack Use Bounded

- Pop each consumed table value during iteration instead of accumulating
  entries on the stack. Preserve the key needed by the next `lua_next` call.
- Define each helper's incoming and outgoing stack layout. Restore temporary
  entries on success and preserve documented return values on failure. Keep
  registration functions stack-balanced unless their contract returns values.
- Use `lua_checkstack` when additional stack capacity is needed. A later
  native allocation or size check cannot protect an earlier Lua parsing pass.
- Prefer a local stack fix over a parsing framework. Preserve deliberate
  compact argument parsing unless correctness requires changing it.

## Verify the Changed Contract

Compile the binding with the selected host and porting layer. Exercise the Lua
entry points and inspect runtime trace output. Choose checks that cover the
changed behavior: invalid arguments, operational failure, partial construction,
close/reopen, callback errors, pending work during close or collection, and
unload/reload. For stack-sensitive changes, include oversized input lists and
use Lua's `LUA_USE_APICHECK` and host sanitizers where practical.

For hardware bindings, apply the focused checks in the hardware guide and
report host tests, target compilation, and physical peripheral operation
separately. State the runtime and target used and what remains untested.
