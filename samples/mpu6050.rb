#
# mruby on YABM script
#
# MPU-6050 sample code on HomeSpotCube
#

# Homespotcube GPIO

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)

# GPIO I2C Pin (SW12)

SCL = 2
SDA = 11

def delay(yabm, val) 
  start = yabm.count() 
  while yabm.count() < start + val do
  end
end

def pointstr(p, c)
  if p == 0 then
    return "0." + "0" * c
  elsif p.abs < 10 ** c
    l = c - p.abs.to_s.length + 1
    s = p.to_s.insert(p < 0 ? 1 : 0, "0" * l)
    return s.insert(-1 - c, ".")
  else
    return p.to_s.insert(-1 - c, ".")
  end
end

def gpioinit(yabm) 
  yabm.gpiosetsel(0x003c300c, 0x003c300c, 0x00001800, 0x00001800)

  reg = yabm.gpiogetctl()
  reg = reg & ~(STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
  reg = reg & ~(TOP_LED1 | TOP_LED2 | TOP_LED3)
  reg = reg & ~(TOP_BUTTON)
  yabm.gpiosetctl(reg)

  reg = yabm.gpiogetdir()
  reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
  reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
  reg = reg & ~(TOP_BUTTON)
  yabm.gpiosetdir(reg)

  reg = yabm.gpiogetdat()
  reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
  reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
  yabm.gpiosetdat(reg)
end

def s16(v)
  if v > 0x8000 then
    return v - 0x10000
  else
    return v
  end
end

# main

MPUADDR = 0x68

begin

  yabm = YABM.new

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 1)

  whoiam = yabm.i2cread(MPUADDR, 117)

  yabm.print "Who Am I: " + whoiam.to_s + "\r\n"

# start 
  yabm.i2cwrite(MPUADDR, 0x6B, 0)

  while 1 do

    ax = s16((yabm.i2cread(MPUADDR, 0x3b) << 8) | yabm.i2cread(MPUADDR, 0x3c))
    ay = s16((yabm.i2cread(MPUADDR, 0x3d) << 8) | yabm.i2cread(MPUADDR, 0x3e))
    az = s16((yabm.i2cread(MPUADDR, 0x3f) << 8) | yabm.i2cread(MPUADDR, 0x40))
    yabm.print ax.to_s + ":" + ay.to_s + ":" + az.to_s + ":"
    gx = s16((yabm.i2cread(MPUADDR, 0x43) << 8) | yabm.i2cread(MPUADDR, 0x44))
    gy = s16((yabm.i2cread(MPUADDR, 0x45) << 8) | yabm.i2cread(MPUADDR, 0x46))
    gz = s16((yabm.i2cread(MPUADDR, 0x47) << 8) | yabm.i2cread(MPUADDR, 0x48))
    yabm.print gx.to_s + ":" + gy.to_s + ":" + gz.to_s + "\r\n"

    if ay < -16000 then
      curled = TOP_LED3
    elsif ay > 16000 then
      curled = TOP_LED2
    elsif az > 16000 then
      if yabm.gpiogetdat() & TOP_BUTTON == TOP_BUTTON then
        curled = STATUS_LED1
      else
        curled = STATUS_LED2
      end
    else
      curled = TOP_LED1
    end

    reg = yabm.gpiogetdat()
    reg = reg & ~curled
    yabm.gpiosetdat(reg)

    delay(yabm, 2000)

    reg = yabm.gpiogetdat()
    reg = reg | curled
    yabm.gpiosetdat(reg)

    delay(yabm, 1000)

  end

rescue => e
  yabm.print e.to_s
end
