#
# rtlbm-mruby mruby script
#
# This is demonstration for BH1750 
# Used BBR-4MG V2

APIKEY = "naisyo"

# i2c pin

# TRST# (1) internal pull-up resistor
I2CSCK = 3
# TDI (3) internal pull-up resistor
I2CSDA = 5

# i2c BH1750 address

BHADDR = 0x23

def delay(yabm, val) 
  start = yabm.count() 
  while yabm.count() < start + val do
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

mtreg = 254

yabm.i2cwrites(BHADDR, [0x40 | (mtreg >> 5)], 0)
delay(yabm, 200)
yabm.i2cwrites(BHADDR, [0x60 | (mtreg & 0x1f)], 0)
delay(yabm, 200)
yabm.i2cwrites(BHADDR, [0x11], 0)
delay(yabm, 200)

count = 0
while 1 do
  yabm.print "."
  bharr = yabm.i2creads(BHADDR, 2)
  val = (bharr[0] << 8) | bharr[1]
  lx = (val * 50 * 69 * 5 / (6 * mtreg)).round
  if lx == 0 then
    lxstr = "0"
  elsif lx < 100 then
    lxstr = "0." + lx.to_s
  else
    lxstr = lx.to_s.insert(-3, ".")
  end
  para = "api_key=" + APIKEY + "&field1=" + count.to_s + "&field2=" + lxstr
  res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?" + para, {'User-Agent' => "test-agent"})
  if res != nil && res.status.to_s.length != 0 then
    yabm.print count.to_s + " " + lxstr + " " + res.status.to_s + "\r\n"
  end
  count = count + 1
  delay(yabm, 1000 * 20)
end

rescue => e
  yabm.print e.to_s
end
