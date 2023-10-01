#
# rtlbm-mruby mruby script
#
# echo -n "MORIMORI" | nc -w 0 -u 10.10.10.2 7000
#

begin

# ip address setting

addr = "10.10.10.2"
mask = "255.255.255.0"
gw = "10.10.10.1"
dns = "10.10.10.1"

rtl = YABM.new

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit
rtl.udpbind(7000)

i = 0
loop do
  rtl.print "."
  rtl.msleep(500)
  udpstr = rtl.udprecv()
  if udpstr.length != 0 then
    rtl.print udpstr
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
