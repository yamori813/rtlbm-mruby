#
# rtlbm-mruby mruby script
# GPIO output check script for RTL8196E

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

begin

rtl = YABM.new

rtl.netstartdhcp

# JTAG and all LED is GPIO
rtl.gpiosetsel(0x06, 0x1ffffff, 0x36db, 0x3fff)

# all pin is gpio
reg = 0
rtl.gpiosetctl(reg)

#all pin is out
reg = 0xffff
rtl.gpiosetdir(reg)

rtl.print "GPIO out check\n"

i = 0
while 1 do
  rtl.print i.to_s + "\n"
  if i < 16 
    reg = 1 << i
  else
    reg = ~(1 << (i - 16))
  end
  rtl.gpiosetdat(reg)
  delay(rtl, 2000)
  i = i + 1
  if i == 32
    i = 0
  end
end

rescue => e
  rtl.print e.to_s
end

