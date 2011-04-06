#!/bin/bash

## Usage: This file must be run from inside the communications folder
	## upbot/comunication

## Purpose: To automate collection of data, for any map and any number of trials
## Output: A folder in parent directory named after arg3. Also a info.txt file
	## that describes the test.
## Input: The map number, the number of trials, the name of the trial.

## Author: Dustin Dalen
## Date modified: 4/5/2011

NUM_ARGS=3

#arg1 is the map
#arg2 is how many trials
#arg3 is the anme of the trial

if [ $# -ne $NUM_ARGS ]; then
        echo 'USAGE: gatherData <map> <nsim> <nameOfTrial>'
	echo '!!must be in the communication folder of upbot!!'
        exit
fi
if [ -d ../$3 ]; then
        echo "A test named: $3, has already been made. ABORTING"
        exit
fi

#variables
MAP=$1
NUM_TRIALS=$2
NAME=$3

#necessary file attributes
mkdir ../$NAME
touch ../$NAME/info.txt
echo "Description of trial $NAME :" > ../$NAME/info.txt
vi ../$NAME/info.txt

#for j in {1..10}; do
for (( j = 0 ; j < $NUM_TRIALS ; j++ )); do
	#run the program with the right args
	./unitTestServer.out $MAP &
	sleep 1
	./supervisorClient.out localhost > temp

	#clean up the text
	tail -n51 temp | head -n50 > temp
	head -n1 temp | cut -d' ' -f7 >> ../$NAME/results.txt
	for i in {1..50}; do
		tail -n49 temp | cut -d' ' -f11 >> ../$NAME/results.txt
	done
	echo 'end run' >> ../$NAME/results.txt

	#kill the process to start again
	ps | grep unitTest | head -n1 | sed -e 's/tty.*//' | xargs kill 

done
rm temp
say done
