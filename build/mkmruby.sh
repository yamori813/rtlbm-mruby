#!/bin/sh

MRUBYVER=4.0.0

. ../TOOLPATH.conf

CDIR=`pwd`

START=`date '+%s'`

rm -f mruby/build_config.rb.lock

cd work

if [ ! -d mruby ]; then

mkdir mruby

tar -zxf ${MRUBYVER}.tar.gz -C mruby --strip-components 1

fi

cd mruby

rm -rf build

rake MRUBY_CONFIG=${CDIR}/mruby/build_config.rb

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"
