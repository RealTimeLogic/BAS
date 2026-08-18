---
name: bas-sqlite-dedicated-writer
description: Use when building BAS, Mako Server, Xedge, or LSP applications that write to SQLite using one dedicated writer thread, one persistent write connection, optional commit batching, and measured selection between rollback journaling and WAL.
---

# BAS SQLite Dedicated Writer Skill

## Purpose

If you are new to BAS: BAS request handlers, LSP pages, timers, and background tasks can run concurrently. SQLite supports many readers but only one writer at a time, so this skill keeps writes out of request handlers and serializes them through one BAS-managed writer thread.

SQLite writes must be serialized through one dedicated writer thread per
database. The writer thread owns one persistent SQLite write connection. All
`CREATE`, `ALTER`, `DROP`, `INSERT`, `UPDATE`, and `DELETE` operations must be
queued to this writer thread.

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

For broader OWASP-style review of SQL injection, authorization before reads and writes, secret handling, logging, error handling, and database file exposure, also use https://realtimelogic.com/downloads/ai-skills/OWASP-General-Security-Skill.md.

This version uses a lower-level API shaped like:

```lua
dbExec(sql, noCommit, callback, errCallback)
```

The important rule is:

- Commit by default.
- Defer the commit only when `noCommit == true`.
- Commit explicitly on the final queued write by omitting `noCommit`, passing
  `nil`, or passing `false`.

The performance benefit comes from reducing SQLite commits, not from merely
using the same thread. Several writes that belong to one logical operation
should normally share one transaction and commit once at the end.

## Core Design

Use this shape in `.preload` or in a Lua module loaded by `.preload`.

The example below uses Mako Server's `sqlutil` helper for opening the database.
This is a Mako-only convenience. For generic BAS or Xedge targets, replace
`sqlutil.open(dbName)` with explicit `luasql.sqlite()` plus target-specific
database path resolution.

```lua
local sqlutil = require "sqlutil"

local M = {}

local function logDb(message, op)
   if mako and mako.log then
      mako.log(message, op or "sqlite")
   else
     trace(message)
   end
end

local function setBusyTimeout(conn) conn:setbusytimeout(2000) end

local function retryBusy(op, label)
   while true do
      local ok, err = op()
      if ok then
         return ok
      end
      if err ~= "BUSY" then
         return nil, err
      end
      logDb("SQLite BUSY during " .. tostring(label or "write") .. "; retrying")
      ba.sleep(50)
   end
end

function M.openWriteDb(app, dbName)
   local env, conn = sqlutil.open(dbName)
   setBusyTimeout(conn)

   local ok, err = conn:execute("PRAGMA foreign_keys = ON")
   if not ok then
      conn:close()
      env:close()
      error(err)
   end

   ok, err = retryBusy(function()
      return conn:setautocommit("IMMEDIATE")
   end, "setautocommit(IMMEDIATE)")
   if not ok then
      conn:close()
      env:close()
      error(err)
   end

   local dbthread = ba.thread.create()
   local closed = false

   local function commit()
      return retryBusy(function()
         return conn:commit("IMMEDIATE")
      end, "commit(IMMEDIATE)")
   end

   local function rollback()
      pcall(function()
         conn:rollback("IMMEDIATE")
      end)
   end

   local function finishError(err, errCallback)
      rollback()
      if errCallback then
         local cbOk, cbErr = pcall(errCallback, err)
         if not cbOk then
            logDb("SQLite error callback failed: " .. tostring(cbErr))
         end
      else
         logDb("SQLite write failed: " .. tostring(err))
      end
   end

   local function dbExec(sql, noCommit, callback, errCallback)
      if closed then
         error("SQLite writer is closed")
      end

      dbthread:run(function()
         local ok, err = true, nil
         if sql then ok, err = conn:execute(sql) end
         if ok and noCommit ~= true then ok, err = commit() end
         if not ok then
            finishError(err, errCallback)
            return
         end
         if callback then
            local cbOk, cbErr = pcall(callback, conn:lastid())
            if not cbOk then
               logDb("SQLite success callback failed: " .. tostring(cbErr))
            end
         end
      end)
   end

   local function close(callback)
      if closed then return end
      closed = true
      dbthread:run(function()
         pcall(function() conn:close() end)
         pcall(function() env:close() end)
         if callback then pcall(callback) end
      end)
   end

   return {
      exec = dbExec,
      thread = dbthread,
      close = close
   }
end

return M
```

## Mako Server `sqlutil`

When targeting Mako Server, use the bundled `sqlutil` module to simplify
SQLite path handling and short-lived read connections.

```lua
local sqlutil = require "sqlutil"

local env, conn = sqlutil.open("users", "READONLY")
-- read from conn
sqlutil.close(env, conn)
```

