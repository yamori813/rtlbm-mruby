#
# mruby on YABM script
#
# HTU21D sample code on HomeSpotCube
# need sub_hsc.rb
#

# GPIO I2C Pin (SW12)

SCL = 2
SDA = 11

def pointstr(p, c)
  if p == 0 then
    "0." + "0" * c
  elsif p.abs < 10 ** c
    l = c - p.abs.to_s.length + 1
    s = p.to_s.insert(p < 0 ? 1 : 0, "0" * l)
    s.insert(-1 - c, ".")
  else
    p.to_s.insert(-1 - c, ".")
  end
end

class HTU21D
  HTU21ADDR = 0x40

  HTU21_RESET_COMMAND = [0xFE]
  HTU21_READ_TEMPERATURE_W_HOLD_COMMAND = [0xE3]
  HTU21_READ_TEMPERATURE_WO_HOLD_COMMAND = [0xF3]
  HTU21_READ_HUMIDITY_W_HOLD_COMMAND = [0xE5]
  HTU21_READ_HUMIDITY_WO_HOLD_COMMAND = [0xF5]

  HTU21_READ_SERIAL_FIRST_8BYTES_COMMAND = [0xFA, 0x0F]
  HTU21_READ_SERIAL_LAST_6BYTES_COMMAND = [0xFC, 0xC9]

# Conversion timings
  HTU21_TEMPERATURE_CONVERSION_TIME_T_14b_RH_12b = 50000
  HTU21_TEMPERATURE_CONVERSION_TIME_T_13b_RH_10b = 25000
  HTU21_TEMPERATURE_CONVERSION_TIME_T_12b_RH_8b = 13000
  HTU21_TEMPERATURE_CONVERSION_TIME_T_11b_RH_11b = 7000
  HTU21_HUMIDITY_CONVERSION_TIME_T_14b_RH_12b = 16000
  HTU21_HUMIDITY_CONVERSION_TIME_T_13b_RH_10b = 5000


def htu21_crc value
  
  polynom = 0x988000   # x^8 + x^5 + x^4 + 1
  msb     = 0x800000
  mask    = 0xFF8000
  result  = value << 8   # Pad with zeros as specified in spec 

  while msb != 0x80
    if (result & msb) != 0
      result = ((result ^ polynom) & mask) | ( result & ~mask)
    end
    # Shift by one
    msb >>= 1
    mask >>= 1
    polynom >>=1
  end
  result
end

  def init yabm
    @y = yabm
  end

  def reset
    @y.i2cwrites(HTU21ADDR, HTU21_RESET_COMMAND, 1)
    @y.msleep 15
  end

  def htu21_read_serial_number
    @y.i2cwrites(HTU21ADDR, HTU21_READ_SERIAL_FIRST_8BYTES_COMMAND, 0)
    rcv_data = @y.i2creads(HTU21ADDR, 8)

    @y.i2cwrites(HTU21ADDR, HTU21_READ_SERIAL_LAST_6BYTES_COMMAND, 0)
    rcv_data += @y.i2creads(HTU21ADDR, 6)

    serial_number = ""
    values = [0, 2, 4, 6, 8, 9, 11, 12]
    values.each do |n|
      serial_number += rcv_data[n].to_s(16).rjust(2, '0')
    end

    serial_number
  end

  def getCelsiusHundredths
    while @y.i2cchk(HTU21ADDR) == 0
      @y.msleep(1)
    end
    @y.i2cwrites(HTU21ADDR, HTU21_READ_TEMPERATURE_W_HOLD_COMMAND, 0)
#    @y.i2cwrites(HTU21ADDR, HTU21_READ_TEMPERATURE_WO_HOLD_COMMAND, 1)
#    @y.msleep(HTU21_TEMPERATURE_CONVERSION_TIME_T_14b_RH_12b/1000)
    arr = @y.i2creads(HTU21ADDR, 3)
    crc = htu21_crc (arr[0] << 8) | arr[1]
    if crc != arr[2] then
      @y.print arr.to_s + "," + crc.to_s
    end
    tempcode = (arr[0] << 8) | arr[1]
    (tempcode * 17572) / 65536 - 4685
  end
end

# main


begin

  yabm = YABM.new

  gpioinit(yabm)

  yabm.i2cinit(SCL, SDA, 10)

  h = HTU21D.new

  h.init yabm

  h.reset

  yabm.print h.htu21_read_serial_number

  loop do
    yabm.print pointstr(h.getCelsiusHundredths, 2) + ","
    yabm.msleep 2_000
  end

rescue => e
  yabm.print e.to_s
end
