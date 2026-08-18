---
name: design-smq-realtime-apps
description: Use when designing, specifying, reviewing, or implementing language-neutral SMQ application architecture for Barracuda App Server, Mako Server, Xedge, and SMQ clients. Covers topic/subtopic naming, one-to-one and one-to-many routing, broker-mediated IoT patterns, request/reply flows, authentication, authorization, and shared message vocabulary across JavaScript, Lua, C/C++, Java, and Python. Use separate language-specific skills for low-level C/C++, Java, or Python client implementation details.
---

# Design SMQ Real-Time Applications

Use this skill to design language-neutral SMQ systems that are correct for BAS/Mako/Xedge, especially IoT applications with a Mako portal, an SMQ broker, browser clients, and device clients.

If you are new to BAS/SMQ: SMQ is BAS's built-in publish/subscribe messaging system. A server-side broker accepts browser, device, and service clients; topics name shared channels, subtopics name operations or events, and a connected client's ephemeral topic ID is used for direct messages.

## Language Scope

This skill defines SMQ architecture, message taxonomy, routing patterns, and naming conventions that should be consistent across all supported clients:

- Server-side Lua broker code
- Lua device clients
- JavaScript browser clients
- C/C++ embedded clients
- Java clients
- Python tools or service clients

Keep the abstract vocabulary consistent across languages: `sendToBroker`, `sendToPeer`, `publishEvent`, `subscribeToEvent`, `subscribeToDirectMessage`, and `registerBrokerHandler`.

Use language-specific skills for API mechanics that differ substantially, especially C/C++ receive loops, topic ID creation, ACK handling, buffer management, and reconnect behavior.

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

Use the official docs before inventing APIs:

- BAS API bundle: `https://realtimelogic.com/downloads/basapi.md`
- Mako tutorials bundle: `https://makoserver.net/download/tutorials.md`
- BAS tutorials bundle: `https://realtimelogic.com/downloads/tutorials.md`
- BAS OWASP general security skill: `https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md`

Trust `basapi.md` for signatures and behavior. Use tutorials for architecture and patterns.

Use the OWASP general security skill when an SMQ design includes browser sessions, REST handoff, bearer tokens, device commands, firmware updates, uploads, secrets, TLS exposure, or logging requirements.

This skill includes generalized patterns from a production-style IoT portal and device application: a Mako-hosted broker, browser SPA clients, Lua device clients, broker-side helper wrappers, direct request/reply messaging, broadcast state events, device quarantine, and OTA coordination.

## SMQ Design Intake

Do not assume the human knows which SMQ details to provide. If the request is vague or underspecified, briefly interview the human before designing the protocol or writing code. Ask only the missing high-impact questions; do not turn the intake into a long form if the answer can be inferred safely.

Classify these areas:

- Actors: browser, device, broker app, backend service, mobile app, C/C++ client, Java client, Python tool, or other SMQ client.
- Trust: which actors are trusted, semi-trusted, untrusted, authenticated, provisioned, or anonymous.
- Topology: broker-mediated only, peer-to-peer via `ptid`, hybrid, single broker, or cluster.
- Message types: commands, events, direct responses, state reports, discovery messages, and diagnostics.
- Routing: which messages are one-to-one direct messages and which are one-to-many broadcast events.
- Readiness: whether any actor needs initial state immediately after connect; if yes, require the final-subscription-`onack` readiness barrier.
- Identity and authentication: browser login, device credential, provisioning, quarantine, denial, or anonymous demo access.
- State ownership: what is authoritative in broker memory, device memory, database, browser state, or external services.
- Payloads: JSON, text, binary, or mixed; identify any large payloads that should use HTTP/REST instead of SMQ.
- Reconnect: what each actor must rebuild, rediscover, re-request, fail, or retry after reconnect.
- Scale and security: expected number of devices/browsers, fixed topic/subtopic names, deny-by-default authorization, and whether `onpublish`, `permittop`, and `permitsubtop` must enforce routing.
- Implementation languages: Lua broker, Lua device, JavaScript browser, C/C++, Java, Python, or another client. Use separate language-specific skills for low-level API mechanics after the architecture is settled.

