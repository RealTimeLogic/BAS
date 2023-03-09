<?lsp

-- WebSocket SMQ Example
-- SMQ Doc: https://realtimelogic.com/ba/doc/?url=SMQ.html

if page.smq then
   if require"smq.hub".isSMQ(request) then
       -- Upgrade HTTP(S) request to SMQ connection
      page.smq:connect(request)
      return
   else -- Not an SMQ client
      print"Example already installed!"
   end
else
   -- The following code would normally be put in a .preload/.config
   -- script.
   page.smq = require"smq.hub".create()
   print"SMQ broker installed."
end
response:setcontenttype"text/plain"
?>

See README.md for how to connect SMQ clients.

