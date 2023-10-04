#
# mruby on YABM script
#
# PIC12F1822 I2C slave read sample script
# [0] ADC Hi, [1] ACD Lo, [2], CRC
# thingspeak channel update by HomeSpotCube
# need sub_hsc.rb
#

APIKEY = "naisyo"

# GPIO I2C Pin (SW12)

SCL = 2
SDA = 11

PICADDR = 4

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~TOP_LED3)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | TOP_LED3)
end

def chkcrc dat, n
  crc8 = 0xff
  for i in 0..(n - 1) do
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
  crc8 & 0xff
end


begin

  yabm = YABM.new

  yabm.netstartdhcp

  gpioinit(yabm)

# sync date by ntp use https X.509
  ntpaddr = yabm.lookup("ntp.nict.jp")
  yabm.sntp(ntpaddr)

  yabm.i2cinit(SCL, SDA, 1)

  interval = 60

  yabm.watchdogstart(256)

  arr = Array.new

  count = 0
  loop do
    ledon yabm

    yabm.i2cwrite(PICADDR, 0, 0)
    for addr in 0..2 do
      arr[addr] = yabm.i2cread(PICADDR, addr)
    end
    yabm.print arr[0].to_s(16).rjust(2, '0') + ","
    yabm.print arr[1].to_s(16).rjust(2, '0') + ","
    yabm.print arr[2].to_s(16).rjust(2, '0') + " "
    val = arr[0] * 0x100 + arr[1]
    crc = chkcrc(arr, 2)
    yabm.print val.to_s + " " + crc.to_s(16).rjust(2, '0') + "\r\n"

    # first time is not stabilize
    if count != 0 && arr[2] == crc
      obj = SimpleHttp.new("https", "api.thingspeak.com", 443)
      res = obj.request("GET", "/update?api_key=" + APIKEY + "&field1=" +
        val.to_s, {'User-Agent' => "test-agent"})
       yabm.print "." + "\r\n"
    end
    ledoff yabm
    yabm.msleep interval * 1000
    yabm.watchdogreset
    count += 1
  end

rescue => e
  yabm.print e.to_s
end