If enough context is missing, ask concise questions such as:

- What actors connect to SMQ, and which language/runtime does each actor use?
- Should all traffic go through the broker, or may clients send direct `ptid` messages to each other?
- Which messages are commands, state reports, direct responses, and broadcast events?
- How do browsers and devices authenticate, and what happens to unknown devices?
- Which state is authoritative in the device, broker, database, and browser?
- What must happen after reconnect?
- Are payloads JSON, text, binary, or mixed?

After intake, produce a compact design brief before code:

- Actor list and trust model.
- Message matrix with name, sender, receiver, payload type, route, and response/event behavior.
- Topic/subtopic naming plan.
- Direct vs broadcast routing plan.
- Readiness and reconnect plan.
- Authentication and authorization plan.
- Payload type plan.
- Minimal skeleton for the target languages.

## Core SMQ Model

Think in these primitives:

- Named topic: durable application channel such as `"DeviceState"` or `"/game/state"`.
- Subtopic: second-level message type or operation name such as `"SetCharger"` or `"eaten"`.
- Ephemeral topic ID, or etid: per-connected-client address. The callback argument `ptid` is the publisher's ephemeral topic ID.
- `"self"`: special topic name that subscribes a client to its own ephemeral topic ID, enabling direct one-to-one messages.
- Server broker TID: normally `1` unless the broker is created with `rndtid=true`. Use `smq:gettid()` if code must tolerate a randomized server TID.

Use one-to-many topics for events that many clients should see. Use one-to-one direct publish to an ephemeral TID for commands, request/reply, ACKs, and device-specific work.

Exact Lua publish signatures:

```lua
smq:publish(data, "topic")          -- one-to-many broadcast
smq:publish(data, ptid, "subtopic") -- one-to-one direct message
```

Exact JavaScript publish shape:

```js
smq.publish(data, topic, subtopic)
smq.pubjson(value, topic, subtopic)
```

## Recommended Architecture

Use a hub-and-spoke design for managed IoT systems:

- Mako/BAS broker owns authentication, authorization, connection tables, state correlation, and backend access.
- Devices connect outward to the broker using the Lua or C SMQ client. They subscribe to `"self"` for commands.
- Browsers connect after normal HTTP/Web authentication. They subscribe to public event topics and `"self"` response subtopics.
- Browsers and devices publish requests to the broker, not directly to each other, when the portal must enforce policy or audit actions.
- The broker forwards validated device events to browser event topics and sends validated commands directly to device ephemeral TIDs.

Allow peer-to-peer SMQ only for simpler apps such as chat, games, or demos where clients can safely talk directly. For production IoT, prefer broker-mediated flows and enforce them with `onpublish`.

## Broker Setup Pattern

Create the broker in `.preload` and expose a tiny SMQ entry point via an LSP page or directory function. Keep the entry point free of application logic.

```lua
local smq = require"smq.hub".create(op)

local function smqEntry(_ENV, relpath)
   if #relpath == 0 then
      if require"smq.hub".isSMQ(request) then
         smq:connect(request, {type="browser", user=request:user()})
      else
         response:senderror(400, "Not an SMQ connection request")
      end
   else
      return false
   end
   return true
end

local smqDir = ba.create.dir"smq"
smqDir:setfunc(smqEntry)
dir:insert(smqDir, true)

function onunload()
   smq:shutdown()
end
```

Use `smq:connect(request, arg)` to upgrade the HTTP(S) request into an SMQ connection. The optional `arg` is available as `info.arg` in broker callbacks.

Guard broker endpoints so ordinary browser navigation, crawlers, and incorrect HTTP requests do not accidentally enter the SMQ upgrade path. For LSP endpoints, use `require"smq.hub".isSMQ(request)` or equivalent request-header checks before calling `smq:connect(request)`.

## Messaging Vocabulary

Use the same conceptual helper names in browser, broker, device, and language-specific client examples. The implementation differs by runtime, but the mental model should not.

Recommended verbs:

