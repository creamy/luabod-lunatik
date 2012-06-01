#!/bin/sh
if [ $# -lt 1 ] ; then
	echo "usage: $0 <cmajor>"
else
	rm /dev/lunatik
	mknod /dev/lunatik c $1 0
fi

