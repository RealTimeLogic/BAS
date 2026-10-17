# GPIO and Hardware Lua Bindings

Use this companion to [Lua-Binding-Skill.md](upstream/Lua-Binding-Skill.md)
when implementing or reviewing native GPIO, ADC, PWM, pulse counter, RMT,
I2C, SPI, UART, camera, or other peripheral bindings. Keep the existing BAS
metatable, uservalue, error, dispatcher-mutex, and `LThreadMgr` conventions.
Verify driver contracts in the selected SDK's headers and implementation;
do not assume another chip, SDK version, or peripheral has the same rules.

Apply only the mechanisms the peripheral needs. Prefer a small ownership
flag, busy guard, or event identity check over a new general framework.
Preserve deliberate compact argument parsing unless a demonstrated correctness
issue requires changing it; measure code and static RAM when size matters.

## Interrupts and Event Delivery

- Keep interrupt handlers bounded and use only operations documented as
  ISR-safe. Queue plain native event data to a safe broker/task context;
  acquire the BAS dispatcher mutex there and deliver Lua callbacks through
  `LThreadMgr`. Never call Lua or lock the BAS mutex from an ISR.
- Define queue-full behavior: drop, coalesce, count overflow, or recover as
  required by the API. Check enqueue results. Publish pending/running state
  in an order safe for concurrent consumers, and undo it on enqueue failure;
  a failed notification must not permanently suppress later events.
- A lost completion event can strand a busy flag or retained buffer. Give
  completion delivery a recovery path, not merely the drop policy used for
  optional input notifications.
- Distinguish captured event values and timestamps from values sampled later
  by a worker. Document coalescing, overflow, and ordering. Lua callback
  scheduling does not provide precise pulse timing or lossless edge capture;
  use hardware capture/counting when the application requires it.

## Resource Ownership and Partial Construction

- Validate pin capability, direction, target limits, and known board-reserved
  pins before any reset or mux change. Include SDK/system-owned flash, PSRAM,
  and peripheral pins; prefer actual SDK reservations over duplicated module
  pin maps. A valid GPIO number is not necessarily available, and a driver
  conflict warning may still allow reconfiguration. Track pins, buses, channels,
  shared timers, and installed driver services at the scope where they are
  actually shared. Do not reconfigure another live object's resources.
- Make construction recoverable at every failure point, including Lua
  allocation failure after native allocation. Establish cleanup ownership
  before acquiring resources, or provide rollback for resources not yet
  owned by userdata. Publish the object only when initialization succeeds.
- Cleanup must release only resources that this instance acquired. A failed
  constructor must not clear another object's pin registration, delete a
  shared bus, or decrement a service count it never incremented.
- Make explicit close and GC safe after partial initialization and repeated
  close. Stop event production and resolve outstanding operations before
  freeing state. A driver stop/cancel failure is not proof that callbacks,
  DMA, or hardware access have stopped.

## Queued Events and Userdata Lifetime

- A pin number or reused driver slot alone does not identify an object.
  Prevent old events from reaching a replacement object, especially when a
  different peripheral type reuses the pin. Use a validated instance/generation
  identity or prove that old events are drained/cancelled before reuse.
  Do not dereference a potentially freed pointer to validate its identity.
- A callback stored in a userdata uservalue keeps the callback reachable
  while the userdata is reachable; it does not itself root the userdata.
  Weak references and raw C pointers also do not keep the owner alive.
  Ensure queued jobs either retain the necessary owner or safely detect its
  disappearance without touching freed state. Prefer `balua_wkRef` for owner
  lookup and an existing native ownership registry for recovery; add a second
  table/list only when its separate lifetime or lookup role is necessary.
  Check missing weak values, and prevent reference-slot reuse from redirecting
  old jobs to a new object.
- Protect required userdata and buffers across callback execution and Lua
  allocations. Callbacks can close the object, trigger GC, or start another
  operation. After `lua_pcall`, recheck state before further native access.
  Global GC suppression is not a substitute for correct ownership.
- Release temporary strong references on completion, cancellation, and
  failure. Avoid permanent self-roots that make the documented GC cleanup
  unreachable. Define what callback errors do to the producer and queued work.

## Blocking Operations, Buffers, and Concurrency

