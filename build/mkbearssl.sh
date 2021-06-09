#!/bin/sh

BEARSSL=bearssl-0.6

TOOLPATH=$HOME/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${PATH}:${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4

cd work

rm -rf ${BEARSSL}

tar -zxf ${BEARSSL}.tar.gz

START=`date '+%s'`

cp -r ../BearSSL/RTL.mk ${BEARSSL}/conf/

cd ${BEARSSL};patch -p1 < ../../BearSSL/bear.patch;make CONF=RTL

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"
