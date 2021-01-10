#
# RTL8196E makefile
#

CROSS_CFLAGS += -DCONFIG_RTL8196E -DCONFIG_RTL865XC=1
CROSS_CFLAGS += -DRTL8196E=1
CROSS_CFLAGS += -march=4181
CROSS_CFLAGS += -L./$(MRUBYDIR)/build/rtl8196/include
CROSS_LDFLAGS += -L./$(MRUBYDIR)/build/rtl8196/lib
OBJS += rtl8196d/swCore.o
VMOBJ = main_e
