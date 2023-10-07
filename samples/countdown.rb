#
# LED countdown test on BBR
#

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~RLED)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | RLED)
end

def blink yabm
  ledon yabm
  yabm.msleep 100
  ledoff yabm
end

yabm = YABM.new

# JTAG(GPIOA2,4,5,6) and LED_PORT3(GPIOB5) is GPIO
#yabm.gpiosetsel(0x06, 0x1ffffff, 0x600, 0x3fff)
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

RLED = (1 << 16)

interval = [5, 4, 3, 2, 2, 1, 1, 1]

reg = yabm.gpiogetdat()
ledoff yabm

str = "mruby on yet another bare metal"

loop do
  sum = 0
  interval.each do |n|
    yabm.msleep n * 1000 - 100
    blink yabm
    yabm.print "."
    sum += n
  end
  yabm.msleep 1000
  sum += 1
  yabm.print sum.to_s
# do something
end

