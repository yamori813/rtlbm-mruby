#!/bin/sh

. ./TOOLPATH.conf

START=`date '+%s'`

cd rtktools;make
cd ..

rm *rtl
make clean
make TARGET=RTL8196E

make clean
make TARGET=RTL8197D_SW

make clean
make TARGET=RTL8198

make clean
make


END=`date '+%s'`

TIME=`expr ${END} - ${START}`

echo ${TIME} sec
