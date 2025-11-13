#!/bin/sh
# if you use multi file then you need subroutine file first

TARGET="hoge"

MRBOFFSET=0x180000

./build/work/mruby/build/host/bin/mrbc -o${TARGET}.mrb $*

sha256 ${TARGET}.mrb

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}

echo "build ${TARGET}.img"
