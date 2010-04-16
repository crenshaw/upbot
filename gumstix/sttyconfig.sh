#!/bin/bash
case "$1" in
    start)
	stty -F /dev/ttyS2 ispeed 57600 cs8 -cstopb -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke
	;;
    *)
	echo "serial port configuration can only be started"
	exit 1
	;;
esac

exit 0
