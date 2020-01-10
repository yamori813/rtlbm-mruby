#
# mruby on YABM script
#
# Weather Station used by BMP180 and Si7021 on HomeSpotCube
#

APIKEY = "naisyo"

# for debug
NONET = false
#NONET = true

MAXFAILE = 10

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

BMPADDR = 0x77
SIADDR = 0x40

def delay(yabm, val) 
  start = yabm.count() 
  while yabm.count() < start + val do
  end
end

def pointstr(p, c)
  return p.to_s.insert(-1 - c, ".")
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

# sensor class

class SI7021
  def init yabm
    @y = yabm
  end

  def getRevition
    @y.i2cwrites(SIADDR, [0x84, 0xb8], 0)
    delay(@y, 1)
    siarr = @y.i2creads(SIADDR, 1)
    return siarr[0]
  end

  def getSerialBytes
    @y.i2cwrites(SIADDR, [0xfa, 0x0f], 0)
    delay(@y, 1)
    siarr1 = @y.i2creads(SIADDR, 8)
    @y.i2cwrites(SIADDR, [0xfc, 0xc9], 0)
    delay(@y, 1)
    siarr2 = @y.i2creads(SIADDR, 6)
    res = Array.new
    res.push(siarr1[0])
    res.push(siarr1[2])
    res.push(siarr1[4])
    res.push(siarr1[6])
    res.push(siarr2[0])
    res.push(siarr2[1])
    res.push(siarr2[3])
    res.push(siarr2[4])
    return res
  end

# not use
  def chkcrc dat
    crc8 = 0xff
    for i in 0..1 do
      crc8 = crc8 ^ dat[i]
      8.times {
        if crc8 & 0x80 == 0x80 then
          crc8 = crc8 << 1
          crc8 = crc8 ^ 0x31
        else
          crc8 = crc8 << 1
        end
      }
    end
    if (crc8 & 0xff) == dat[2] then
      return true
    else
      return false
    end
  end

  def getCelsiusHundredths
    @y.i2cwrites(SIADDR, [0xf3], 1)
    while 1 do
      delay(@y, 1)
      siarr = @y.i2creads(SIADDR, 2)
      if siarr != nil then
        break
      end
      delay(@y, 1)
    end
    tempcode = (siarr[0] << 8) | siarr[1]
    t = (tempcode * 17572) / 65536 - 4685
    return t
  end

  def getCelsiusPostHumidity
    @y.i2cwrites(SIADDR, [0xe0], 1)
    while 1 do
      delay(@y, 1)
      siarr = @y.i2creads(SIADDR, 2)
      if siarr != nil then
        break
      end
      delay(@y, 1)
    end
    tempcode = (siarr[0] << 8) | siarr[1]
    t = (tempcode * 17572) / 65536 - 4685
    return t
  end

  def getHumidityPercent
    @y.i2cwrites(SIADDR, [0xf5], 1)
    while 1 do
      siarr = @y.i2creads(SIADDR, 2)
      if siarr != nil then
        break
      end
      delay(@y, 1)
    end
    rhcode = (siarr[0] << 8) | siarr[1]
    rh = (125 * rhcode) / 65536 - 6
    return rh
  end
end

class BMP180
  PRESSURE_WAIT = [5, 8, 14, 26]

  def readup
    msb = @y.i2cread(BMPADDR, 0xf6)
    lsb = @y.i2cread(BMPADDR, 0xf7)
    xlsb = @y.i2cread(BMPADDR, 0xf8)
    val = ((msb << 16) + (lsb << 8) + xlsb) >> (8 - @oss)
    return val
  end

  def readu16 addr 
    val = @y.i2cread(BMPADDR, addr) << 8 | @y.i2cread(BMPADDR, addr + 1)
    return val
  end

  def read16 addr 
    val = @y.i2cread(BMPADDR, addr) << 8 | @y.i2cread(BMPADDR, addr + 1)
    if val >= 0x8000 then
      val = val - 0x10000
    end
    return val
  end

  def init yabm, oss
    @y = yabm
    @oss = oss
    @ac1 = read16 0xaa
    @ac2 = read16 0xac
    @ac3 = read16 0xae
    @ac4 = readu16 0xb0
    @ac5 = readu16 0xb2
    @ac6 = readu16 0xb4
    @b1 = read16 0xb6
    @b2 = read16 0xb8
    @mb = read16 0xba
    @mc = read16 0xbc
    @md = read16 0xbe
  end

  def readTemperature
    @y.i2cwrite(BMPADDR, 0xf4, 0x2e)
    delay(@y, 5)
    ut = read16 0xf6
#    @y.print ut.to_s + " "

# calculate true temperature

    x1 = (ut - @ac6) * @ac5 >> 15
    x2 = (@mc << 11) / (x1 + @md)
    @b5 = x1 + x2
    t = (@b5 + 8) >> 4

    return t
  end

  def getChipid
    return @y.i2cread(BMPADDR, 0xd0)
  end
