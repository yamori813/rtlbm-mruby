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

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)
SW1 = (1 << 2)
SW2 = (1 << 11)


def switch(rtl)

  sw = rtl.gpiogetdat()
  if (sw & SW1) == 0
    res = 0
  elsif (sw & SW2) == 0
    res = 1
  else
    res = 2
  end

  return res
end

def button(rtl)
  reg = rtl.gpiogetdat()
  return reg & TOP_BUTTON
end

def led(rtl, type, val)
  reg = rtl.gpiogetdat()
  if(type == 0) then
    reg = reg | (STATUS_LED1 | STATUS_LED2 | STATUS_LED3)
    reg = reg & ~val
  else
    reg = reg | (TOP_LED1 | TOP_LED2 | TOP_LED3)
    reg = reg & ~val
  end
  rtl.gpiosetdat(reg)
end

def initgpio(rtl)

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

end

begin

  rtl = YABM.new

  interval = 2000

  initgpio(rtl)

  lastbtn = button(rtl)

  phase = 0
  period = rtl.count() + interval

  while 1 do
    if rtl.count() > period then
      if (phase % 2) == 1
        led(rtl, 1, 0)
      elsif phase == 0
        led(rtl, 1, TOP_LED1)
      elsif phase == 2
        led(rtl, 1, TOP_LED2)
      elsif phase == 4
        led(rtl, 1, TOP_LED3)
      end
      phase = (phase + 1) % 6
      period = rtl.count() + interval
    end

    if button(rtl) == 0 then
      sw = switch(rtl)
      if sw == 0
        led(rtl, 0, STATUS_LED1)
      elsif sw == 1
        led(rtl, 0, STATUS_LED2)
      else
        led(rtl, 0, STATUS_LED3)
      end
    else
       led(rtl, 0, 0)
    end
  end

rescue => e
  rtl.print e.to_s
end
