#!/bin/sh

TERGET=$1
IMG=${TERGET}.rtl

ENTRY=`readelf -h ${TERGET}.elf | awk '/Entry point address/{print $4}'`
./rtktools/cvimg linux ${TERGET}.bin ${IMG} ${ENTRY} 30000
