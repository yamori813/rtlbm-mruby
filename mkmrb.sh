#!/bin/sh

TARGET=`echo $1 | sed 's/\.rb//'`

OFFSET=0x100000

./mruby/build/host/bin/mrbc -E ${TARGET}.rb

./rtktools/cvimg root ${TARGET}.mrb ${TARGET}.img ${OFFSET} ${OFFSET}
