#
# rtlbm-mruby mruby script
#
# i2c lcd display and eeprom write and read 
# eeprom is 1024 bit chip
#
# echo -n "MORIMORI" | nc -w 0 -u 10.10.10.2 7000
#

# i2c pin

I2CSCK = 3
I2CSDA = 5

# i2c lcd and eeprom address

LCDADDR = 0x3e
ROMADDR = 0x50

class I2CEPROM
  def initialize yabm
    @y = yabm
  end

  def read
    restore = []

    skip = 0

    for addr in 0..127 do
      val = @y.i2cread(ROMADDR, 1, addr)
      if val == 0 then
        skip = 1
      end
      if skip == 0 then
        restore.push(val)
        @y.print val.to_s + " "
      end
    end
    return restore
  end

  def write str
    @y.i2cwrite(ROMADDR, 0, 0x40)
      @y.msleep(20)
    arr = str.split("")
    i = 1
    for ch in arr do
      @y.i2cwrite(ROMADDR, i, ch.ord)
      @y.msleep(20)
      i = i + 1
    end
    @y.i2cwrite(ROMADDR, i, 0)
    @y.msleep(20)
  end
end

class I2CLCD
  def initialize yabm
    @y = yabm
    @y.i2cwrite(LCDADDR, [0x38, 0x39, 0x14, 0x70, 0x56, 0x6c])
    @y.msleep(200)
    @y.i2cwrite(LCDADDR, [0x38, 0x0d, 0x01])
    @y.msleep(10)
  end

  def clear
    @y.i2cwrite(LCDADDR, [0x00, 0x01])
    @y.msleep(100)
  end

  def next
    @y.i2cwrite(LCDADDR, [0x00, 0xc0])
    @y.msleep(100)
  end

  def print str
    lcdcmd = [0x40]
    arr = str.split("")
    for ch in arr do
      lcdcmd.push(ch.ord)
    end
    @y.print lcdcmd.to_s
    @y.i2cwrite(LCDADDR, lcdcmd)
  end

  def cmd para
    @y.i2cwrite(LCDADDR, para)
  end
end

begin

# ip address setting

addr = "10.10.10.2"
mask = "255.255.255.0"
gw = "10.10.10.1"
dns = "10.10.10.1"

yabm = YABM.new

yabm.netstart(addr, mask, gw, dns)

yabm.udpinit
yabm.udpbind(7000)

# use gpio pin
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

yabm.i2cinit(I2CSCK, I2CSDA, 1)

lcd = I2CLCD.new yabm

rom = I2CEPROM.new yabm

tmpstr = ""

restore = rom.read

yabm.msleep(100)

if restore.length != 0 && restore[0] == 0x40 then
  lcd.clear
  lcd.cmd restore
end

laststr = ""
loop do
  yabm.print "."
  yabm.msleep(500)
  udpstr = yabm.udprecv()
  if udpstr.length != 0 then
     lcd.clear
     lcd.print udpstr
     rom.write udpstr
     if laststr.length != 0
       lcd.next
       lcd.print laststr
     end
     laststr = udpstr
  end
end

rescue => e
  yabm.print e.to_s
end
