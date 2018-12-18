#!/bin/sh

IMG=$1

PATH=`/usr/bin/dirname ${IMG}`
FILE=`/usr/bin/basename ${IMG}`

cd ${PATH} ; echo "bin
put ${FILE}
quit" | /usr/bin/tftp 192.168.1.6
