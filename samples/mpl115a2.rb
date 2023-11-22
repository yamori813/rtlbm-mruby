#
# rtlbm-mruby mruby script
#
# test from i2c mpl115a2 pressure data
#

# This calculate code is based c source code in NXP AN3785 document

def calculatePCompLong(padc, tadc, a0, b1, b2, c12)
  if a0 >= 0x8000 then
    a0 = a0 - 0x10000
  end
  if b1 >= 0x8000 then
    b1 = b1 - 0x10000
  end
  if b2 >= 0x8000 then
    b2 = b2 - 0x10000
  end
  if c12 >= 0x8000 then
    c12 = c12 - 0x10000
  end
  padc = padc >> 6
  tadc = tadc >> 6
# ******* STEP 1 : c12x2 = c12 * Tadc
  lt1 = c12
  lt2 = tadc
  lt3 = lt1 * lt2
  c12x2 = lt3 >> 11
# ******* STEP 2 : a1 = b1 + c12x2
  lt1 = b1
  lt2 = c12x2
  lt3 = lt1 + lt2
  a1 = lt3
# ******* STEP 3 : a1x1 = a1 * Padc
  lt1 = a1
  lt2 = padc
  lt3 = lt1 * lt2
  a1x1 = lt3
# ******* STEP 4 : y1 = a0 + a1x1
  lt1 = a0 << 10
  lt2 = a1x1
  lt3 = lt1 + lt2
  y1 = lt3
# ******* STEP 5 : a2x2 = b2 * Tadc
  lt1 = b2
  lt2 = tadc
  lt3 = lt1 * lt2;
  a2x2 = lt3 >> 1
# ******* STEP 6 : PComp = y1 + a2x2
  lt1 = y1
  lt2 = a2x2
  lt3 = lt1 + lt2
  pcomp = lt3 >> 9

  return pcomp
end

def calculatePCompShort(padc, tadc, a0, b1, b2, c12)
  if a0 >= 0x8000 then
    a0 = a0 - 0x10000
  end
  if b1 >= 0x8000 then
    b1 = b1 - 0x10000
  end
  if b2 >= 0x8000 then
    b2 = b2 - 0x10000
  end
  if c12 >= 0x8000 then
    c12 = c12 - 0x10000
  end
  padc = padc >> 6
  tadc = tadc >> 6
  c12x2 = (c12 * tadc) >> 11
  a1 = b1 + c12x2;
  a1x1 = a1 * padc
  y1 = (a0 << 10) + a1x1
  a2x2 = (b2 * tadc) >> 1
  pcomp = (y1 + a2x2) >> 9
  return pcomp
end

begin

# i2c address

MPLADDR = 0x60

# GPIO I2C Pin

SCL = 3
SDA = 5

# start processing

rtl = YABM.new

rtl.i2cinit(SCL, SDA, 1)

a0 = rtl.i2cread(MPLADDR, 1, 0x04) << 8 | rtl.i2cread(MPLADDR, 1, 0x05)
b1 = rtl.i2cread(MPLADDR, 1, 0x06) << 8 | rtl.i2cread(MPLADDR, 1, 0x07)
b2 = rtl.i2cread(MPLADDR, 1, 0x08) << 8 | rtl.i2cread(MPLADDR, 1, 0x09)
c12 = rtl.i2cread(MPLADDR, 1, 0x0a) << 8 | rtl.i2cread(MPLADDR, 1, 0x0b)

rtl.print "a0 = " + a0.to_s + "\n"
rtl.print "b1 = " + b1.to_s + "\n"
rtl.print "b2 = " + b2.to_s + "\n"
rtl.print "c12 = " + c12.to_s + "\n"

interval = 15
count = 0

i = 0
loop do
  rtl.print "."
  rtl.msleep(1000)
  count += 1

  if count % interval == 0 then

    rtl.i2cwrite(MPLADDR, 0x12, 0x00)
    rtl.msleep(100)

    padc = rtl.i2cread(MPLADDR, 1, 0x00) << 8 | rtl.i2cread(MPLADDR, 1, 0x01)
    tadc = rtl.i2cread(MPLADDR, 1, 0x02) << 8 | rtl.i2cread(MPLADDR, 1, 0x03)

    pcomp = calculatePCompShort(padc, tadc, a0, b1, b2, c12)
#    pcomp = calculatePCompLong(padc, tadc, a0, b1, b2, c12)
    pressure = ((pcomp * 1041) >> 14) + 800

    frec = (((pressure & 0xf) * 1000) / 16).to_s
    if frec.length == 3 then
      kpa =  (pressure >> 4).to_s + "." + frec
    else
      kpa =  (pressure >> 4).to_s + ".0" + frec
    end
    rtl.print kpa + "\n"

  end

end

rescue => e
  rtl.print e.to_s
end
