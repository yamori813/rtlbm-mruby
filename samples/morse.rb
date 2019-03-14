#
#
#

MORSE_NUM = ["01111","00111","00011","00001","00000","10000","11000","11100","11110","11111"]

MORSE_ALPH =["01","1000","1010","100","0","0010","110","0000","00","0111","101","0100","11","10","111","0110","1101","010","000","1","001","0001","011","1001","1011","1100"]

def delay yabm, val
  start = yabm.count() 
  while yabm.count() < start + val do
  end
end

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | RLED)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~RLED)
end

def morus yabm, str
  i = 0
  ledon yabm
  while i < str.length do
    if str[i] == '0'
      delay yabm, 100
    else
      delay yabm, 100
    end
    ledoff yabm
    i = i + 1
  end
  delay yabm, 1000
end


yabm = YABM.new

# JTAG(GPIOA2,4,5,6) and LED_PORT3(GPIOB5) is GPIO
yabm.gpiosetsel(0x06, 0x1ffffff, 0x600, 0x3fff)

# WR8165N, WR8166N
# GPIOA6 GREEN LED 0x0040
# GPIOB5 RED LED   0x2000
# GPIOA2 Slide SW  0x0004
# GPIOA5 Push SW   0x0020
# GPIOA4 ?         0x0010

GLED = 0x0040
RLED = 0x2000
SSW = 0x0004
PSW = 0x0020

yabm.gpiosetctl(~(GLED | RLED | SSW | PSW))
yabm.gpiosetdir(GLED | RLED)
reg = yabm.gpiogetdat()
yabm.gpiosetdat(reg & ~(RLED | GLED))

str = "mruby on yet another bare metal"

i = 0

while 1 do
while i < str.length do
  if str[i] == ' '
    p "word"
  elsif str[i] >= 'a' && str[i] <= 'z'
    morus yabm, MORSE_ALPH[str[i].ord - 'a'.ord]
  else
    morus yabm, MORSE_NUM[str[i].ord - '0'.ord]
  end
  i = i + 1
end
end

