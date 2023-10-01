#
# mruby on YABM script
# for BMP180

APIKEY = "naisyo"

NONET = true

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

PRESSURE_WAIT = [5, 8, 14, 26]

def readup(yabm, oss) 
  msb = yabm.i2cread(BMPADDR, 0xf6)
  lsb = yabm.i2cread(BMPADDR, 0xf7)
  xlsb = yabm.i2cread(BMPADDR, 0xf8)
  val = ((msb << 16) + (lsb << 8) + xlsb) >> (8 - oss)
  return val
end

def readu16(yabm, addr) 
  val = yabm.i2cread(BMPADDR, addr) << 8 | yabm.i2cread(BMPADDR, addr + 1)
  return val
end

def read16(yabm, addr) 
  val = yabm.i2cread(BMPADDR, addr) << 8 | yabm.i2cread(BMPADDR, addr + 1)
  if val >= 0x8000 then
    val = val - 0x10000
  end
  return val
end

begin

  yabm = YABM.new

  if !NONET then
    yabm.netstartdhcp

    yabm.print yabm.getaddress + "\n"

    ntpaddr = yabm.lookup("ntp.nict.jp")
    yabm.sntp(ntpaddr)
  end

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

  yabm.i2cinit(SCL, SDA, 1)

  ac1 = read16(yabm, 0xaa)
  ac2 = read16(yabm, 0xac)
  ac3 = read16(yabm, 0xae)
  ac4 = readu16(yabm, 0xb0)
  ac5 = readu16(yabm, 0xb2)
  ac6 = readu16(yabm, 0xb4)
  b1 = read16(yabm, 0xb6)
  b2 = read16(yabm, 0xb8)
  mb = read16(yabm, 0xba)
  mc = read16(yabm, 0xbc)
  md = read16(yabm, 0xbe)

# indecate start by led
  reg = yabm.gpiogetdat()
  reg = reg & ~STATUS_LED2
  yabm.gpiosetdat(reg)

# 0 ultra low power, 1 standard, 2 high resolution, 3 ultra high resolution

  oss = 3
  count = 0

  loop do

    reg = yabm.gpiogetdat()
    reg = reg & ~TOP_LED3
    yabm.gpiosetdat(reg)

    yabm.i2cwrite(BMPADDR, 0xf4, 0x2e)
    yabm.msleep(5)
    ut = read16(yabm, 0xf6)
    yabm.print ut.to_s + " "

    yabm.i2cwrite(BMPADDR, 0xf4, 0x34 + (oss << 6))
    yabm.msleep(PRESSURE_WAIT[oss])
    up = readup(yabm, oss)
    yabm.print up.to_s + " "

    if ut != 0 && up != 0 then

# calculate true temperature

      x1 = (ut - ac6) * ac5 >> 15
      x2 = (mc << 11) / (x1 + md)
      b5 = x1 + x2
      t = (b5 + 8) >> 4

      tstr = (t / 10).to_s + "." + (t % 10).to_s

      yabm.print tstr + " "

# calculate true pressure

      b6 = b5 - 4000
      x1 = (b2 * (b6 * b6 >> 12)) >> 11
      x2 = ac2 * b6 >> 11
      x3 = x1 + x2
      b3 = (((ac1 * 4 + x3) << oss) + 2) >> 2
      x1 = ac3 * b6 >> 13
      x2 = (b1 * (b6 * b6 >> 12)) >> 16
      x3 = ((x1 + x2) + 2) >> 2
      b4 = (ac4 * (x3 + 32768)) >> 15
      b7 = (up - b3) * (50000 >> oss)

# This is only temporally code because of valid non overflow
      p = b7 / (b4 >> 1)

      x1 = (p >> 8) * (p >> 8)
      x1 = (x1 * 3038) >> 16
      x2 = (-7357 * p) >> 16
      p = p + ((x1 + x2 + 3791) >> 4)

      pstr = (p / 100).to_s + "."
      syo = p % 100
      if syo < 10 then
        pstr = pstr + "0" + syo.to_s
      else
        pstr = pstr + syo.to_s
      end
      yabm.print pstr + " "

      if !NONET then
        res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + APIKEY + "&field1=" + tstr + "&field2=" + pstr + "&field3=" + count.to_s, {'User-Agent' => "test-agent"})
        count += 1
        if res 
          yabm.print " " + res.status.to_s
        end
      end
      yabm.print "\r\n"

    end

    reg = yabm.gpiogetdat()
    reg = reg | TOP_LED3
    yabm.gpiosetdat(reg)

    if count == 1000 then
      reg = yabm.gpiogetdat()
      reg = reg | STATUS_LED2
      yabm.gpiosetdat(reg)
    end

    # ThingSpeak Free account need at intervals 15 sec.
    yabm.msleep(20000)

  end

rescue => e
  yabm.print e.to_s
end
