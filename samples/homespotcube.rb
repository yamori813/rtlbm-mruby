#
# rtlbm-mruby mruby script
#
# T2
# 1	VCC
# 2	96	GPIOA[3]	SW
# 3	43	GPIOC[1]	Status LED
# 4	109	GPIOC[0]	TOP LED
# 5	98	GPIOA[6]	Status LED
# 6	97	GPIOA[4]	TOP LED
# 7	67	GPIOA[0]	Status LED
# 8	40	GPIOA[1]	TOP LED
# 9	GND
# 10	GND
# SW12
# 	100	GPIOA[2]
#	102	GPIOB[3]
# Reset
#	99	GPIOA[5]
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)

def button(rtl)
  reg = rtl.gpiogetdat()
  return reg & TOP_BUTTON
end

def led(rtl, type, val)
  sarr = [0, STATUS_LED1, STATUS_LED2, STATUS_LED3]
  tarr = [0, TOP_LED1, TOP_LED2, TOP_LED3]
  reg = rtl.gpiogetdat()
  if(type == 1) then
    reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
    reg = reg & ~sarr[val]
  else
    reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
    reg = reg & ~tarr[val]
  end
  rtl.gpiosetdat(reg)
end

begin

rtl = YABM.new

addr = "10.10.10.2"
mask = "255.255.255.0"
gw = "10.10.10.1"
dns = "10.10.10.1"

rtl.netstart(addr, mask, gw, dns)

rtl.udpinit
rtl.udpbind(7000)

rtl.gpiosetsel(0x003c300c, 0x003c300c, 0x00001800, 0x00001800)

reg = rtl.gpiogetctl()
reg = reg & ~(STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
reg = reg & ~(TOP_LED1 | TOP_LED2 | TOP_LED3)
reg = reg & ~(TOP_BUTTON)
rtl.gpiosetctl(reg)

reg = rtl.gpiogetdir()
reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
reg = reg & ~(TOP_BUTTON)
rtl.gpiosetdir(reg)

reg = rtl.gpiogetdat()
reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
rtl.gpiosetdat(reg)

stat = 0
lastbtn = button(rtl)

i = 0
loop do
   udpstr = rtl.udprecv()
   if udpstr.length != 0 then
     if udpstr == "GREEN" then
       led(rtl, 2, 2)
     elsif udpstr == "RED" then
       led(rtl, 2, 1)
     elsif udpstr == "ORANGE" then
       led(rtl, 2, 3)
     elsif udpstr == "OFF" then
       led(rtl, 2, 0)
     end
     rtl.print udpstr
   end
   if lastbtn != button(rtl) && lastbtn != 0 then
     rtl.print "button"
     stat = stat + 1
     if stat == 4 then
       stat = 0
     end
     led(rtl, 1, stat)
   end
   lastbtn = button(rtl)
   i = i + 1
end

rescue => e
  rtl.print e.to_s
end
