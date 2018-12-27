#
# rtlbm-mruby mruby script
#
# uart echo back sample script
# baudrate is 38400
#

begin

rtl = YABM.new(YABM::MODULE_RTL8196C)

i = 0
while 1 do
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
