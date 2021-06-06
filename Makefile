#
# mruby on RTL8196 Bare Metal 
#

CROSS_CC = mips-cc
CROSS_LD = mips-ld
CROSS_OBJCOPY = mips-objcopy

RSDK=../rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
MRUBYDIR = mruby
NEWLIBDIR = newlib-3.0.0.20180831
LWIPDIR = lwip-2.1.2
BEARSSLDIR = bearssl-0.6

CROSS_CFLAGS = -Ibuild/work/$(NEWLIBDIR)/newlib/libc/include/ -I./$(MRUBYDIR)/include/ -Ibuild/work/$(LWIPDIR)/src/include -Ibuild/work/$(LWIPDIR)/realtek/include -Ibuild/work/$(BEARSSLDIR)/inc
CROSS_CFLAGS += -Os -g -fno-pic -mno-abicalls
CROSS_CFLAGS += -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0
CROSS_CFLAGS += -pipe -mlong-calls

CROSS_CFLAGS += -DCONFIG_SPI_STD_MODE
CROSS_CFLAGS += -DRTLBM_MRUBY_DEBUG
CROSS_CFLAGS += -DUSE_INQUEUE=1
CROSS_CFLAGS += $(MY_CFLAGS)

CROSS_LDFLAGS = -static -Lbuild/work/$(LWIPDIR)/realtek/
CROSS_LDFLAGS += -Lbuild/work/$(BEARSSLDIR)/build/
CROSS_LDFLAGS += -L$(RSDK)/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/
CROSS_LDFLAGS += -Lbuild/work/$(NEWLIBDIR)/mips/newlib/
CROSS_LDLIB = -lmruby -llwip -lbearssl -lc -lgcc
CROSS_LDSCRIPT = main.ld

CROSS_ASFLAGS = -G 0 -mno-abicalls -fno-pic -fomit-frame-pointer
CROSS_ASFLAGS += -DCONFIG_RTL865XC -D__ASSEMBLY__

#WOBJ=wlan/test.o wlan/8192cd_host.o wlan/8192cd_hw.o wlan/8192d_hw.o
OBJS = main.o uart.o rtl_timer.o net.o intr.o traps.o syscalls.o start.o inthandler.o rtl_ether.o rtl_switch.o rtl_gpio.o spi_common.o spi_flash.o xprintf.o bear.o mt19937ar.o i2c.o $(WOBJ)

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
	rm -rf *.o rtl8196d/*.o *.elf *.bin ver.c *.map 
