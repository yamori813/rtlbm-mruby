#!/bin/sh

IMG=$1

echo "bin
put ${IMG}
quit" | tftp 192.168.1.6
