/* Filename: saccFilt.h
 * Authors:	Tim Yandl, Michael Liedtke, Andrew Nuxoll
 * Created: 20 Sep 2011
 * Last Modified: 24 Sep 2011
 */

#ifndef SACC_FILT
#define SACC_FILT

#include <stdio.h>
#include <stdlib.h>

//defines
#define NUM_SENSES 10           // number of binary senses that the agent has
#define TRUE 1
#define FALSE 0

// Function Prototypes
extern char * saccReceiveState(char * input);
extern int saccReceiveAction(int command);
extern void saccGetCurrSensing(char *buf);




#endif //SACC_FILT