- `sendToBroker(messageName, payload)`: send a direct one-to-one message to the broker.
- `sendToPeer(peerTid, messageName, payload)`: send a direct one-to-one message to a specific browser or device.
- `publishEvent(eventName, payload)`: publish a one-to-many event on a named topic.
- `subscribeToEvent(eventName, handler)`: receive one-to-many events on a named topic.
- `subscribeToDirectMessage(messageName, handler)`: receive one-to-one messages sent to `"self"` with `messageName` as subtopic.
- `registerBrokerHandler(messageName, handler)`: broker-only alias for `subscribeToDirectMessage`; use it for request handlers installed on the broker.

Use `messageName` for subtopics used as operation names, such as `"SetCharger"` or `"DeviceReq"`. Use `eventName` for broadcast topics, such as `"DeviceState"`. Use `peerTid` for an ephemeral topic ID captured from `ptid`, a connection table, or broker `onconnect`.

The same string may be used in more than one route when that makes the product protocol easier to read. For example, a device can send `sendToBroker("ChargerState", payload)` as a direct report, and the broker can validate it and then call `publishEvent("ChargerState", payload)` as a browser event. If the two payloads differ materially, prefer distinct names such as `"ChargerStateReport"` and `"ChargerState"`.

Runtime mapping:

| Helper | Route | JavaScript browser | Lua broker API | Lua device client |
|---|---|---|---|---|
| `sendToBroker(messageName, payload)` | direct to broker | `smq.pubjson(payload, brokerTid, messageName)` | usually not needed inside broker | `smq:publish(payload, brokerTid, messageName)` |
| `sendToPeer(peerTid, messageName, payload)` | direct to peer | `smq.pubjson(payload, peerTid, messageName)` | `smq:publish(payload, peerTid, messageName)` | rarely needed in broker-mediated device code |
| `publishEvent(eventName, payload)` | one-to-many topic | `smq.pubjson(payload, eventName)` | `smq:publish(payload, eventName)` | usually avoid unless devices may broadcast |
| `subscribeToEvent(eventName, handler)` | one-to-many topic | `smq.subscribe(eventName, {datatype:"json", onmsg:handler})` | `smq:subscribe(eventName, {json=true, onmsg=handler})` | `smq:subscribe(eventName, {datatype="json", onmsg=handler})` |
| `subscribeToDirectMessage(messageName, handler)` | direct to `"self"` | `smq.subscribe("self", messageName, {...})` | `smq:subscribe("self", {subtopic=messageName, ...})` | `smq:subscribe("self", messageName, {...})` |

The broker's server-side subscribe API uses an option table with `subtopic=messageName`. JavaScript and Lua device clients pass the subtopic as the second positional argument. Keep the helper names the same even though the raw APIs differ.

Use `brokerTid = 1` only when the broker uses the default server ephemeral TID. If the broker is created with `rndtid=true`, do not hard-code `1`; distribute or discover the broker TID and keep it behind the `brokerTid` variable.

### Abstract Pattern

In a broker-mediated application, all actors use the same routing rules:

1. A browser or device calls `sendToBroker("SomeReq", payload)`.
2. The broker handles it with `registerBrokerHandler("SomeReq", handler)`.
3. The broker replies privately with `sendToPeer(requesterTid, "SomeResp", payload)` or commands a device with `sendToPeer(deviceTid, "SomeCommand", payload)`.
4. Any actor that must receive direct messages installs `subscribeToDirectMessage("SomeResp", handler)` or `subscribeToDirectMessage("SomeCommand", handler)`.
5. The broker or a trusted actor calls `publishEvent("SomeState", payload)` for one-to-many state updates.
6. Browsers call `subscribeToEvent("SomeState", handler)` to update the UI.

This vocabulary maps cleanly to JavaScript, Lua broker code, Lua device code, and lower-level client implementations in C/C++, Java, or Python. Language-specific examples should keep the same helper names around their native connect, subscribe, publish, and received-message dispatch APIs.

### JavaScript Browser Helpers

