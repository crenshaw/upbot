/* Filename: hallucinogen.h
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 2/23/11
 * Last Modified: 2/23/11 by Dustin Dalen
 */


//prototypes
extern char * insertConfusion(char * inputSenes);   //main function to do all replacements
char chaos();                                       //random
char exor(int a, int b, char * senses);             //xor two bits
char night();                                       //always 0
char day();                                         //always 1

//number of senses
#define NUM_SENSES2 10
