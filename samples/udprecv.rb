#
# rtlbm-mruby mruby script
#
# echo -n "MORIMORI" | nc -w 0 -u 10.10.10.2 7000
#

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

begin

# ip address setting

addr = 10 << 24 | 10 << 16 | 10 << 8 | 2
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 10 << 16 | 10 << 8 | 1
dns = 10 << 24 | 10 << 16 | 10 << 8 | 1

rtl = YABM.new(YABM::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)

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
