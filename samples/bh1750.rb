#
# rtlbm-mruby mruby script
#
# This is demonstration for BH1750 
# Used BBR-4MG V2

APIKEY = "naisyo"

# i2c pin

# TRST# (1) has internal pull-up resistor
I2CSCK = 3
# TDI (3) has internal pull-up resistor
I2CSDA = 5

# i2c BH1750 address

BHADDR = 0x23

def delay(yabm, val) 
  start = yabm.count() 
  while yabm.count() < start + val do
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

  def init yabm, addr
    @y = yabm
    @addr = addr
  end

  def setMTreg mtreg
    @mtreg = mtreg
    @y.i2cwrites(@addr, [0x40 | (@mtreg >> 5)], 0)
    delay(@y, 200)
    @y.i2cwrites(@addr, [0x60 | (@mtreg & 0x1f)], 0)
    delay(@y, 200)
  end

  def setMeasurement mode
    @meas = mode
    if @meas == CONTINUOUS_HIGH_RES_MODE ||
      @meas == CONTINUOUS_HIGH_RES_MODE_2 ||
      @meas == CONTINUOUS_LOW_RES_MODE then
      @y.i2cwrites(@addr, [@meas], 0)
    end
  end

  def getLightLevel
    if @meas == ONE_TIME_HIGH_RES_MODE ||
      @meas == ONE_TIME_HIGH_RES_MODE_2 then
      @y.i2cwrites(@addr, [@meas], 0)
      delay(@y, 120 * @mtreg / 69)
    elsif @meas == ONE_TIME_LOW_RES_MODE then
      @y.i2cwrites(@addr, [@meas], 0)
      delay(@y, 16 * @mtreg / 69)
    end
    bharr = @y.i2creads(@addr, 2)
    val = (bharr[0] << 8) | bharr[1]
    if @meas == CONTINUOUS_HIGH_RES_MODE_2 ||
      @meas == ONE_TIME_HIGH_RES_MODE_2 then
      lx = (val * 50 * 69 * 5 / (6 * @mtreg)).round
    else
      lx = (val * 100 * 69 * 5 / (6 * @mtreg)).round
    end
    if lx == 0 then
      lxstr = "0"
    elsif lx < 100 then
      lxstr = "0." + lx.to_s
    else
      lxstr = lx.to_s.insert(-3, ".")
    end
    return lxstr
  end
end

begin

yabm = YABM.new

yabm.netstartdhcp

yabm.print yabm.getaddress + "\n"

ntpaddr = yabm.lookup("ntp.nict.jp")
yabm.sntp(ntpaddr)

# use gpio pin
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

gpio = yabm.gpiogetdat
yabm.gpiosetdat(gpio | (1 << 16) | 0x7c00)

yabm.i2cinit(I2CSCK, I2CSDA, 1)

bh = BH1750.new

bh.init(yabm, BHADDR)

mtreg = 254
mode = BH1750::CONTINUOUS_HIGH_RES_MODE_2

interval = 20

bh.setMTreg(mtreg)

bh.setMeasurement(mode)

count = 0

while 1 do
  delay(yabm, 1000 * interval)
  lxstr = bh.getLightLevel
  para = "api_key=" + APIKEY + "&field1=" + count.to_s + "&field2=" + lxstr
  res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?" + para, {'User-Agent' => "test-agent"})
  if res != nil && res.status.to_s.length != 0 then
    yabm.print count.to_s + " " + lxstr + " " + res.status.to_s + "\r\n"
  end
  count = count + 1
end

rescue => e
  yabm.print e.to_s
end
