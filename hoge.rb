#
# rtlbm-mruby mruby script
# used mruby-rtlbm-hsc gem
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

begin
rtl = RTL8196C.new("")
hcount = 0
count = 0
res = SimpleHttp.new("http", "www.yahoo.co.jp", 80).request("GET", "/", {'User-Agent' => "test-agent"})
rtl.print res.body
while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 50 do
   end
   count = count + 1
   if count % 10 == 0 then
   hcount = hcount + 1
   rtl.print " " + hcount.to_s + "\r\n"
   res = SimpleHttp.new("https", "10.0.1.37", 8080).request("GET", "/", {'User-Agent' => "test-agent"})
    if res.body != nil then
      rtl.print res.body
    end
   end
end

hsc = HomeSpotCube.new()
i = 0
while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 50 do
   end
   udpstr = rtl.udprecv()
   if udpstr.length != 0 then
     if udpstr == "GREEN" then
       hsc.led(2)
     elsif udpstr == "RED" then
       hsc.led(1)
     elsif udpstr == "ORANGE" then
       hsc.led(3)
     elsif udpstr == "OFF" then
       hsc.led(0)
     end
     rtl.print udpstr
   end
   i = i + 1
end

rescue => e
  rtl.print e.to_s
end