```js
const brokerTid = 1;

export function sendToBroker(messageName, payload) {
  smq.pubjson(payload || {}, brokerTid, messageName);
}

export function sendToPeer(peerTid, messageName, payload) {
  smq.pubjson(payload || {}, peerTid, messageName);
}

export function publishEvent(eventName, payload) {
  smq.pubjson(payload || {}, eventName);
}

export function subscribeToEvent(eventName, handler) {
  smq.subscribe(eventName, { datatype: "json", onmsg: handler });
}

export function subscribeToDirectMessage(messageName, handler) {
  smq.subscribe("self", messageName, { datatype: "json", onmsg: handler });
}
```

Example use:

```js
subscribeToEvent("DeviceState", onDeviceState);
subscribeToDirectMessage("SearchSitesResp", onSearchSitesResp);
sendToBroker("SearchSitesReq", { SiteName: searchText });
```

### Lua Broker Helpers

```lua
local smq

local function registerBrokerHandler(messageName, handler)
   smq:subscribe("self", {subtopic=messageName, json=true, onmsg=handler})
end

local function sendToPeer(peerTid, messageName, payload)
   smq:publish(payload or {}, peerTid, messageName)
end

local function publishEvent(eventName, payload)
   smq:publish(payload or {}, eventName)
end
```

Example use:

```lua
registerBrokerHandler("SearchSitesReq", function(request, requesterTid)
   database.searchSites(request.SiteName, function(rows)
      sendToPeer(requesterTid, "SearchSitesResp", rows)
   end)
end)

publishEvent("DeviceState", {DeviceID=device.DeviceID, Chargers=chargers})
```

### Lua Device Helpers

```lua
local brokerTid = 1
local smq = require"smq.client".create(url, op)

local function sendToBroker(messageName, payload)
   smq:publish(payload or {}, brokerTid, messageName)
end

local function subscribeToDirectMessage(messageName, handler)
   smq:subscribe("self", messageName, {datatype="json", onmsg=handler})
end
```

Example use:

```lua
subscribeToDirectMessage("SetCharger", onSetCharger)
sendToBroker("DeviceReady", {})
sendToBroker("ChargerState", {ChargerID=chargerId, On=charger.On})
```

## Message Taxonomy

Define messages before coding. Separate commands, events, and responses.

Use names consistently:

- Requests to the broker: suffix `Req`, for example `DeviceReq`, `Chargers4SiteReq`.
- Direct responses from broker to requester: suffix `Resp`, for example `DeviceResp`, `Chargers4SiteResp`.
- Direct device reports to the broker: use the state/event name if the broker forwards the same payload, for example `ChargerState`; use suffix `Report` if the payload is device-specific, for example `ChargerStateReport`.
- Broadcast state events: readable nouns without `Req` or `Resp`, for example `DeviceState`, `ChargerState`, `Quarantined`, `Error`.
- Device commands from broker: imperative names, for example `SetCharger`, `ToggleCharger`, `SetDeviceID`, `ResetDevice`.
- OTA or workflows that require multiple phases may reuse one subtopic with distinct payload shapes, but document the phases explicitly.

For JSON tables with numeric keys crossing Lua and JavaScript, encode keys as strings if the object must remain an object and not become a JSON array.

## Payload Type Discipline

Choose the payload type per message and make the subscriber match it:

- Use JSON for structured application messages, commands, state, and request/reply payloads.
- Use text for human-readable chat/log/status messages that are not structured.
- Use binary payloads such as `Uint8Array` for compact device commands, sensor frames, or protocol packets.

Do not subscribe with JSON decoding for binary messages. A JSON decode failure prevents the normal `onmsg` callback from running and may send the raw message to a global/unhandled handler instead.

Examples:

```js
smq.subscribe("DeviceState", {datatype:"json", onmsg:onDeviceState});
smq.subscribe("ChatMsg", {datatype:"text", onmsg:onChatMessage});
smq.subscribe("/m2m/led/device", "led", {onmsg:onLedBinaryFrame});
```

For binary browser payloads, use typed arrays and document the byte layout near the message definition:

```js
const data = new Uint8Array(2);
data[0] = ledId;
data[1] = isOn ? 1 : 0;
smq.publish(data, deviceTid);
```

Prefer names that read well in helper calls:

