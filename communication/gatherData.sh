#!/bin/bash

## Usage: This file must be run from inside the communications folder
	## upbot/comunication

## Purpose: To automate collection of data, for any map and any number of trials
## Output: A folder in parent directory named after arg3. Also a info.txt file
	## that describes the test.
## Input: The map number, the number of trials, the name of the trial.

## @Author: Dustin Dalen
## @Date modified: 4/5/2011

NUM_ARGS=4

#arg1 is the map
#arg2 is how many trials
#arg3 is the anme of the trial

#variables
ALGORITHM=$1
MAP=$2
NUM_TRIALS=$3
NAME=$4
make

if [ $# -ne $NUM_ARGS ]; then
        echo 'USAGE: gatherData <ziggurat/mccallum> <map> <nsim> <nameOfTrial>'
		echo '!!must be in the communication folder of upbot!!'
        exit
fi
if [ -d ../$NAME ]; then
        echo "A test named: $NAME, has already been made. ABORTING"
        exit
fi

make

#Make the output file and description file.
mkdir ../$NAME
touch ../$NAME/info.txt
echo "Sorting algorith is: $ALGORITHM" >> ../$NAME/info.txt
echo "Map is: $MAP" >> ../$NAME/info.txt
echo "Number of trials: $NUM_TRIALS" >> ../$NAME/info.txt
echo "Description of trial $NAME :" >> ../$NAME/info.txt

#change this line to vi or emacs or whatever you prefer
emacs ../$NAME/info.txt

#the main loop
for (( j = 0 ; j < $NUM_TRIALS ; j++ )); do
	#run the program with the right args
	./unitTestServer.out $MAP &
	sleep 1 #let the program catch up to be safe
	if [ "$ALGORITHM" == "ziggurat" ]; then
		./supervisorClient.out localhost > temp
	elif [ "$ALGORITHM" == "mccallum" ]; then
		./mccallumClient.out localhost > temp
	else
		echo "learning algorithm: $ALGORITHM not found. EXITING"
		rm -rf ../$NAME
		exit
	fi

	#clean up the output text
	if [ "$ALGORITHM" == "ziggurat" ]; then
		tail -n51 temp | head -n50 > temp
		head -n1 temp | cut -d' ' -f7 >> ../$NAME/results.txt
		tail -n49 temp | cut -d' ' -f11 >> ../$NAME/results.txt
		echo '*********************************' >> ../$NAME/results.txt
	else   #it's mccallum
		grep "Goal" temp > temp2
		head -n1 temp2 | cut -d' ' -f6 >> ../$NAME/results.txt
		tail -n49 temp2 | cut -d' ' -f5 >> ../$NAME/results.txt
		echo '*********************************' >> ../$NAME/results.txt
		rm temp2
	fi
	
	#kill the process to start again
	ps | grep unitTest | head -n1 | sed -e 's/tty.*//' | xargs kill

done
rm temp
