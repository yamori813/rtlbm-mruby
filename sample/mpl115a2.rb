#
# rtlbm-mruby mruby script
#
# get from i2c mpl115a2 pressure data to update thingspeak channel
#

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

# This calculate code is based c source code in NXP AN3785 document

def calculatePCompLong(padc, tadc, a0, b1, b2, c12)
  if a0 >= 0x8000 then
    a0 = a0 - 0x10000
  end
  if b1 >= 0x8000 then
    b1 = b1 - 0x10000
  end
  if b2 >= 0x8000 then
    b2 = b2 - 0x10000
  end
  if c12 >= 0x8000 then
    c12 = c12 - 0x10000
  end
  padc = padc >> 6
  tadc = tadc >> 6
# ******* STEP 1 : c12x2 = c12 * Tadc
  lt1 = c12
  lt2 = tadc
  lt3 = lt1 * lt2
  c12x2 = lt3 >> 11
# ******* STEP 2 : a1 = b1 + c12x2
  lt1 = b1
  lt2 = c12x2
  lt3 = lt1 + lt2
  a1 = lt3
# ******* STEP 3 : a1x1 = a1 * Padc
  lt1 = a1
  lt2 = padc
  lt3 = lt1 * lt2
  a1x1 = lt3
# ******* STEP 4 : y1 = a0 + a1x1
  lt1 = a0 << 10
  lt2 = a1x1
  lt3 = lt1 + lt2
  y1 = lt3
# ******* STEP 5 : a2x2 = b2 * Tadc
  lt1 = b2
  lt2 = tadc
  lt3 = lt1 * lt2;
  a2x2 = lt3 >> 1
# ******* STEP 6 : PComp = y1 + a2x2
  lt1 = y1
  lt2 = a2x2
  lt3 = lt1 + lt2
  pcomp = lt3 >> 9

  return pcomp
end

def calculatePCompShort(padc, tadc, a0, b1, b2, c12)
  if a0 >= 0x8000 then
    a0 = a0 - 0x10000
  end
  if b1 >= 0x8000 then
    b1 = b1 - 0x10000
  end
  if b2 >= 0x8000 then
    b2 = b2 - 0x10000
  end
  if c12 >= 0x8000 then
    c12 = c12 - 0x10000
  end
  padc = padc >> 6
  tadc = tadc >> 6
  c12x2 = (c12 * tadc) >> 11
  a1 = b1 + c12x2;
  a1x1 = a1 * padc
  y1 = (a0 << 10) + a1x1
  a2x2 = (b2 * tadc) >> 1
  pcomp = (y1 + a2x2) >> 9
  return pcomp
end

begin

# i2c address

MPLADDR = 0x60

# GPIO I2C Pin

SCL = 3
SDA = 5

# ip address setting

addr = 10 << 24 | 0 << 16 | 1 << 8 | 222
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 0 << 16 | 1 << 8 | 1
dns = 10 << 24 | 0 << 16 | 1 << 8 | 1

# start processing

rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)

rtl.i2cinit(SCL, SDA)

rtl.i2cwrite(MPLADDR, 0x12, 0x00)

delay(rtl, 10)

a0 = rtl.i2cread(MPLADDR, 0x04) << 8 | rtl.i2cread(MPLADDR, 0x05)
b1 = rtl.i2cread(MPLADDR, 0x06) << 8 | rtl.i2cread(MPLADDR, 0x07)
b2 = rtl.i2cread(MPLADDR, 0x08) << 8 | rtl.i2cread(MPLADDR, 0x09)
c12 = rtl.i2cread(MPLADDR, 0x0a) << 8 | rtl.i2cread(MPLADDR, 0x0b)

rtl.print "a0 = " + a0.to_s + "\n"
rtl.print "b1 = " + b1.to_s + "\n"
rtl.print "b2 = " + b2.to_s + "\n"
rtl.print "c12 = " + c12.to_s + "\n"

apikey = "naisyo"

interval = 15
count = 0

i = 0
while 1 do
  rtl.print "."
  delay(rtl, 100)
  count = count + 1

  if count % interval == 0 then

    padc = rtl.i2cread(MPLADDR, 0x00) << 8 | rtl.i2cread(MPLADDR, 0x01)
    tadc = rtl.i2cread(MPLADDR, 0x02) << 8 | rtl.i2cread(MPLADDR, 0x03)

    pcomp = calculatePCompShort(padc, tadc, a0, b1, b2, c12)
#    pcomp = calculatePCompLong(padc, tadc, a0, b1, b2, c12)
    pressure = ((pcomp * 1041) >> 14) + 800

    kpa =  (pressure >> 4).to_s + "." + (((pressure & 0xf) * 1000) / 16).to_s
    rtl.print kpa + "\n"

    res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + apikey + "&field1=" + kpa, {'User-Agent' => "test-agent"})
  end

end

rescue => e
  rtl.print e.to_s
end