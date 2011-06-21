#ifndef _WME_H_
#define _WME_H_

/**
* wme.h
*
* This is the header file for the set of functions
* that relate to using episodes with WME attributes.
*
* Author: Zachary Paul Faltersack
* Last edit: June 15, 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "../supervisor/vector.h"
#include "../communication/communication.h"

//Used for passing arbitrary information as agent's state
typedef struct WMEStruct {
    char* attr;                 // name of attribute
    int type;                   // var type of attribute
    union {
        int iVal;
        char cVal;
        double dVal;
        char* sVal;
    } value;                    // value of attribute
} WME;

// Episode struct for WMEs. Only difference is Vector WMEs instead of int[] sensors
typedef struct EpisodeWMEStruct
{
	Vector*	sensors;            // A Vector of WMEs
	int		now;
	int 	cmd;
} EpisodeWME;

int          compareEpisodesWME(EpisodeWME* ep1, EpisodeWME* ep2, int compCmd);
int          compareWME(WME* wme1, WME* wme2);
EpisodeWME*  createEpisodeWME(Vector* wmes);
void         displayEpisodeWME(EpisodeWME* ep);
void         displayEpisodeWMEShort(EpisodeWME* ep);
void         displayWME(WME* wme);
void         displayWMEList(Vector *sensors);
int          episodeContainsAttr(EpisodeWME* ep, char* attr);
int 		 episodeContainsReward(EpisodeWME* ep);
void         freeEpisodeWME(EpisodeWME* ep);
void         freeWME(WME* wme);
char         getCHARValWME(EpisodeWME* ep, char* attr, int* found);
double       getDOUBLEValWME(EpisodeWME* ep, char* attr, int* found);
int          getINTValWME(EpisodeWME* ep, char* attr, int* found);
char*        getSTRINGValWME(EpisodeWME* ep, char* attr, int* found);
int          getNumMatches(EpisodeWME* ep1, EpisodeWME* ep2, int compareCMD);
Vector*      roombaSensorsToWME(char* dataArr);
Vector*		 stringToWMES(char* senseString);

#endif // _WME_H_
