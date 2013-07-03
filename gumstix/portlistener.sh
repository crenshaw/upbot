#!/bin/bash
case "$1" in
    start)
	/home/root/portlistener.out &
	;;
    *)
	echo "portlistener can only be started"
	exit 1
	;;
esac

exit 0