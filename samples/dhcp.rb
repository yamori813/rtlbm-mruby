#
# rtlbm-mruby mruby script
#
# echo -n "MORIMORI" | nc -w 0 -u <ip address> 7000
#

begin

rtl = YABM.new

rtl.netstartdhcp

ip = rtl.getaddress

rtl.print ip + "\n"

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
