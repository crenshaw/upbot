#!/bin/bash

## Usage: This file must be run from inside the communications folder
    ## upbot/comunication

## Purpose: To automate collection of data, for any map and any number of trials
## Output: A folder in parent directory named after arg3. Also a info.txt file
    ## that describes the test.
## Input: The map number, the number of trials, the name of the trial.

## @Author: Dustin Dalen
## @Date modified: 4/5/2011
## @Date modified 08 Jul 2011 - made more robust, Linux-ified - :AMN:
##                          

NUM_ARGS=4

#arg1 is the map
#arg2 is how many trials
#arg3 is the anme of the trial

#variables
ALGORITHM=$1
MAP=$2
NUM_TRIALS=$3
NAME=$4

if [ $# -ne $NUM_ARGS ]; then
        echo 'USAGE: gatherData <ziggurat/mccallum> <map> <nsim> <nameOfTrial>'
        echo '!!must be in the communication folder of upbot!!'
        exit
fi
if [ -d ../$NAME ]; then
        echo "A test named: $NAME, has already been made. ABORTING"
        exit
fi

#make sure no unitTestServer processes are currently running
ps -e | grep unitTest | sed -e 's/^ *//' | sed -e 's/ .*//' | xargs kill


make virt

#Make the output file and description file.
mkdir ../$NAME
touch ../$NAME/info.txt
echo "Sorting algorithm is: $ALGORITHM" >> ../$NAME/info.txt
echo "Map is: $MAP" >> ../$NAME/info.txt
echo "Number of trials: $NUM_TRIALS" >> ../$NAME/info.txt
echo "Description of trial $NAME :" >> ../$NAME/info.txt

#change this line to vi or emacs or whatever you prefer
emacs ../$NAME/info.txt

#the main loop
for (( j = 0 ; j < $NUM_TRIALS ; j++ )); do
    #run the program with the right args
    ./unitTestServer.out $MAP &
    sleep 3 #let the program catch up to be safe
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
    grep "found a" temp | grep timestamp | sed -e 's/s at timestamp.*//g' | sed -e 's/Goal.*found//g' | sed -e 's/[a-z]//g' | sed -e 's/ //g' >> ../$NAME/results.txt
    echo '*********************************' >> ../$NAME/results.txt

    #kill the process to start again
    ps -e | grep unitTest | sed -e 's/^ *//' | sed -e 's/ .*//' | xargs kill
    sleep 3  #delay to guarantee a clean reset



done
rm temp

