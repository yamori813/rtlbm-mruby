#
# RTL8197D with RTL8367RB makefile
#

CROSS_CFLAGS += -DCONFIG_RTL8196D -DCONFIG_RTL8198=1 -DCONFIG_RTL865XC=1
CROSS_CFLAGS += -DRTL8196=1 -DRTL8198=1 -DCONFIG_SW_8367R=1
CROSS_CFLAGS += -march=5281
CROSS_CFLAGS += -Irtl8367r -Irtl8367r/asicdrv -Irtl8367r/asicdrv/basic
CROSS_LDFLAGS += -L./$(MRUBYDIR)/build/rtl8196/lib
#CROSS_LDFLAGS += -L./$(MRUBYDIR)/build/rtl8198/lib
OBJS += rtl8197d/swCore.o
#OBJS += rtl8367r/rtk_api.o
#OBJS += rtl8367r/smi.o
#OBJS += rtl8367r/gpio.o
#OBJS += rtl8367r/rtl8367b_asicdrv_phy.o
#OBJS += rtl8367r/rtl8367b_asicdrv_cputag.o
#OBJS += rtl8367r/rtl8367b_asicdrv.o
#OBJS += rtl8367r/rtl8367b_asicdrv_port.o
VMOBJ = main_sw
