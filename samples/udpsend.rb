#
# rtlbm-mruby mruby script
#

begin

# ip address setting

addr = 10 << 24 | 10 << 16 | 10 << 8 | 2
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 10 << 16 | 10 << 8 | 3
dns = 10 << 24 | 10 << 16 | 10 << 8 | 1

dist = 10 << 24 | 10 << 16 | 10 << 8 | 3

rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit

while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 100 do
   end
   rtl.udpsend(dist, 514, "Hello", 5)
end

rescue => e
  rtl.print e.to_s
end
