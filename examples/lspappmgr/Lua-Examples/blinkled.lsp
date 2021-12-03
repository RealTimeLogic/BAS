<?lsp

--[[
This LSP page creates a timer coroutine that controls the LED via the
Lua bindings in led.c. The timer object is free-floating since we are
not referencing (anchoring) the object. The created timer(s) will
eventually be collected by the Lua garbage collector. You can speed
up the collecting by calling the Lua function: collectgarbage()

Timer documentation:
https://realtimelogic.com/ba/doc/?url=lua.html#ba_timer
--]]

response:setcontenttype"text/plain" -- As simple as it can get

if request:data"gc" then -- Trigger by adding the following to the URL: ?gc=
   local before=collectgarbage"count"
   collectgarbage()
   local after = collectgarbage"count"
   print(string.format("KBytes in use: %d\nCollected %d KBytes",
                       after,
                       before - after))
   return
end

local function blink()
   while true do
      trace"blink"
      LED.setLed(true)
      coroutine.yield(true) -- Sleep
      LED.setLed(false)
      coroutine.yield(true) -- Sleep
   end
end
ba.timer(blink):set(1000)
?>


   ____  ___       __      __    __________   
   / __ )/ (_)___  / /__   / /   / ____/ __ \  
  / __  / / / __ \/ //_/  / /   / __/ / / / /  
 / /_/ / / / / / / ,<    / /___/ /___/ /_/ /   
/_________/_/ /_/_/|_|  /_____/___________/    
  / ____/___  _________  __  __/ /_(_)___  ___ 
 / /   / __ \/ ___/ __ \/ / / / __/ / __ \/ _ \
/ /___/ /_/ / /  / /_/ / /_/ / /_/ / / / /  __/
\____/\____/_/   \____/\__,_/\__/_/_/ /_/\___/ 
                                               


This LSP page creates a new blinking LED timer coroutine each time it runs.
Click the refresh button to create a new.
Each timer runs until the garbage collector kicks in.
You may run the collector by adding ?gc= to the URL
