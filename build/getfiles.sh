#!/bin/sh

MRUBYVER=4.0.0

OS=`uname -s`

if [ "$OS" = 'Linux' ]; then
  CMD=wget
else
  CMD=fetch
fi

cd work

if [ ! -e newlib-3.0.0.20180831.tar.gz ]; then
${CMD} ftp://sourceware.org/pub/newlib/newlib-3.0.0.20180831.tar.gz
fi

if [ ! -e lwip-2.1.2.zip ]; then
${CMD} http://download.savannah.nongnu.org/releases/lwip/lwip-2.1.2.zip
fi

if [ ! -e bearssl-0.6.tar.gz ]; then
${CMD} https://bearssl.org/bearssl-0.6.tar.gz
fi

if [ ! -e ${MRUBYVER}.tar.gz ]; then
${CMD} https://github.com/mruby/mruby/archive/refs/tags/${MRUBYVER}.tar.gz
fi