```lua
registerBrokerHandler("Chargers4SiteReq", onChargersForSiteRequest)
sendToPeer(requesterTid, "Chargers4SiteResp", chargers)
sendToPeer(deviceTid, "SetCharger", command)
publishEvent("ChargerState", state)
```

Avoid names that encode transport details, such as `"SelfChargerResp"`, `"BroadcastDeviceState"`, or `"TopicSetCharger"`. The helper function already communicates the route.

## Readiness Handshake

Do not let the broker send application messages to a client merely because broker `onconnect` fired. At that point, the SMQ connection exists, but the client may not yet have subscribed to the named topics and `"self"` subtopics it needs. Direct messages sent to the client's ephemeral TID can be lost if the relevant `"self"` subtopic subscription has not completed.

Use a final subscription ACK as a readiness barrier:

1. The client creates all normal event subscriptions and direct `"self"` subtopic subscriptions.
2. The client makes one final harmless subscription with an `onack` callback.
3. SMQ processes subscription requests sequentially, so when the final `onack` runs, all earlier subscription requests have been accepted or denied.
4. The client then sends `ClientReady`, `DeviceReady`, or an equivalent direct message to the broker.
5. The broker starts sending initial state, direct responses, or device commands only after it receives this ready message.

The `onack` callback is often documented for authorization scenarios where a broker may deny a subscription. In this design, use it as a sequencing primitive. Still check `accepted`; if it is false, do not send the ready message.

Device pattern:

```lua
smq.onconnect = function()
   subscribeToDirectMessage("SetDeviceID", onSetDeviceID)
   subscribeToDirectMessage("DeviceState", onDeviceState)
   subscribeToDirectMessage("SetCharger", onSetCharger)

   smq:subscribe("self", "DeviceReady", {onack=function()
      sendToBroker("DeviceReady", {})
   end})
end
```

Browser pattern:

```js
smq.subscribe("SubscriptionBarrier", {
  onack: function(accepted) {
    if (accepted) sendToBroker("ClientReady", {});
  }
});
```

The barrier topic name is arbitrary; choose a readable name such as `"SubscriptionBarrier"` or `"ClientReadyBarrier"`. The important part is that this subscription is issued last and has the `onack` callback that triggers the ready message.

## Authentication And Authorization

Use browser HTTP/Web authentication for browser clients. Avoid embedding SMQ passwords in JavaScript. In the broker `authenticate(credentials, info)`, inspect `info.uname`, `request:user()`, or the explicit `arg` passed to `smq:connect`.

Use SMQ protocol authentication for devices. Devices may return a token or JSON credential from `smq.onauth`. Require TLS with `trusted=true` in production.

Device Lua pattern:

```lua
local op = {uid=esp32 and esp32.mac(), trusted=true}
local smq = require"smq.client".create(url, op)

smq.onauth = function(rnd, ipaddr)
   local cfg = cfgfile()
   return ba.json.encode{DeviceID=cfg.DeviceID, DeviceToken=cfg.DeviceToken}
end
```

Broker-side checks to include:

- `authenticate(credentials, info)` accepts authenticated browsers and validates device credentials.
- `onconnect(tid, info, peer)` classifies peers into browser/device/quarantine tables.
- `onclose(tid, sock, peer, err)` removes all connection-indexed state and broadcasts disconnect events if needed.
- `permittop(topic, issub, peer)` limits topic creation and subscription.
- `permitsubtop(subtopic, peer)` limits subtopic creation.
- `onpublish(data, ptid, tid, subtid, peer)` validates every attempted publish and can drop illegal messages.

A broker-mediated topology can be enforced with:

```lua
onpublish=function(data, ptid, tid, subtid, peer)
   if 1 ~= ptid and 1 ~= tid then
      return false
   end
   return true
end
```

This allows browser/device to broker and broker to browser/device, while dropping client-to-client messages.

## Debug Instrumentation

During development, add broker-side `onpublish`, `ondrop`, and global unhandled-message logging. These callbacks make routing mistakes visible: wrong topic names, missing subscriptions, stale peer TIDs, unexpected payload types, and messages dropped because no subscriber exists.

Broker trace pattern:

