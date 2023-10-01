#
# rtlbm-mruby mruby script
#
# This is demonstration of I2C LCD
#


# i2c pin

I2CSCK = 3
I2CSDA = 5

# i2c lcd and eeprom address

LCDADDR = 0x3e

class I2CLCD
  def init yabm
    @y = yabm
    @y.i2cwrites(LCDADDR, [0x38, 0x39, 0x14, 0x70, 0x56, 0x6c], 0)
    @y.msleep(200)
    @y.i2cwrites(LCDADDR, [0x38, 0x0d, 0x01], 0)
    @y.msleep(10)
  end

  def clear
    @y.i2cwrites(LCDADDR, [0x00, 0x01], 0)
    @y.msleep(100)
  end

  def next
    @y.i2cwrites(LCDADDR, [0x00, 0xc0], 0)
    @y.msleep(100)
  end

  def print str
    lcdcmd = [0x40]
    arr = str.split("")
    for ch in arr do
      lcdcmd.push(ch.ord)
    end
    @y.print lcdcmd.to_s
    @y.i2cwrites(LCDADDR, lcdcmd, 0)
  end

  def cmd para
    @y.i2cwrites(LCDADDR, para, 0)
  end
end

begin

yabm = YABM.new

# use gpio pin
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

gpio = yabm.gpiogetdat
yabm.gpiosetdat(gpio | (1 << 16) | 0x7c00)

yabm.i2cinit(I2CSCK, I2CSDA, 1)

lcd = I2CLCD.new
lcd.init yabm

lcd.clear

str1 = "mruby on"
str2 = "YABM"

i = 0
while 1 do
  yabm.print "."
  yabm.msleep(500)
  if i < str1.length
    lcd.print str1[i]
  else
    lcd.print str2[i-8]
  end
  i = i + 1
  if i == str1.length
    lcd.next
  end
  if i == str1.length + str2.length
    yabm.msleep(1_000)
    5.times {
      yabm.gpiosetdat(gpio & ~(1 << 16))
      yabm.msleep(200)
      yabm.gpiosetdat(gpio | (1 << 16))
      yabm.msleep(200)
    }
    yabm.msleep(3_000)
    lcd.clear
    i = 0
  end
end

rescue => e
  yabm.print e.to_s
end
