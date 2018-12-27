#
# rtlbm-mruby mruby script
#

begin

# ip address setting

addr = "10.10.10.2"
mask = "255.255.255.0"
gw = "10.10.10.1"
dns = "10.10.10.1"

dist = "10.10.10.3"

rtl = YABM.new(YABM::MODULE_RTL8196C)

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit

while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 1000 do
   end
   rtl.udpsend(dist, 514, "Hello", 5)
end

rescue => e
  rtl.print e.to_s
end