```lua
local smq

local function describePayload(data)
   return ba.json.decode(data) and data or "?"
end

local op = {
   onpublish=function(data, ptid, tid, subtid, peer)
      trace(string.format("ptid=%u tid=%s subtid=%s data=%s",
         ptid,
         smq:tid2topic(tid) or tostring(tid),
         smq:tid2subtopic(subtid) or tostring(subtid),
         describePayload(data)))
      return true
   end,
   ondrop=function(data, ptid, tid, subtid, peer)
      trace(string.format("DROP ptid=%u tid=%s subtid=%s data=%s",
         ptid,
         smq:tid2topic(tid) or tostring(tid),
         smq:tid2subtopic(subtid) or tostring(subtid),
         describePayload(data)))
   end
}

smq = require"smq.hub".create(op)
```

Use `smq:tid2topic()` and `smq:tid2subtopic()` in logs whenever possible. Numeric TIDs are useful for protocol work, but topic names are faster for humans to debug.

## State Tables

Keep connection-indexed state in memory and rebuild it on connect.

Useful tables:

- `browsersT[tid] = {user=..., session=...}`
- `devicesT[tid] = {DeviceID=..., chargers={...}}`
- `devicesByIdT[DeviceID] = tid`
- `chargersT[ChargerID] = deviceTid`
- `quarantinedDevicesT[tid] = sock`
- `pendingDevicesT[tid] = true` when a ready message races database lookup.

On close:

- Remove connection-indexed entries.
- Remove reverse indexes.
- Broadcast offline state to browsers.
- Move unknown or reset devices into quarantine only when the product workflow requires it.

Do not store volatile device state such as relay on/off in the database unless the product requires persistence. A practical pattern is to keep online state in broker/device memory and enrich database responses with an `On` attribute only when the device is online.

## Request/Reply Pattern

Simulate RPC with direct SMQ messages:

1. Browser or device publishes a request/report to `brokerTid` with the request name as subtopic.
2. Broker has `registerBrokerHandler("RequestName", handler)`.
3. Handler validates `browsersT[tid]` or `devicesT[tid]`.
4. Handler performs work, often async.
5. Broker replies with `sendToPeer(tid, "ResponseName", data)`.

This avoids per-request dynamic topics and keeps responses private to the requester.

### RPC Correlation Pattern

For a reusable RPC layer over SMQ, add correlation IDs and pending-call cleanup. This is especially useful when many requests can be in flight at the same time.

Client-side pattern:

1. Subscribe once to `"self"` response subtopic, such as `"$RpcResp"`.
2. Maintain `pendingCalls[id] = {resolve, reject}` or the equivalent callback record.
3. For each call, increment an ID and send `{id=id, name=methodName, args=args}` to broker TID `1` using request subtopic `"$RpcReq"`.
4. When `"$RpcResp"` arrives, look up `pendingCalls[response.id]`, remove it, then resolve with `response.rsp` or reject with `response.err`.
5. On SMQ disconnect, reject or fail all pending calls and clear the table so callers do not hang forever.

JavaScript sketch:

```js
let pendingCalls = {};
let nextRpcId = 0;

subscribeToDirectMessage("$RpcResp", function(response) {
  const pending = pendingCalls[response.id];
  if (!pending) return;
  delete pendingCalls[response.id];
  if (response.err) pending.reject(new Error(response.err));
  else pending.resolve(response.rsp);
});

function callRpc(name, args) {
  const id = ++nextRpcId;
  return new Promise(function(resolve, reject) {
    pendingCalls[id] = {resolve, reject};
    sendToBroker("$RpcReq", {id:id, name:name, args:args});
  });
}

function failPendingRpc(error) {
  for (const id in pendingCalls) pendingCalls[id].reject(error);
  pendingCalls = {};
}
```

Server-side broker pattern:

```lua
registerBrokerHandler("$RpcReq", function(request, requesterTid)
   local f = rpcInterface[request.name]
   local ok, result, err
   if f then
      ok, result, err = pcall(f, table.unpack(request.args or {}))
      if not ok then err, result = result, nil end
   else
      err = "RPC function not found"
   end
   sendToPeer(requesterTid, "$RpcResp", {id=request.id, rsp=result, err=err})
end)
```

