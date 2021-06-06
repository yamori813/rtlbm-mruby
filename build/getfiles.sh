#!/bin/sh

OS=`uname -s`

if [ "$OS" = 'Linux' ]; then
  CMD=wget
else
  CMD=fetch
fi

cd work

${CMD} ftp://sourceware.org/pub/newlib/newlib-3.0.0.20180831.tar.gz

${CMD} http://download.savannah.nongnu.org/releases/lwip/lwip-2.1.2.zip

${CMD} https://bearssl.org/bearssl-0.6.tar.gz
