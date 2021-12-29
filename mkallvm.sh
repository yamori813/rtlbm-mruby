#!/bin/sh

TOOLPATH=$HOME/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${PATH}:${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4

START=`date '+%s'`

cd rtktools;make
cd ..

rm *rtl
make clean
make TARGET=RTL8196E
mv main.rtl main_e.rtl
make clean
make TARGET=RTL8197D_SW
mv main.rtl main_8197d_sw.rtl
make clean
make TARGET=RTL8198
mv main.rtl main_8198.rtl
make clean
make


END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo ${TIME}
