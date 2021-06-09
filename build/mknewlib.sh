#!/bin/sh

OS=`uname -s`


NEWLIB=newlib-3.0.0.20180831

TOOLPATH=$HOME/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${PATH}:${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4

cd work

rm -rf ${NEWLIB}

tar -zxf ${NEWLIB}.tar.gz

sed -i -e '/__deprecated__/d' ${NEWLIB}/newlib/libc/include/stdlib.h
sed -i -e '/Unable to determine/d' ${NEWLIB}/newlib/libc/include/sys/_intsup.h

START=`date '+%s'`

if [ "$OS" = 'Linux' ]; then
cd ${NEWLIB};./configure --target=mips;make
else
ARCH=`uname -p`
cd ${NEWLIB};./configure --host=${ARCH} --target=mips;gmake
fi

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"
