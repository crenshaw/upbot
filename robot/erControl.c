/**
 * erControl.c
 *
 * erControl contains the functions used to set and change the
 * event responder.
 *
 * @author Matt Holland
 * @since July 2013
 */

#include "erControl.h"

/**
 * setEventResponder() 
 *
 * Sets the event responde based on the name provided 
 *
 * @param erName: char array containing the name of the er to open
 * @param myER: pointer to the empty/old event responder to overwriten
 *
 * TODO: set responder based on int id
 * - or -
 * TODO: set responder via internet!
 */
void setEventResponder(char * erName, eventResponder* myER) {	
	cleanupER(myER);

	selectNextER(erName, myER);

	//now set the clock for the next event responder if needed
	if (myER->states[myER->curState].clockTime > 0) {
		setClock(myER->states[myER->curState].clockTime,0);
	}
}


/**
 * cleanupER()
 *
 * This function will free all dynamic memory within
 * myER and zero everything on the stack
 *
 * @param myER: pointer to the eventResponder you want to clear
 *
 */
void cleanupER(eventResponder* myER) {

	int i;
	for (i=0;i<myER->stateCount;++i) {
		free(myER->states[i].transitions);
	}
	free(myER->states);

}


