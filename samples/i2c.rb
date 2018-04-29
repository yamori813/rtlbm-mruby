#
# rtlbm-mruby mruby script
#
# i2c lcd display and eeprom write and read 
# eeprom is 1024 bit chip
#
# echo -n "MORIMORI" | nc -w 0 -u 10.10.10.2 7000
#

def delay(rtl, val) 
  start = rtl.count() 
  while rtl.count() < start + val do
  end
end

begin

# i2c lcd and eeprom address

LCDADDR = 0x3e
ROMADDR = 0x50

# ip address setting

addr = 10 << 24 | 10 << 16 | 10 << 8 | 2
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 10 << 16 | 10 << 8 | 1
dns = 10 << 24 | 10 << 16 | 10 << 8 | 1

rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit
rtl.udpbind(7000)

# use gpio pin
rtl.gpiosetsel(0x300000, 0x300000, 0, 0)

rtl.i2cinit(3, 5)

tmpstr = ""

rtl.i2cwrites(LCDADDR, [0x38, 0x39, 0x14, 0x70, 0x56, 0x6c])
delay(rtl, 200)
rtl.i2cwrites(LCDADDR, [0x38, 0x0d, 0x01])
delay(rtl, 10)

restore = []

skip = 0

for addr in 0..127 do
  val = rtl.i2cread(ROMADDR, addr)
  if val == 0 then
    skip = 1
  end
  if skip == 0 then
    restore.push(val)
    rtl.print val.to_s + " "
  end
end

delay(rtl, 100)

if restore[0] == 0x40 then
  rtl.print "*"
  rtl.i2cwrites(LCDADDR, [0x00, 0x01])
  rtl.i2cwrites(LCDADDR, restore)
end

i = 0
while 1 do
  rtl.print "."
  delay(rtl, 500)
  udpstr = rtl.udprecv()
  if udpstr.length != 0 then
    rtl.i2cwrites(LCDADDR, [0x00, 0x01])
    lcdcmd = [0x40]
    arr = udpstr.split("")
    for ch in arr do
      lcdcmd.push(ch.ord)
    end
    rtl.print lcdcmd.to_s
    rtl.i2cwrites(LCDADDR, lcdcmd)
      i = 0
      for num in lcdcmd do
        rtl.i2cwrite(ROMADDR, i, num)
        delay(rtl, 20)
        i = i + 1
      end
      rtl.i2cwrite(ROMADDR, i, 0)
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