Keep this layer for small/medium RPC payloads. Use HTTP/REST or streaming APIs for large transfers.

## Broadcast Event Pattern

Use broadcasts for state that all interested UIs should update:

```lua
publishEvent("DeviceState", {DeviceID=device.DeviceID, Chargers=chargers})
publishEvent("ChargerState", {ChargerID=msg.ChargerID, On=msg.On})
publishEvent("Quarantined", quarantinedDevices)
```

Browsers subscribe once at connection startup:

```js
subscribeToEvent("DeviceState", onDeviceState);
subscribeToEvent("ChargerState", onChargerState);
subscribeToEvent("Quarantined", onQuarantined);
```

Broadcasts may include messages produced by the same browser. If self-echo is unwanted, compare callback `ptid` with `smq.gettid()`.

## Device Command Pattern

Route browser commands through the broker:

```lua
registerBrokerHandler("SetCharger", function(req, tid)
   local dtid = chargersT[req.ChargerID]
   if dtid then sendToPeer(dtid, "SetCharger", req) end
end)
```

Device subscribes:

```lua
subscribeToDirectMessage("SetCharger", function(cmd)
   assert("number" == type(cmd.ChargerID))
   assert("boolean" == type(cmd.On))
   -- apply hardware change, persist if needed, then publish state
   sendToBroker("ChargerState", {ChargerID=cmd.ChargerID, On=cmd.On})
end)
```

Always validate payload shape before touching hardware.

## Discovery And Presence

For peer-to-peer style apps, use `ptid` and `observe`:

- Subscribe all clients to a discovery topic, such as `"Hello"` or `"/m2m/led/display"`.
- Subscribe each client to `"self"` with the discovery response subtopic, such as `"Hello"` or `"devinfo"`.
- New clients broadcast hello on the discovery topic.
- Existing clients receive hello and respond directly to the sender's `ptid`.
- The new client builds its peer/device list from direct responses sent to `"self"`.
- Each client calls `smq.observe(ptid, cb)` to detect disconnect and remove stale peers.
- On reconnect, clear volatile peer/device UI state and repeat discovery by broadcasting hello again.

Browser sketch:

```js
subscribeToEvent("Hello", function(message, peerTid) {
  if (peerTid === smq.gettid()) return;
  sendToPeer(peerTid, "Hello", {name:myName});
  observePeer(peerTid);
});

subscribeToDirectMessage("Hello", function(message, peerTid) {
  peers[peerTid] = {name:message.name};
  observePeer(peerTid);
});

function announcePresence() {
  publishEvent("Hello", {name:myName});
}
```

Use self-message filtering when a client also receives its own broadcast. Compare callback `ptid` with `smq.gettid()` and ignore, mark as local confirmation, or handle it separately.

Do not rely on `observe` for a clustered broker setup unless the docs say the specific observation is cluster-safe for your use case. For managed IoT, broker `onconnect` and `onclose` state tables are usually clearer.

## Reconnect Rules

JavaScript clients can queue subscriptions and publishes before connect, but robust apps should use `{cleanstart:true}` and explicitly set up subscriptions after each connect/reconnect.

Lua clients cannot subscribe or publish before `onconnect`. Re-subscribe after reconnect. If `onreconnect` is not supplied, `onconnect` is called after reconnect.

On reconnect:

- Recreate all event subscriptions and `"self"` direct-message subscriptions.
- Repeat the readiness barrier before asking the broker for initial state.
- Clear volatile peer/device maps that were keyed by old ephemeral TIDs.
- Repeat discovery by publishing hello again if peers/devices are discovered dynamically.
- Fail, retry, or rebuild pending request/reply or RPC calls; do not leave pending promises/callbacks waiting for responses from the old connection.

Device `onclose` should decide between reconnect delay and restart:

```lua
smq.onclose = function(message, canreconnect)
   if canreconnect then return 5000 end
   esp32.execute"restart"
end
```

Browser `onclose` should distinguish authentication failure from transient network loss.

## OTA Workflow Pattern

