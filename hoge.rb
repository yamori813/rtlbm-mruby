#
# rtlbm-mruby mruby script
# used mruby-rtlbm-hsc gem
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_HOMESPOTCUBE)
hsc = HomeSpotCube.new()

addr = 10 << 24 | 10 << 16 | 10 << 8 | 2
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 0 << 16 | 1 << 8 | 1
dns = 10 << 24 | 0 << 16 | 1 << 8 | 1

rtl.netstart(addr, mask, gw, dns)

rtl.udpbind(7000)

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
