#!/bin/sh

LWIP=lwip-2.1.2

. ../TOOLPATH.conf

cd work

rm -rf ${LWIP}

unzip ${LWIP}.zip

START=`date '+%s'`

cp -r ../lwip ${LWIP}/realtek

cd ${LWIP}/realtek;make

END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo "${TIME} sec"
