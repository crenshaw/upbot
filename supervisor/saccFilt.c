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
 *saccReceiveState() takes in a 10 bit sense and returns a thinned senses array.
 *
 * @param the 10 bit sense
 */
char * saccReceiveState(char * input)
{
    return input;
}


/**
 *saccReceiveAction() takes an integer command and forwards it to the environment
 *
 *@param int command code.
 */
int saccReceiveAction(int command)
{
    //send command to the environment
    return command;
}


/**
 *saccGetCurrSensing() takes a sensing array buffer and fills it with
 * the current sensing.
 *
 * @param buf    the 10 bit sense
 */
void saccGetCurrSensing(char *buf)
{

}
