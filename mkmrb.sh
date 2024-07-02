#!/bin/sh
# if you use multi file then you need subroutine file first

TARGET="hoge"

MRBOFFSET=0x180000

./mruby/build/host/bin/mrbc -o${TARGET}.mrb $*

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}

echo "build ${TARGET}.img"
