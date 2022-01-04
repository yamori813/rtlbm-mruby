#!/bin/sh

TARGET="hoge"

MRBOFFSET=0x180000

./mruby/build/host/mrbc/bin/mrbc -o${TARGET}.mrb $*

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}

echo "build ${TARGET}.img"
