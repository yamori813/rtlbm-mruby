#!/bin/sh

TERGET=main
IMG=${TERGET}.rtl

make clean
make
ENTRY=`readelf -h ${TERGET}.elf | awk '/Entry point address/{print $4}'`
echo ${ENTRY}
./rtktools/cvimg linux ${TERGET}.bin ${IMG} ${ENTRY} 30000
echo "bin
put ${IMG}
quit" | tftp 192.168.1.6