`sqlutil.open(name [, options])` opens `name.sqlite.db` in Mako's configured
database directory and returns `env, conn`. The optional `options` argument is
passed to LuaSQL SQLite, for example `"READONLY"` or `"NOCREATE"`.

Use `sqlutil` only as a Mako Server shortcut. Do not treat it as a generic BAS
or Xedge API. It simplifies opening connections, path conventions, and common
read helpers, but it does not create the writer thread, serialize writes,
manage commit batching, or make a persistent connection safe across request
threads. The dedicated writer pattern still owns write coordination.

## Journal Mode And WAL

The dedicated-writer rule applies in every SQLite journal mode. Write-ahead
logging (WAL) does not permit concurrent SQLite writers and must not replace the
single writer thread or its persistent connection.

Do not enable WAL merely because it is commonly recommended for web
applications. WAL can substantially improve a workload with many competing
connections because readers do not normally block the writer. It can also be
slower when the application already serializes short writes through one writer,
uses short-lived readers, or serves many reads from an application cache. WAL
adds a WAL index, checkpoint work, and the `-wal` and `-shm` sidecar files.

Select the journal mode by benchmarking the application on the target operating
system, filesystem, and storage device:

1. Compare the default rollback journal, normally `journal_mode=DELETE`, with
   `journal_mode=WAL` while retaining the same dedicated-writer design.
2. Use the same schema, representative data, request handlers, concurrency,
   write frequency, and cache behavior that the deployed application will use.
3. Warm up the application and run several rounds. Measure request throughput,
   average and tail latency such as p95/p99, write latency, `BUSY` errors, and
   other failures.
4. First compare modes with equivalent durability settings. Test
   `synchronous=NORMAL` separately only if its different durability guarantees
   are acceptable; do not use it merely to make the WAL result faster.
5. Prefer an end-to-end HTTP or application benchmark over a raw SQLite
   microbenchmark. A microbenchmark with many independent writer processes does
   not model an application that already has one writer.

Keep the rollback journal if WAL does not provide a repeatable benefit for the
real workload. If WAL is selected:

- Change journal mode once during controlled startup, before normal traffic,
  and verify that `PRAGMA journal_mode=WAL` returns `wal`.
- Continue routing every write through the dedicated writer thread.
- Use WAL only on a local filesystem that supports SQLite shared memory. Do not
  use it for a database on a network filesystem.
- Plan checkpoint behavior and monitor WAL growth and write latency.
- Do not copy only the main database while it is live. Close and checkpoint it
  cleanly, or preserve the associated WAL state as part of a supported backup
  procedure.

See the SQLite <https://sqlite.org/wal.html> documentation for WAL operation,
limitations, checkpointing, and performance considerations.

## `noCommit` Semantics

`noCommit` is optional. The helper commits unless `noCommit == true`.

```lua
db.exec("INSERT INTO log(message) VALUES('saved')")
```

The call above commits after the insert.

```lua
db.exec("UPDATE Device SET DeviceName='A' WHERE DeviceID=1", true)
db.exec("UPDATE Device SET DeviceIP='192.0.2.10' WHERE DeviceID=1", true)
db.exec("UPDATE Device SET DeviceStatus='online' WHERE DeviceID=1")
```

The first two calls execute without committing. The final call commits the whole
transaction.

This pattern is useful when an API naturally queues separate write operations
but they should be persisted as one transaction.

## Rules For Deferred Commits

- Use `noCommit=true` only for short, DB-only batches.
- Always make the final queued command commit by omitting `noCommit`, passing
  `nil`, or passing `false`.
- Do not leave a transaction open across outbound HTTP calls, `ba.sleep`,
  timers, user interaction, slow filesystem work, heavy computation, or any
  other time-consuming action.
- If any statement in a deferred batch fails, roll back the transaction.
- Keep the API obvious. A reviewer should be able to see where the transaction
  starts deferring commits and where it commits.
- Avoid success callbacks on intermediate `noCommit=true` writes unless the
  callback only queues more DB work. Intermediate writes are not durable until
  the final commit succeeds.

## Dependent Writes

If later writes need the row ID from an earlier insert, chain the final write
from the callback and commit on the last command:

```lua
db.exec(
   "INSERT INTO users(username) VALUES('john')",
   true,
   function(userId)
      db.exec(
         "INSERT INTO sessions(user_id) VALUES(" .. tonumber(userId) .. ")",
         false,
         done,
         failed
      )
   end,
   failed
)
```

The user insert is not committed immediately. The session insert commits both
statements.

If the operation has many dependent statements, consider adding a second helper
that queues one Lua function on the writer thread and commits once when that
function returns. The same rule still applies: one logical operation should
commit once.

