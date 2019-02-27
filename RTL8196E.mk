#
# RTL8196E makefile
#

CROSS_CFLAGS += -DCONFIG_RTL8196E -DCONFIG_RTL865XC=1
CROSS_CFLAGS += -DRTL8196E=1
CROSS_CFLAGS += -march=4181
CROSS_LDFLAGS += -Lrsdk/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/4181/
OBJS += rtl8196d/swCore.o
VMOBJ = main_e
