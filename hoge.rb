#
# rtlbm-mruby mruby script
# used mruby-rtlbm-hsc gem
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

o = Object.new
hsc = HomeSpotCube.new()
i = 0
while 1 do
   o.myputs "index:" + i.to_s
   start = hsc.count() 
   while hsc.count() < start + 50 do
   end
   udpstr = hsc.udprecv()
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
   end
   i = i + 1
end
