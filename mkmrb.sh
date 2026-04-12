#!/bin/sh
# if you use multi file then you need subroutine file first

TARGET="hoge"

MRBOFFSET=0x180000

if [ -e "build/work/mruby/build/host/bin/mrbc" ]; then
./build/work/mruby/build/host/bin/mrbc -o${TARGET}.mrb $*
else
mrbc -o${TARGET}.mrb $*
fi

sha256 ${TARGET}.mrb

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}

echo "build ${TARGET}.img"
