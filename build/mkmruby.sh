#!/bin/sh

TOOLPATH=$HOME/rtl819x-toolchain/toolchain/rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714
PATH=${PATH}:${TOOLPATH}/mips-linux/bin:${TOOLPATH}/libexec/gcc/mips-linux/4.4.5-1.5.5p4

CDIR=`pwd`

START=`date '+%s'`

cd ../mruby

rm -rf build

rake MRUBY_CONFIG=${CDIR}/mruby/build_config.rb

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo ${TIME}
