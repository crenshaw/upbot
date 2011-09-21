/* Filename: saccFilt.c
 * Authors:	Michael Liedtke, Tim Yandl, Andrew Nuxoll
 * Created: 20 Sep 2011
 * Last Modified: -
 *
 * TODO 
 *
 **/
 

#include "saccFilt.h"
#include "../communication/communication.h"
#include "supervisor.h"


/* 
 *receiveState() takes in a 10 bit sense and returns a thinned senses array.
 *
 * @param the 10 bit sense
 */
char * receiveState(char * input)
{
    return input;
}


/**
 *receiveAction() takes an integer command and forwards it to the environment
 *
 *@param int command code.
 */
int receiveAction(int command)
{
    //send command to the environment
    return command;
}


/**
 *getCurrSaccSensing() takes a sensing array buffer and fills it with
 * the current sensing.
 *
 * @param buf    the 10 bit sense
 */
void getCurrSaccSensing(char *buf)
{

}