# calculate true pressure

  def readPressure
    @y.i2cwrite(BMPADDR, 0xf4, 0x34 + (@oss << 6))
    delay(@y, PRESSURE_WAIT[@oss])
    up = readup
#    @y.print up.to_s + " "

    b6 = @b5 - 4000
    x1 = (@b2 * (b6 * b6 >> 12)) >> 11
    x2 = @ac2 * b6 >> 11
    x3 = x1 + x2
    b3 = (((@ac1 * 4 + x3) << @oss) + 2) >> 2
    x1 = @ac3 * b6 >> 13
    x2 = (@b1 * (b6 * b6 >> 12)) >> 16
    x3 = ((x1 + x2) + 2) >> 2
    b4 = (@ac4 * (x3 + 32768)) >> 15
    b7 = (up - b3) * (50000 >> @oss)

# This is only temporally code because of valid non overflow
    p = b7 / (b4 >> 1)

    x1 = (p >> 8) * (p >> 8)
    x1 = (x1 * 3038) >> 16
    x2 = (-7357 * p) >> 16
    p = p + ((x1 + x2 + 3791) >> 4)
    return p
  end

end

# main

begin

  yabm = YABM.new

  if !NONET then

    yabm.netstartdhcp

    yabm.print yabm.getaddress + "\n"

    ntpaddr = yabm.lookup("ntp.nict.jp")
    yabm.sntp(ntpaddr)
  end

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 1)

  lastbt = 0
  lastbp = 0
  lastst = 0
  lastsh = 0

# Si7021 Firmware Revision
  si = SI7021.new
  si.init(yabm)
  rev = si.getRevition
  ser = si.getSerialBytes
  yabm.print "Si70" + ser[4].to_s + " REV: " + rev.to_s + "\r\n"

  bmp = BMP180.new
# 0 ultra low power, 1 standard, 2 high resolution, 3 ultra high resolution
  bmp.init(yabm, 3)
  id = bmp.getChipid
  yabm.print "BMP180 ID: " + id.to_s + "\r\n"

# indecate start by led
  reg = yabm.gpiogetdat()
  reg = reg & ~STATUS_LED2
  yabm.gpiosetdat(reg)

  count = 0
  neterr = 0

  yabm.watchdogstart(256)

  while 1 do

    reg = yabm.gpiogetdat()
    reg = reg & ~TOP_LED3
    yabm.gpiosetdat(reg)

    error = 0

# Si7021

    while yabm.i2cchk(SIADDR) == 0 do
      delay(yabm, 1)
    end
    sh = si.getHumidityPercent
    if count == 0 || (lastsh - sh).abs < 10 then
      shstr = sh.to_s
      lastsh = sh
    else
      shstr = lastsh.to_s
      error = error | (1 << 3)
    end
    while yabm.i2cchk(SIADDR) == 0 do
      delay(yabm, 1)
    end
    st = si.getCelsiusHundredths
    if count == 0 || (lastst - st).abs < 100 then
      ststr = pointstr(st, 2)
      lastst = st
    else
      ststr = pointstr(lastst, 2)
      error = error | (1 << 2)
    end
    yabm.print count.to_s + " SIT: " + ststr + " RH: " + shstr + " "

# BMP180

    if yabm.i2cchk(BMPADDR) == 1 then
    bt = bmp.readTemperature
    if count == 0 || (lastbt - bt).abs < 10 then
      btstr = pointstr(bt, 2)
      lastbt = bt
    else
      btstr = pointstr(lastbt, 2)
      error = error | (1 << 0)
    end
    yabm.print " BMPT: " + btstr + " "

    bp = bmp.readPressure
    if count == 0 || (lastbp - bp).abs < 100 then
      bpstr = pointstr(bp, 2)
      lastbp = bp
    else
      bpstr = pointstr(lastbp, 2)
      error = error | (1 << 1)
    end
    yabm.print "P: " + bpstr + " " + error.to_s

    para = "api_key=" + APIKEY + "&field1=" + count.to_s + "&field2=" + btstr + "&field3=" + bpstr + "&field4=" + ststr + "&field5=" + shstr + "&field6=" + error.to_s
    if !NONET then
      res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?" + para, {'User-Agent' => "test-agent"})
      if res != nil && res.status.to_s.length != 0 then
        yabm.print " " + res.status.to_s
        neterr = 0
      else
        neterr = neterr + 1
        if neterr == MAXFAILE then
          raise ""
        end
      end
    else
#      yabm.print para
    end
    end
    yabm.print "\r\n"
    count = count + 1

    reg = yabm.gpiogetdat()
    reg = reg | TOP_LED3
    yabm.gpiosetdat(reg)

    if count == 500 then
      reg = yabm.gpiogetdat()
      reg = reg | STATUS_LED2
      yabm.gpiosetdat(reg)
    end

    yabm.watchdogreset

    # ThingSpeak Free account need at intervals over 15 sec.
    if NONET then
      delay(yabm, 5000)
    else
      delay(yabm, 20000)
    end

  end

rescue => e
  yabm.print e.to_s
end
