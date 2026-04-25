#!/bin/sh

OS=`uname -s`


NEWLIB=newlib-3.0.0.20180831

. ../TOOLPATH.conf

cd work

#rm -rf ${NEWLIB}

if [ ! -d ${NEWLIB} ]; then

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

fi
echo "${TIME} sec"
