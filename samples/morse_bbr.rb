#
# morus code generator on BBR
#

MORSE_NUM = ["01111","00111","00011","00001","00000","10000","11000","11100","11110","11111"]

MORSE_ALPH =["01","1000","1010","100","0","0010","110","0000","00","0111","101","0100","11","10","111","0110","1101","010","000","1","001","0001","011","1001","1011","1100"]

LEN = 70

def delay yabm, val
  start = yabm.count() 
  while yabm.count() < start + val do
  end
end

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~RLED)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | RLED)
end

def morus yabm, str
  i = 0
  while i < str.length do
    ledon yabm
    if str[i] == '0'
      delay yabm, LEN
    else
      delay yabm, LEN * 3
    end
    ledoff yabm
    i = i + 1
    delay yabm, LEN
  end
end


yabm = YABM.new

# JTAG(GPIOA2,4,5,6) and LED_PORT3(GPIOB5) is GPIO
#yabm.gpiosetsel(0x06, 0x1ffffff, 0x600, 0x3fff)
yabm.gpiosetsel(0x300000, 0x300000, 0, 0)

RLED = (1 << 16)

reg = yabm.gpiogetdat()
ledoff yabm

str = "mruby on yet another bare metal"

while 1 do
  i = 0
  while i < str.length do
    if str[i] == ' '
      delay yabm, LEN * 7
    elsif str[i] >= 'a' && str[i] <= 'z'
      morus yabm, MORSE_ALPH[str[i].ord - 'a'.ord]
    else
      morus yabm, MORSE_NUM[str[i].ord - '0'.ord]
    end
    i = i + 1
  end
  delay yabm, 3000
end

