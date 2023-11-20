#
# mruby on YABM script
#
# MPU-6050 sample code on HomeSpotCube
# need sub_hsc.rb
#

# GPIO I2C Pin (SW12)

SCL = 2
SDA = 11

def s16(v)
  if v > 0x8000 then
    v - 0x10000
  else
    v
  end
end

# main

MPUADDR = 0x68

begin

  yabm = YABM.new

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 1)

  whoiam = yabm.i2cread(MPUADDR, 1, 117)

  yabm.print "Who Am I: " + whoiam.to_s + "\r\n"

# start 
  yabm.i2cwrite(MPUADDR, 0x6B, 0)

  loop do

    ax = s16((yabm.i2cread(MPUADDR, 1, 0x3b) << 8) | yabm.i2cread(MPUADDR, 1, 0x3c))
    ay = s16((yabm.i2cread(MPUADDR, 1, 0x3d) << 8) | yabm.i2cread(MPUADDR, 1, 0x3e))
    az = s16((yabm.i2cread(MPUADDR, 1, 0x3f) << 8) | yabm.i2cread(MPUADDR, 1, 0x40))
    yabm.print ax.to_s + ":" + ay.to_s + ":" + az.to_s + ":"
    gx = s16((yabm.i2cread(MPUADDR, 1, 0x43) << 8) | yabm.i2cread(MPUADDR, 1, 0x44))
    gy = s16((yabm.i2cread(MPUADDR, 1, 0x45) << 8) | yabm.i2cread(MPUADDR, 1, 0x46))
    gz = s16((yabm.i2cread(MPUADDR, 1, 0x47) << 8) | yabm.i2cread(MPUADDR, 1, 0x48))
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

    yabm.msleep(2_000)

    reg = yabm.gpiogetdat()
    reg = reg | curled
    yabm.gpiosetdat(reg)

    yabm.msleep(1_000)

  end

rescue => e
  yabm.print e.to_s
end
