#
# mruby on YABM script
# MZK-MF300N GPIO test script
#

POWER_LED = (1 << 10)
SYSTEM_LED = (1 << 6)
WPS_LED = (1 << 4)
ETHERNET_LED = (1 << 14)

WPS_BUTTON = (1 << 3)
RESET_BUTTON = (1 << 5)
SLIDE1_SWITCH = (1 << 2)
SLIDE2_SWITCH = (1 << 11)

def dispin(yabm, val) 
  if val & WPS_BUTTON == 0 then
    yabm.print "0"
  else
    yabm.print "1"
  end
  if val & RESET_BUTTON == 0 then
    yabm.print "0"
  else
    yabm.print "1"
  end
  if val & SLIDE1_SWITCH == 0 then
    yabm.print "0"
  else
    yabm.print "1"
  end
  if val & SLIDE2_SWITCH == 0 then
    yabm.print "0"
  else
    yabm.print "1"
  end
end

begin

yabm = YABM.new

# JTAG and LED is GPIO
yabm.gpiosetsel(0x3000ff, 0x3000ff, 0x0, 0x0)

# all pin is gpio
reg = yabm.gpiogetctl()
reg = reg & ~POWER_LED
yabm.print "ctl: " + reg.to_s + "\n"
yabm.gpiosetctl(reg)

reg = yabm.gpiogetdir()
yabm.print "dir: " + reg.to_s + "\n"
reg = reg | POWER_LED
yabm.gpiosetdir(reg)

val = yabm.gpiogetdat()
val = val | WPS_LED & ~POWER_LED
yabm.gpiosetdat(val)

i = 0
loop do
  val = yabm.gpiogetdat()
  yabm.print "in:"
  dispin(yabm, val)
  yabm.print " "
  val = val & ~SYSTEM_LED
  yabm.gpiosetdat(val)
  yabm.msleep(500)
  val = val | SYSTEM_LED
  yabm.gpiosetdat(val)
  yabm.msleep(500)
end

rescue => e
  yabm.print e.to_s
end

