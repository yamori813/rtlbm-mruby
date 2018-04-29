#
# rtlbm-mruby mruby script
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

i = 0
while 1 do
  rtl.print "."
  start = rtl.count() 
  while rtl.count() < start + 500 do
  end
  uartstr = rtl.readuart()
  if uartstr.length != 0 then
    rtl.print uartstr
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
