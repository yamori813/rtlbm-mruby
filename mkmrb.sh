#!/bin/sh

TARGET=`echo $1 | sed 's/\.rb//'`

MRBOFFSET=0x180000

./mruby/build/host/bin/mrbc ${TARGET}.rb

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${MRBOFFSET} ${MRBOFFSET}
