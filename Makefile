#
# mruby-1.4.0 on RTL8196 Bare Metal 
#

CROSS_CC = mips-cc
CROSS_OBJCOPY = mips-objcopy
CROSS_LD = mips-ld

NEWLIBDIR = newlib-2.5.0.20171222
#MRUBYDIR = mruby-1.4.0
MRUBYDIR = mruby
LWIPDIR = lwip-2.0.3
BARESSLDIR=BearSSL

CROSS_CFLAGS = -I./$(NEWLIBDIR)/newlib/libc/include/ -I./$(MRUBYDIR)/include/ -I./$(LWIPDIR)/src/include -I./$(LWIPDIR)/rtl/include -I$(BARESSLDIR)/inc
CROSS_CFLAGS += -march=4181 -Os -g -fno-pic -mno-abicalls
CROSS_CFLAGS += -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0
CROSS_CFLAGS += -pipe -mlong-calls
CROSS_CFLAGS += -DCONFIG_RTL8196C -DCONFIG_RTL8196C_REVISION_B
CROSS_CFLAGS += -DRTL8196C=1 -DCONFIG_RTL865XC=1
CROSS_CFLAGS += -DRTL8196=1 -DRTL8196B=1
CROSS_CFLAGS += -DCONFIG_SPI_STD_MODE
CROSS_CFLAGS += -DRTLBM_MRUBY_DEBUG
CROSS_CFLAGS += -DUSE_DHCP=1

CROSS_LDFLAGS = -static -L./$(MRUBYDIR)/build/realtek/lib -L./$(NEWLIBDIR)/mips/newlib/ -Lrsdk/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/4181/ -L./$(LWIPDIR)/rtl/ -L./BearSSL/build/
CROSS_LDLIB = -lmruby -lc -lgcc -llwip -lbearssl
CROSS_LDSCRIPT = main.ld

CROSS_ASFLAGS = -G 0 -mno-abicalls -fno-pic -fomit-frame-pointer
CROSS_ASFLAGS += -DCONFIG_RTL865XC -D__ASSEMBLY__

OBJS = main.o uart.o rtl_timer.o net.o intr.o traps.o syscalls.o start.o inthandler.o rtl_ether.o rtl_switch.o swCore.o spi_common.o spi_flash.o xprintf.o bear.o mt19937ar.o time.o

all: main.bin 

start.o: start.S
	$(CROSS_CC) -O2 $(CROSS_ASFLAGS) -c start.S

inthandler.o: inthandler.S
	$(CROSS_CC) -O2 $(CROSS_ASFLAGS) -c inthandler.S

main.o: main.c hoge.rb
	./mruby-1.4.0/build/host/bin/mrbc -Bbytecode hoge.rb
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c main.c

uart.o: uart.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c uart.c

rtl_timer.o: rtl_timer.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c rtl_timer.c

net.o: net.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c net.c

intr.o: intr.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c intr.c

rtl_ether.o: rtl_ether.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c rtl_ether.c

rtl_switch.o: rtl_switch.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c rtl_switch.c

swCore.o: swCore.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c swCore.c

spi_common.o: spi_common.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c spi_common.c

spi_flash.o: spi_flash.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c spi_flash.c

traps.o: traps.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c traps.c

syscalls.o: syscalls.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c syscalls.c

xprintf.o: xprintf.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c xprintf.c

mt19937ar.o: mt19937ar.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c mt19937ar.c

time.o: time.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c time.c

bear.o: bear.c
	$(CROSS_CC) -O2 $(CROSS_CFLAGS) -c bear.c

main.elf: $(OBJS) main.ld
	$(CROSS_LD) $(CROSS_LDFLAGS) -T $(CROSS_LDSCRIPT) -Map main.map $(OBJS) $(MRBOBJ) $(CROSS_LDLIB) -o main.elf

main.bin: main.elf
	$(CROSS_OBJCOPY) -O binary main.elf main.bin

clean:
	rm -rf *.o *.elf *.bin hoge.c main.map main.rtl