- Release the BAS dispatcher mutex around blocking driver calls as described
  in the main binding guide, but establish any busy guard and required object
  or buffer references first. Reacquire the mutex before using Lua. Define
  whether concurrent read, write, reconfiguration, and close are permitted
  by the driver; reject or serialize unsafe combinations.
- The BAS mutex protects Lua access, not state accessed independently by an
  ISR or another core. Use the platform's appropriate synchronization for
  shared native state; `volatile` alone does not provide synchronization.
  A naturally atomic Boolean load/store does not by itself establish ordering
  or make a read-modify-write operation safe.
  Preserve task/core affinity where the driver requires it. Do not wait for
  a worker while holding a lock that worker needs to finish.
- Check where interrupts are allocated, including constructors and callback
  registration: shared interrupt groups may require one CPU even when later
  I/O is thread-safe. Reuse an existing task only if its affinity, locks, and
  ownership contracts fit; a small temporary pinned task can be safer and
  smaller than changing a shared broker.
- Check whether a driver copies a buffer or retains its address after the
  call returns. For asynchronous operations, keep the buffer alive until
  completion or confirmed cancellation, including during close and GC.
  Respect target-specific DMA alignment, memory-capability, and cache rules.
- Every success and failure path must release busy state and temporary
  resources at the appropriate time. Returning from the submission function
  does not mean an asynchronous operation has finished.

## Arguments, Units, and Errors

- Validate required Lua types, integer ranges, supported enum values, and length arithmetic
  before narrowing or converting to unsigned native types. Do not let a
  missing/non-numeric argument silently become pin zero, or accept an enum
  sentinel as a usable hardware instance.
- Verify each driver's units: milliseconds, RTOS ticks, peripheral ticks,
  nanoseconds, frequency, and byte/sample counts are distinct. Preserve
  documented timeout sentinels and check conversion overflow and rounding.
  For example, an API accepting milliseconds must not receive a value divided
  by `portTICK_PERIOD_MS` simply because another API accepts RTOS ticks.
- Inspect driver assertions and derived limits, not just public argument
  types. Check finite/ranged floating-point values before integer conversion
  and calculated dividers/counts against hardware limits using the actual
  selected clock. Keep checks inside the driver when only it knows that state;
  avoid duplicating its clock-selection logic in the binding.
- Identify SDK workarounds explicitly: affected versions, failure prevented,
  patch location, upgrade checks, and removal condition. Prefer build-local
  copies over changing an installed SDK. Stop on a mismatched patch fragment;
  a matching fragment alone does not prove a new SDK is compatible.
- Check calibration fields against the selected target: unit, channel,
  attenuation, and bit width may all matter. An omitted field initialized to
  zero can silently select the wrong channel's compensation.
- Follow the skill's misuse-versus-operational-failure contract. Missing
  peripherals, transfer failures, and timeouts must remain recoverable;
  do not turn them into firmware aborts. Preserve established API behavior
  unless its change is authorized.

## Focused Validation

Choose tests for the changed lifecycle and peripheral, including relevant
cases from this list:

- burst input and queue saturation, followed by successful later delivery;
- callback error, callback-triggered close/reentry, and GC during pending work;
- close/reopen on the same pin, including reuse by a different peripheral;
- concurrent blocking I/O and close/reconfiguration;
- partial initialization failure and attempts to acquire an occupied resource;
- multiple interrupt-using instances created across worker tasks/cores;
- asynchronous completion/cancellation failure and buffer lifetime;
- missing hardware, timeout recovery, and server responsiveness during I/O;
- invalid input or partial construction followed by valid reuse of the same
  resource, confirming that busy flags and temporary references were released;
- configuration save/commit failures reaching Lua without a success report or
  reboot, plus save/load round trips using distinct values for adjacent fields.

Document helper inputs, return/stack effects, mutex and ownership contracts
as described in the binding guide. Explain why synchronization is needed and
what published state it protects. Preserve compact parsing; add only checks
needed for the actual contract.

Compile affected supported targets when shared native code changes. Report
compilation, host tests, physical peripheral operation, timing behavior, and
network/server responsiveness separately. A build or successful HTTP response
does not validate GPIO timing, DMA safety, or a connected peripheral.
