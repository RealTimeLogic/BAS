<?lsp

--[[ 
The following example fetches the time from a time server using the
Time Protocol: https://en.wikipedia.org/wiki/Time_Protocol

We use the TCP time protocol and not UDP since not all Barracuda App
Server porting layers support UDP.

We send the time to an online ASCII art REST service and then download
the converted ASCII art time. The time is sent to the browser as
plain text (no HTML).

Socket API: https://realtimelogic.com/ba/doc/?url=auxlua.html#socket
HTTP client API: https://realtimelogic.com/ba/doc/?url=auxlua.html#httpc
How to use the socket API: https://realtimelogic.com/ba/doc/?url=SockLib.html
--]]


local function getTime()
   local s,err = ba.socket.connect("time.nist.gov",37)
   if s then
      local d,err=s:read(5000)
      s:close()
      if d and #d == 4 then
         local secs = ba.socket.n2h(4,d)
         secs = secs - 2208988800 -- Convert from 1900 to 1970 format
         local t=os.date("*t",secs)
         return string.format("%02d:%02d:%02d",t.hour,t.min,t.sec)
      end
   end
   return "NIST offline"
end

local http = require"http".create()
http:request{
   url="http://artii.herokuapp.com/make",
   query = {
      text=getTime(),
      font="epic"
   }
}

response:setcontenttype"text/plain"
print(http:read"*a")
?>
