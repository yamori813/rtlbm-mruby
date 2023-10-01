#
# rtlbm-mruby mruby script
#

begin

rtl = YABM.new

rtl.gpiosetsel(0x3003ff, 0x3003ff, 0, 0)

reg = rtl.gpiogetctl()
reg = reg & ~0x7c7c
rtl.gpiosetctl(reg)

reg = rtl.gpiogetdir()
reg = reg | 0x7c7c
rtl.gpiosetdir(reg)

reg = rtl.gpiogetdat()
reg = reg | 0x7c7c
rtl.gpiosetdat(reg)

i = 0
loop do
  reg = rtl.gpiogetdat()
  val = i % 9
  if val > 4 then
    reg = (reg | 0x7c7c) & ~(1 << (9 - val + 0xa))
  else
    reg = (reg | 0x7c7c) & ~(1 << (val + 0xa))
  end
  rtl.gpiosetdat(reg)
  rtl.msleep(100)
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end

