/* Filename: hallucinogen.c
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 2/22/11
 * Last Modified: 4/19/11 by Ben Walker
 *
 * Description: This file is used to add in simulated sensory input to the agent. The function 
 * insertConfusion allows each of the bits in the 10 character array to be manipulated
 * on an individual basis. insertConfusion does protect against the overwriting of the known important
 * bits. This is stored in the variable gold. Helper functions are used to apply different effects to each
 * sense bit. 
 */

#include <stdio.h>
#include "hallucinogen.h"

//instance variables
char gold[] = "1000000011";                 //1's indicate an important bit in the 10 character array of senses

/**
 * chaos returns a random bit as a '1' or '0'. 50/50 chance of a '1' or '0'.
 */
char chaos()
{
    int num = 10*rand();
	if(num < 5)
		return '0';
	else 
		return '1';    
}

/**
 * day always returns '1'
 */
char day()
{
    return '0';
}
 

/**
 *exor returns the value when two bits are exclusive or'd
 * @param, the indicies in the sense array to xor
 */
char exor(int a, int b, char * senses)
{
    int num1 = '0' - senses[a];                 //convert to ints for xor function
    int num2 = '0' - senses[b];
    int result = num1 ^ num2;
    return '0'+result;                          //convert back to proper char value
}

/**
 *night always returns '0'
 */
char night()
{
    return '0';   
}


/**
 *insertConfusion takes in the current sense from the agent and compares against gold to find what senses
 * the agent is actually using. The current sense then has a function applied to each bit that is
 * not currently in use by the agent. The modified current state is returned.
 */
void insertConfusion(char * inputSense)
{
    int i=0;
    for (i; i<NUM_SENSES2; i++)               //only go through the information bits
    {
        if(gold[i]!='1')                      //if the bit is not designated to be saved
        {
            switch (i) {                        //switch to allow for control over the behavior
                case 0:                         //of each bit
                    inputSense[i] = chaos();    //moot
                    break;
                case 1:
					inputSense[i] = chaos();
                    break;
                case 2:
					inputSense[i] = chaos();
                    break;
                case 3:
					inputSense[i] = chaos();
                    break;
                case 4:
					inputSense[i] = chaos();
                    break;
                case 5:
					inputSense[i] = chaos();
                    break;
                case 6:
					inputSense[i] = chaos();
                    break;
                case 7:
					inputSense[i] = chaos();
                    break;
                case 8:
                    inputSense[i] = chaos();
                    break;
                case 9:
                    inputSense[i] = chaos();
                    break;
                default:                        //just in case we get near the time bits, do nothing.
                    break;
            }
        }
    }
}

