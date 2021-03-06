#
# rtlbm-mruby mruby script
#
# echo -n "MORIMORI" | nc -w 0 -u <ip address> 7000
#

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

begin

rtl = YABM.new

rtl.netstartdhcp

ip = rtl.getaddress

rtl.print ip + "\n"

rtl.udpinit
rtl.udpbind(7000)

i = 0
while 1 do
  rtl.print "."
  delay(rtl, 500)
  udpstr = rtl.udprecv()
  if udpstr.length != 0 then
    rtl.print udpstr
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
