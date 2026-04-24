#!/bin/sh

LWIP=lwip-2.1.2

if [ -z "$RUNNER_WORKSPACE" ]; then
TOOLPATH=${HOME}/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4:${PATH}
else
TOOLPATH=${RUNNER_WORKSPACE}/rtlbm-mruby/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${PATH}:${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4
fi

cd work

rm -rf ${LWIP}

unzip ${LWIP}.zip

START=`date '+%s'`

cp -r ../lwip ${LWIP}/realtek

cd ${LWIP}/realtek;make

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"
