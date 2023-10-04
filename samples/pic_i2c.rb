#
# mruby on YABM script
#
# PIC12F1822 I2C slave read sample script
# [0] ADC Hi, [1] ACD Lo, [2], CRC
# need sub_hsc.rb
#

# GPIO I2C Pin (SW12)

SCL = 2
SDA = 11

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

PICADDR = 4

#
# main
#

begin

  yabm = YABM.new

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 1)

  arr = Array.new(3)

  loop do
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

    yabm.msleep(5_000)
  end

rescue => e
  yabm.print e.to_s
end