Use SMQ to coordinate, not transfer large firmware payloads.

Production-style OTA pattern:

- REST `/rest/updatesw` starts the job.
- Broker snapshots connected devices and processes them sequentially.
- Broker sends direct `UpdateSoftware` without token to ask whether firmware/app is stale.
- Device replies direct `UpdateSoftware` with booleans.
- If update is needed, broker sends direct `UpdateSoftware` with a bearer token.
- Device downloads firmware/app via HTTPS REST using that token.
- Broker uses timers to avoid blocking on a silent device and moves to the next device.

Use REST or HTTP streaming for large binaries. Keep SMQ payloads small and control-plane oriented.

## Security Checklist

- Hide broker paths where practical. Do not advertise device-only SMQ URLs.
- Require TLS and `trusted=true` for deployed device clients.
- Use HTTP/Web authentication for browsers.
- Use per-device credentials or tokens for headless devices.
- Rate-limit or quarantine failed device authentication attempts when practical.
- Use `permittop`, `permitsubtop`, and `onpublish`; authentication alone is not enough.
- Deny unknown topics and subtopics for devices.
- Keep browser privileges separate from device privileges.
- Do not expose server secrets in client JavaScript.
- Validate JSON type and required fields in every request handler.
- Guard SMQ endpoints so normal HTTP requests do not call `smq:connect(request)`.
- Add `ondrop` and global `onmsg` logging during development to catch bad routing.

## Review Checklist

When reviewing an SMQ design or patch:

1. Verify every publish uses the correct direction: broadcast topic or direct ephemeral TID.
2. Verify Lua direct publish uses `smq:publish(data, ptid, "subtopic")`.
3. Verify JavaScript request wrappers send to broker TID `1` only when the broker uses default server TID.
4. Confirm every client subscribes to `"self"` before expecting direct messages.
5. Confirm readiness messages are sent only after subscription ACKs.
6. Confirm the broker does not send initial state or direct commands from broker `onconnect`; it should wait for `ClientReady`, `DeviceReady`, or an equivalent application ready message.
7. Confirm Lua clients do not publish or subscribe before `onconnect`.
8. Confirm reconnect re-installs subscriptions and repeats the readiness barrier.
9. Confirm broker `onclose` cleans connection-indexed tables and reverse indexes.
10. Confirm authorization callbacks constrain topic and subtopic creation.
11. Confirm `onpublish` blocks unintended client-to-client routing in broker-mediated systems.
12. Confirm SMQ endpoints reject or redirect non-SMQ HTTP requests before calling `smq:connect(request)`.
13. Confirm RPC/request-reply code correlates responses by ID when multiple requests can be in flight.
14. Confirm disconnect handling fails, retries, or clears pending RPC/request callbacks.
15. Confirm subscription `datatype` matches each message's JSON, text, or binary payload.
16. Confirm reconnect rediscovery rebuilds peer/device lists that depend on ephemeral TIDs.
17. Confirm payload sizes stay below SMQ max payload limits and large data uses HTTP/REST.
18. Confirm local state, database state, and broadcast state have a clear owner.

## Common Pitfalls

- Treating SMQ like MQTT and trying to use wildcard subscriptions.
- Creating many dynamic response topics instead of replying to `ptid`.
- Publishing device commands as broadcast events.
- Forgetting that `"self"` is a special topic name, not a literal shared topic.
- Letting browsers talk directly to devices in a system where the broker must enforce authorization.
- Sending ready messages before all command subscriptions are acknowledged.
- Sending broker `onconnect` messages before the client has installed event and `"self"` subtopic subscriptions.
- Leaving pending RPC promises or callbacks unresolved after disconnect.
- Reusing an old peer/device list after reconnect even though ephemeral TIDs may have changed.
- Subscribing with `datatype:"json"` to a binary message.
- Assuming JavaScript and Lua client reconnect behavior is identical.
- Using numeric Lua table keys that encode to JSON arrays when the browser expects an object.
- Storing transient connection state only in the database.
- Moving firmware binaries over SMQ instead of REST.
- Ignoring `ondrop`, which often reveals missing subscriptions or stale TIDs.
