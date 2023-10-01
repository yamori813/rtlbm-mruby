#
# morus code generator on HomeSpotCube
#

MORSE_NUM = ["01111","00111","00011","00001","00000","10000","11000","11100","11110","11111"]

MORSE_ALPH =["01","1000","1010","100","0","0010","110","0000","00","0111","101","0100","11","10","111","0110","1101","010","000","1","001","0001","011","1001","1011","1100"]

LEN = 70

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~TOP_LED1)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | TOP_LED1)
end

def morus yabm, str
  i = 0
  while i < str.length
    ledon yabm
    if str[i] == '0'
      yabm.msleep LEN
    else
      yabm.msleep LEN * 3
    end
    ledoff yabm
    i = i + 1
    yabm.msleep LEN
  end
end


yabm = YABM.new

yabm.gpiosetsel(0x003c3000, 0x003c3000, 0x00001800, 0x00001800)

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)

ALLLED = STATUS_LED1 | STATUS_LED2 | STATUS_LED3 | TOP_LED1 | TOP_LED2 | TOP_LED3

yabm.gpiosetdir(ALLLED)
reg = yabm.gpiogetdat()
yabm.gpiosetdat(reg | ALLLED)

str = "mruby on yet another bare metal"


loop do
  i = 0
  while i < str.length
    if str[i] == ' '
      yabm.msleep LEN * 7
    elsif str[i] >= 'a' && str[i] <= 'z'
      morus yabm, MORSE_ALPH[str[i].ord - 'a'.ord]
    else
      morus yabm, MORSE_NUM[str[i].ord - '0'.ord]
    end
    i = i + 1
  end
  yabm.msleep 3000
end