## Reads

Reads may use short-lived read connections. They must close cursors,
connections, and environments promptly.

```lua
function M.readAll(app, sql)
   local env, conn = sqlutil.open(app.dbName or "cms")
   setBusyTimeout(conn)
   local cur, err = conn:execute(sql)
   if not cur then
      conn:close()
      env:close()
      error(err)
   end

   local rows = {}
   while true do
      local row = cur:fetch({}, "a")
      if not row then break end
      rows[#rows + 1] = row
   end

   cur:close()
   conn:close()
   env:close()
   return rows
end
```

Do not use read helpers for schema setup, migrations, inserts, updates, or
deletes.

## Request Handling

LSP pages and directory callbacks already run in a BAS request thread. For
ordinary HTTP requests, run short SQLite writes through the dedicated DB writer
thread. Do not create a separate worker thread just to handle the HTTP request.

Use `response:deferred()` only when the response must be completed after the
original request flow returns, for example from a DB-thread callback or a
background worker callback. Do not use the normal request `response` object from
the DB writer thread.

If an operation needs another outbound HTTP call or any time-consuming action:

1. Queue a short DB write if needed, then commit it.
2. Perform the time-consuming action outside the DB writer thread.
3. Queue a second short DB write to store the result.

Never hold a deferred SQLite transaction open while waiting for slow work.

## Schema Changes

Schema setup and migrations are writes. Run them on the writer thread.

For startup schema creation, either queue each statement with `noCommit=true`
and commit at the final statement, or provide a helper that runs the whole schema
function on the writer thread and calls `conn:commit("IMMEDIATE")` once at the
end.

```lua
db.exec("CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY, username TEXT)", true)
db.exec("CREATE TABLE IF NOT EXISTS sessions(id INTEGER PRIMARY KEY, user_id INTEGER)", false)
```

## Checklist

- Create exactly one `ba.thread.create()` writer thread per SQLite database.
- Keep exactly one persistent write connection owned by that thread.
- Use `setautocommit("IMMEDIATE")` on the persistent write connection.
- Commit with `conn:commit("IMMEDIATE")`.
- Roll back with `conn:rollback("IMMEDIATE")` after failed writes.
- Commit by default; suppress commit only when `noCommit == true`.
- Batch multiple writes in one logical operation so they commit once.
- Never open independent write connections in LSP pages, admin callbacks, REST
  handlers, timers, or plugin code.
- Never use the persistent write connection outside the writer thread.
- Never do outbound HTTP calls, sleeps, slow filesystem work, heavy computation,
  or other time-consuming actions on the writer thread.
- Close the persistent write connection on the DB thread during unload/shutdown.
- Use short-lived read connections for reads and close them promptly.
- Retry `BUSY` on the writer connection and sleep briefly between retries.
- Treat WAL as an optional measured optimization, never as a substitute for the
  dedicated writer.
- Benchmark rollback journaling and WAL end to end on the target system before
  selecting a journal mode.

## Anti-Patterns

Do not commit after each statement in one logical operation:

```lua
db.exec("INSERT INTO users(username) VALUES('john')")
db.exec("INSERT INTO sessions(user_id) VALUES(1)")
```

The code above commits twice.

Do this instead:

```lua
db.exec("INSERT INTO users(username) VALUES('john')", true)
db.exec("INSERT INTO sessions(user_id) VALUES(1)")
```

The code above commits once.

Do not defer a commit across time-consuming work:

```lua
db.exec("INSERT INTO jobs(status) VALUES('pending')", true)
local result = timeConsumingCall() -- Wrong: transaction remains open while waiting.
db.exec("UPDATE jobs SET status='done'")
```

Do not open direct write connections from request code:

```lua
local env = luasql.sqlite()
local conn = env:connect("site.db")
conn:execute("UPDATE pages SET title='x' WHERE id=1")
conn:commit()
conn:close()
env:close()
```

All writes must go through the dedicated writer thread API.

## Verification

- Trigger server-side write operations through the relevant page or endpoint and
  confirm no SQLite `BUSY` error is logged or returned in the HTTP response.
- Trigger concurrent write requests and confirm they serialize through the
  writer thread.
- Review multi-write workflows and count commits, not just SQL statements.
- Confirm deferred batches have exactly one final committing call.
- Confirm failed deferred batches roll back.
- Confirm no HTTP call, sleep, slow work, or other time-consuming action runs
  while a commit is deferred.
- If WAL is proposed, record repeatable rollback-journal and WAL results for the
  representative application workload, using equivalent durability settings.
- If WAL is enabled, verify the returned journal mode, checkpoint behavior,
  sidecar-file handling, clean restart, and backup/restore procedure.
