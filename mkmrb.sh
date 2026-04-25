#!/bin/sh
# if you use multi file then you need subroutine file first

if [ $# -eq 0 ]; then
echo "usage: mkmrb.sh <files>"
else
TARGET=`basename $(eval echo '$'{$#}) | sed 's/\..*//'`

MRBOFFSET=0x180000

if [ -e "build/work/mruby/build/host/bin/mrbc" ]; then
./build/work/mruby/build/host/bin/mrbc -o${TARGET}.mrb $*
else
mrbc -o${TARGET}.mrb $*
fi

sha256 ${TARGET}.mrb

if [ -e "rtktools/cvimg" ]; then
./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}
else
cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}
fi

echo "build ${TARGET}.img"
fi
