#
# mruby on YABM script
#

# Homespotcube GPIO

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)

ALLLEDS = (STATUS_LED1 | STATUS_LED2 | STATUS_LED3 |
  TOP_LED1 | TOP_LED2 | TOP_LED3)

def gpioinit(yabm) 
  yabm.gpiosetsel(0x003c300c, 0x003c300c, 0x00001800, 0x00001800)

  reg = yabm.gpiogetctl()
  reg &= ~ALLLEDS
  reg &= ~TOP_BUTTON
  yabm.gpiosetctl(reg)

  reg = yabm.gpiogetdir()
  reg |= ALLLEDS
  reg &= ~TOP_BUTTON
  yabm.gpiosetdir(reg)

  reg = yabm.gpiogetdat()
  reg |= ALLLEDS
  yabm.gpiosetdat(reg)
end

