#!/bin/bash
case "$1" in
    start)
	/home/root/a.out
	;;
    *)
	echo "roomba can only be started"
	exit 1
	;;
esac

exit 0