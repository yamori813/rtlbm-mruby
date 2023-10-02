#
# mruby on YABM script
#
# PIC12F1822 I2C slave read sample script
# [0] ADC Hi, [1] ACD Lo, [2], CRC
#

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


#
# main
#

begin

  yabm = YABM.new

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 1)

  arr = Array.new

  loop do
    yabm.i2cwrite(4, 0, 0)
    for addr in 0..2 do
      arr[addr] = yabm.i2cread(4, addr)
    end
    yabm.print arr[0].to_s(16).rjust(2, '0') + ","
    yabm.print arr[1].to_s(16).rjust(2, '0') + ","
    yabm.print arr[2].to_s(16).rjust(2, '0') + " "
    val = arr[0] * 0x100 + arr[1]
    crc = chkcrc(arr, 2)
    yabm.print val.to_s + " " + crc.to_s(16).rjust(2, '0') + "\r\n"

    yabm.msleep(5_000)
  end

rescue => e
  yabm.print e.to_s
end
