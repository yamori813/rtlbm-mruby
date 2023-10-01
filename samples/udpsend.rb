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

rtl = YABM.new

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit

loop do
   rtl.print "."
   rtl.msleep 1_000
   rtl.udpsend(dist, 514, "Hello", 5)
end

rescue => e
  rtl.print e.to_s
end
