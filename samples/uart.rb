#
# rtlbm-mruby mruby script
#
# uart echo back sample script
# baudrate is 38400
#

begin

rtl = YABM.new

i = 0
loop do
  yabm.msleep 500
  uartstr = rtl.readuart()
  if uartstr.length != 0 then
    rtl.print uartstr
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
