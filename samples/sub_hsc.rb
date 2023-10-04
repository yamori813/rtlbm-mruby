#
# mruby on YABM script
#

# Homespotcube GPIO
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

