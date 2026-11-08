# Deferred HTTP Responses From BAS Callbacks

Use a deferred response when an LSP page or request handler starts asynchronous
work and its HTTP result must report completion of that work. Examples include
a serialized SQLite write or a background device operation.

## Choose When to Respond

If the response means that a database write committed or an operation
completed, finish the response from that operation's completion callback.
Do not keep the original request waiting by polling, spinning, or calling
`ba.sleep()`. An immediate response is appropriate when the API promises only
that work was accepted or queued.

Execute deferred response methods in a BAS Lua thread context, as required by
the API. A callback running in a timer, dispatcher, or native driver context
must arrange suitable thread execution before sending the response.

## Preserve Ownership

Call `response:deferred()` in the original request context, before returning
from it. This closes the normal response and returns the limited deferred
response object. Perform method, authentication, body-size, and field
validation before conversion when possible.

Pass the deferred object and copied or application-owned inputs to the
callback. Do not retain the normal `request`, `response`, LSP `_ENV`, request
reader, or other request-scoped objects. Avoid capturing them indirectly in
callback closures.

Give the callback one terminal response path. Handle synchronous queue
rejection through the deferred object too; the normal response is already
closed. Release operation locks, references, and other owned resources on
success, failure, cancellation, and client disconnect. An early return after
`defresp:valid()` must not skip that cleanup.

## Send in the Required Order

For a fixed response body, use this sequence:

1. `defresp:setstatus(status)` where `status` is an integer HTTP status code.
2. `defresp:setheader(name, value)` for each needed header, using strings.
3. `defresp:setcontentlength(#body)` where `body` is the complete response string
   and its length is a nonnegative byte count.
4. `defresp:send(body)`.
5. `defresp:close()` to flush and close the response.

`send()` requires the content length first. For chunked output, use `write()`
under its documented contract instead. `valid()` returns a boolean indicating
that the response is not closed; it does not guarantee that the client will
receive subsequent writes. Handle transport failures and cleanup accordingly.

## Complete a Database Response

This illustrative LSP fragment runs in a BAS Lua request thread and assumes
two application-provided helpers; neither helper is a BAS API:

- `readAndValidateRequest(request)` returns a validated input table, or `nil`
  after sending a validation error through the normal response.
- `app.queueWrite(input, callback)` returns `true` when accepted or `nil, error`
  when rejected, where `error` is a string. Accepted work invokes the callback
  once in a BAS Lua thread after the transaction ends. The callback receives
  `ok` (boolean) and `result` (a JSON-encodable value on success). Rejected work
  does not invoke the callback. The queue owns transaction cleanup.

```lua
<?lsp
local input = readAndValidateRequest(request)
if not input then return end

-- Convert before queuing; request-scoped objects must not escape.
local defresp = response:deferred()
-- Cache the encoder so the callback does not capture the LSP _ENV.
local encode = ba.json.encode
local function completed(ok, result)
   if not defresp:valid() then return end

   local status, payload
   if ok then
      status, payload = 200, {result=result}
   else
      -- Keep database details in server diagnostics, outside the HTTP body.
      status, payload = 503, {error="database_unavailable"}
   end

   local body = encode(payload)
   defresp:setstatus(status)
   defresp:setheader("Content-Type", "application/json; charset=utf-8")
   defresp:setcontentlength(#body)
   defresp:send(body)
   defresp:close()
end

local queued = app.queueWrite(input, completed)
if not queued then
   -- Queue rejection still uses the deferred response created above.
   completed(false)
end
return
?>
```

Keep the database transaction short. Commit before reporting success, and do
not hold a transaction open while sending HTTP data, performing outbound
requests, sleeping, or doing unrelated slow work. The sample illustrates
response ownership and ordering; apply the host's error handling to callback
exceptions and send failures, and arrange cleanup if queued work is cancelled
or the application unloads.

For the authoritative contract, search `response:deferred()` in
[basapi.md](upstream/basapi.md) and read the
[Deferred Response Object](upstream/basapi.md#defresp).
