#
# mruby on RTL8196 Bare Metal 
#

CROSS_CC = mips-cc
CROSS_LD = mips-ld
CROSS_OBJCOPY = mips-objcopy

MRUBYDIR = mruby
NEWLIBDIR = newlib-2.5.0.20171222
LWIPDIR = lwip-2.1.2
BARESSLDIR=BearSSL

CROSS_CFLAGS = -I./$(NEWLIBDIR)/newlib/libc/include/ -I./$(MRUBYDIR)/include/ -I./$(LWIPDIR)/src/include -I./$(LWIPDIR)/realtek/include -I$(BARESSLDIR)/inc
CROSS_CFLAGS += -march=4181 -Os -g -fno-pic -mno-abicalls
CROSS_CFLAGS += -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0
CROSS_CFLAGS += -pipe -mlong-calls

CROSS_CFLAGS += -DCONFIG_SPI_STD_MODE
CROSS_CFLAGS += -DRTLBM_MRUBY_DEBUG
CROSS_CFLAGS += -DUSE_INQUEUE=1
CROSS_CFLAGS += $(MY_CFLAGS)

CROSS_LDFLAGS = -static -L./$(MRUBYDIR)/build/realtek/lib -L./$(NEWLIBDIR)/mips/newlib/ -Lrsdk/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/4181/ -L./$(LWIPDIR)/realtek/ -L./BearSSL/build/
CROSS_LDLIB = -lmruby -llwip -lbearssl -lc -lgcc
CROSS_LDSCRIPT = main.ld

CROSS_ASFLAGS = -G 0 -mno-abicalls -fno-pic -fomit-frame-pointer
CROSS_ASFLAGS += -DCONFIG_RTL865XC -D__ASSEMBLY__

OBJS = main.o uart.o rtl_timer.o net.o intr.o traps.o syscalls.o start.o inthandler.o rtl_ether.o rtl_switch.o rtl_gpio.o spi_common.o spi_flash.o xprintf.o bear.o mt19937ar.o i2c.o

# Default configuration is 'RTL8196C'
TARGET = RTL8196C

include $(TARGET).mk

.c.o:
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c -o $@ $<

.S.o:
	$(CROSS_CC) -O2 $(CROSS_ASFLAGS) -c $<

$(VMOBJ).rtl: $(VMOBJ).elf
	$(CROSS_OBJCOPY) -O binary $(VMOBJ).elf $(VMOBJ).bin
	./mkimg.sh $(VMOBJ)

$(VMOBJ).elf: $(OBJS) $(CROSS_LDSCRIPT)
	./ver.sh
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c ver.c
	$(CROSS_LD) $(CROSS_LDFLAGS) -T $(CROSS_LDSCRIPT) -Map $(VMOBJ).map $(OBJS) ver.o $(MRBOBJ) $(CROSS_LDLIB) -o $(VMOBJ).elf

clean:
	rm -rf *.o rtl8196d/*.o *.elf *.bin ver.c *.map *.rtl
