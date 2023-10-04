#
# rtlbm-mruby mruby script
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
# need sub_hsc.rb
#

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

gpioinit(rtl)

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
