#
# rtlbm-mruby mruby script
# need sub_hsc.rb
#

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

begin

  rtl = YABM.new

  ontime = 2000
  offtime = 500

  gpioinit(rtl)

  lastbtn = button(rtl)

  phase = 0
  period = rtl.count() + ontime

  loop do
    if rtl.count() > period then
      if (phase % 2) == 1
        led(rtl, 1, 0)
        period = rtl.count() + offtime
      else
        if phase == 0
          led(rtl, 1, TOP_LED1)
        elsif phase == 2
          led(rtl, 1, TOP_LED2)
        elsif phase == 4
          led(rtl, 1, TOP_LED3)
        end
        period = rtl.count() + ontime
      end
      phase = (phase + 1) % 6
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
