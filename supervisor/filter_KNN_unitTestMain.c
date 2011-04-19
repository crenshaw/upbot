/* Filename: main.c
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 1/27/11
 * Last Modified: 4/19/11 by Dustin Dalen
 */

//includes
#include <stdio.h>
#include <stdlib.h>
#include "filter_KNN.h"

/**
 * File Name: filter_KNN_unitTestMain.c
 * Description: This program takes input from a file that represents the agent's actions and sesnses.
 *
 * Input is in the following format:
 * (10chars of sense bits) (action bit)
 * Example:
 * 0000000000 1
 * 0000000000 3
 * (and so on)
 * 1000000000 7
 * 
 * Any input file should end with a song action. Song actions can also be in the middle of the input file.
 * 
 * param the input file to be used
 * Author: Dustin M Dalen
 * Last Edit: April 19, 2011
 *
 */


//main
int main(int argc, char * argv[])
{
	//INSTANCE VARIABLES
    int startOfRun = TRUE;
	int endOfFile = TRUE;
	int episodeNum = 0;						//which episode we are on
	int runNum = 0;							//which run we are on
	
	int i = 0;								//counter for our for loops
	char ch = 'a';							//character value to read in action one char at a time
	
	
	//begin read in files
	FILE*fp;								//pointer to file that we will read in	
	fp = fopen(argv[1], "r");				//in read mode from argument 1
	if(fp == NULL)							//file did not open correctly
	{
		printf("Error opening %s", argv[1]);
		return EXIT_FAILURE;
	}	
	
    //variables to hold the input
    char tempSense[11];
    char * sense = tempSense;
    int tempAction;
    int * action = &tempAction;
	
	for(;;)//infinite loop to read in files.
    {
        if(fscanf(fp, "%s", tempSense)==1)	//successful read of senses
            receiveState(tempSense);        //add to database
        else
            break;                          //otherwise get out of loop
        
        if(fscanf(fp, "%d", action)==1)     //successful read of action
            receiveAction(tempAction);      //add to database
        else 
            break;                          //otherwise get out of loop
    }
	
	fclose(fp);                             //close input file
    
	return EXIT_SUCCESS;
	
}



