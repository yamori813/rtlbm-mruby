#
# rtlbm-mruby mruby script
#
# This is demonstration for BH1750 
# Used BBR-4MG V2

# i2c pin

# TRST# (1) has internal pull-up resistor
I2CSCK = 3
# TDI (3) has internal pull-up resistor
I2CSDA = 5

# i2c BH1750 address

BHADDR = 0x23

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

class BH1750
  @mtreg = 69

  # Measurement at 1 lux resolution. Measurement time is approx 120ms.
  CONTINUOUS_HIGH_RES_MODE  = 0x10
  # Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
  CONTINUOUS_HIGH_RES_MODE_2 = 0x11
  # Measurement at 4 lux resolution. Measurement time is approx 16ms.
  CONTINUOUS_LOW_RES_MODE = 0x13
  # Measurement at 1 lux resolution. Measurement time is approx 120ms.
  ONE_TIME_HIGH_RES_MODE = 0x20
  # Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
  ONE_TIME_HIGH_RES_MODE_2 = 0x21
  # Measurement at 4 lux resolution. Measurement time is approx 16ms.
  ONE_TIME_LOW_RES_MODE = 0x23

  def initialize yabm, addr
    @y = yabm
    @addr = addr
  end

  def setMTreg mtreg
    @mtreg = mtreg
    @y.i2cwrite(@addr, [0x40 | (@mtreg >> 5)])
    @y.msleep(200)
    @y.i2cwrite(@addr, [0x60 | (@mtreg & 0x1f)])
    @y.msleep(200)
  end

  def setMeasurement mode
    @meas = mode
    if @meas == CONTINUOUS_HIGH_RES_MODE ||
      @meas == CONTINUOUS_HIGH_RES_MODE_2 ||
      @meas == CONTINUOUS_LOW_RES_MODE then
      @y.i2cwrite(@addr, [@meas])
    end
  end

  def getLightLevel
    if @meas == ONE_TIME_HIGH_RES_MODE ||
      @meas == ONE_TIME_HIGH_RES_MODE_2 then
      @y.i2cwrite(@addr, [@meas])
      @y.msleep(120 * @mtreg / 69)
    elsif @meas == ONE_TIME_LOW_RES_MODE then
      @y.i2cwrite(@addr, [@meas])
      @y.msleep(16 * @mtreg / 69)
    end
    bharr = @y.i2cread(@addr, 2)
    val = (bharr[0] << 8) | bharr[1]
    if @meas == CONTINUOUS_HIGH_RES_MODE_2 ||
      @meas == ONE_TIME_HIGH_RES_MODE_2 then
      lx = val * 50 * 69 * 5 / (6 * @mtreg)
    else
      lx = val * 100 * 69 * 5 / (6 * @mtreg)
    end
    return lx
  end
end

begin

yabm = YABM.new

# use gpio pin
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

gpio = yabm.gpiogetdat
yabm.gpiosetdat(gpio | (1 << 16) | 0x7c00)

yabm.i2cinit(I2CSCK, I2CSDA, 1)

bh = BH1750.new(yabm, BHADDR)

interval = 20

count = 0

bh.setMTreg(254)
bh.setMeasurement(BH1750::ONE_TIME_HIGH_RES_MODE_2)

loop do
  lx = bh.getLightLevel
  yabm.print count.to_s + " "
  yabm.print pointstr(lx, 2) + "\r\n"
  yabm.msleep(1000 * interval)
  count += 1
end

rescue => e
  yabm.print e.to_s
end
