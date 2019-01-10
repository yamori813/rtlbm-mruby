#
# rtlbm-mruby mruby script
# GPIO input check script for RTL8196E

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

begin

rtl = YABM.new

# JTAG and all LED is GPIO
rtl.gpiosetsel(0x06, 0x1ffffff, 0x36db, 0x3fff)

# all pin is gpio
reg = 0
rtl.gpiosetctl(reg)

#all pin is in
reg = 0
rtl.gpiosetdir(reg)

rtl.print "GPIO input check\n"

i = 0
while 1 do
  val = rtl.gpiogetdat()
  rtl.print val.to_s + "\n"
  delay(rtl, 2000)
end

rescue => e
  rtl.print e.to_s
end

