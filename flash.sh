#!/bin/sh

if [ $# -eq 2 -a "$1" = "2" ]; then
IMG=$2
IPADDR=192.168.2.1
elif [ $# -eq 2 -a "$2" = "0" ]; then
IMG=$2
IPADDR=192.168.0.1
elif [ $# -eq 1 ]; then
IMG=$1
IPADDR=192.168.1.6
else
echo "flash.sh [-02] <img file>"
exit
fi

echo $IMG

echo "target is ${IPADDR}"

PATH=`/usr/bin/dirname ${IMG}`
FILE=`/usr/bin/basename ${IMG}`

cd ${PATH} ; echo "bin
put ${FILE}
quit" | /usr/bin/tftp ${IPADDR}
