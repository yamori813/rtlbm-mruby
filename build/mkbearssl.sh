#!/bin/sh

BEARSSL=bearssl-0.6

. ../TOOLPATH.conf

cd work

#rm -rf ${BEARSSL}

if [ ! -d ${BEARSSL} ]; then

tar -zxf ${BEARSSL}.tar.gz

START=`date '+%s'`

cp -r ../BearSSL/RTL.mk ${BEARSSL}/conf/

cd ${BEARSSL};patch -p1 < ../../BearSSL/bear.patch;make CONF=RTL

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"

fi
