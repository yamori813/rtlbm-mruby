#
# RTL8196C makefile
#

CROSS_CFLAGS += -DCONFIG_RTL8198 -DCONFIG_RTL8198_REVISION_B
CROSS_CFLAGS += -DRTL8196=1 -DRTL8198=1
CROSS_CFLAGS += -march=5281
# This is correct. But 4181 lib is fister than 5281 lib in fib() by mruby. ???
#CROSS_LDFLAGS += -Lrsdk/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/5281/
CROSS_LDFLAGS += -Lrsdk/mips-linux/lib/gcc/mips-linux/4.4.5-1.5.5p4/4181/
OBJS += swCore.o
VMOBJ = main_8198
