#include "wme.h"

/*
 * wme.c
 *
 * This file contains the functions for using episodes
 * with WME attributes.
 *
 * Author: Zachary Paul Faltersack
 * Last updated: June 15, 2011
 */

#define DEBUGGING 1

/**
 * compareEpisodesWME
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an EpisodeWME
 * @arg ep2 a pointer to another EpisodeWME
 * @arg compCmd  TRUE indicates the comparison should include the cmd. FALSE
 *               indicates that only the sensor array should be compared
 * @return TRUE if the episodes match and FALSE otherwise
 */
int compareEpisodesWME(EpisodeWME* ep1, EpisodeWME* ep2, int compCmd)
{
    if(ep1->sensors->size != ep2->sensors->size) return FALSE;

    //Compare the commands if that's required
    if(compCmd && ep1->cmd != ep2->cmd) return FALSE;

    int i;
    for(i = 0; i < ep1->sensors->size; i++)
    {
        WME* ep1WME = getEntry(ep1->sensors, i);
        WME* ep2WME = getEntry(ep2->sensors, i);

#if USE_WALL_MARKER
        //if(!ep1WME->containsWall) continue;
        //if(!ep1WME->isFree) continue;
#endif
        if(!compareWME(ep1WME, ep2WME)) return FALSE;
    }//for

    return TRUE;
}//compareEpisodesWME

/**
 * compareWME
 *
 * This function takes two WMEs and confirms that they contain
 * the same information.
 *
 * @param wme1 A pointer to a WME
 * @param wme2 A pointer to a WME
 * @return int Boolean value
 */
int compareWME(WME* wme1, WME* wme2)
{
    // This may be more complicated than need be.
    // I feel like it's likely we don't need to switch on the type
    // except for that one is a string which requires a function
    // for comparison
    if(strcmp(wme1->attr, wme2->attr) == 0 &&
       wme1->type == wme2->type)
    {
        //%%%TEMPORARY?:  don't compare 'score' and 'steps'
        if ( (strcmp(wme1->attr, "steps") == 0)
             || (strcmp(wme1->attr, "score") == 0))
        {
            return TRUE;
        }
    
        if(wme1->type == WME_INT && 
            wme1->value.iVal == wme2->value.iVal) return TRUE;
/*
        if(wme1->type == WME_INT)
        { 
            int w1 = FALSE, w2 = FALSE;
            if(wme1->value.iVal == V_E_EMPTY ||
               wme1->value.iVal == V_E_FOOD1 ||
               wme1->value.iVal == V_E_FOOD2) w1 = TRUE;
            if(wme2->value.iVal == V_E_EMPTY ||
               wme2->value.iVal == V_E_FOOD1 ||
               wme2->value.iVal == V_E_FOOD2) w2 = TRUE;
            
            if(w1 && w2) return TRUE;
            else return wme1->value.iVal == wme2->value.iVal;
        }
*/
        if(wme1->type == WME_CHAR && 
            wme1->value.cVal == wme2->value.cVal) return TRUE;
        if(wme1->type == WME_DOUBLE && 
            wme1->value.dVal == wme2->value.dVal) return TRUE;
        if(wme1->type == WME_STRING && 
            strcmp(wme1->value.sVal, wme2->value.sVal) == 0) return TRUE;
    }
    return FALSE;
}//compareWME

/**
 * createEpisodeWME
 *
 * Takes a sensor data string and allocates space for episode
 * then parses the data and populates the episode and adds it
 * to the global episode list
 *
 * @arg sensorData char* filled with sensor information
 * @return Episode* a pointer to the newly added episode
 */
EpisodeWME* createEpisodeWME(Vector* wmes)
{
    // Timestamp to mark episodes
    static int timestamp = 0;

    if(wmes == NULL)
    {
        printf("WME vector in parse is null");
        return NULL;;
    }//if

    // Allocate space for episode and score
    EpisodeWME* ep = (EpisodeWME*) malloc(sizeof(EpisodeWME));

    // Set EpisodeWME sensors vector to our WME vector
    ep->sensors = wmes;
    ep->now = timestamp++;  // Just set it to our timestamp for now
    ep->cmd = CMD_NO_OP;       // Default command for now

    return ep;
}//createEpisodeWME

/**
 * displayEpisodeWME
 *
 * Display the contents of an Episode struct in a verbose human readable format
 *
 * @arg ep a pointer to an episode
 */
void displayEpisodeWME(EpisodeWME* ep)
{
    Vector* sensors = ep->sensors;

    printf("\nSenses:");
    int i;
    for(i = 0; i < sensors->size; i++) displayWME(getEntry(sensors, i));

    printf("\nCommand: %d", ep->cmd);
    printf("\nTime: %d\n\n", ep->now);
}//displayEpisodeWME

/**
 * displayEpisodeWMEShort
 *
 * Display the contents of an Episode struct in an abbreviated human readable
 * format
 *
 * @arg ep a pointer to an episode
 */
void displayEpisodeWMEShort(EpisodeWME * ep)
{
    if (ep == NULL)
    {
        printf("<null episode!>");
        return;
    }

    displayWMEList(ep->sensors);
    printf("{%d}", ep->cmd);
}//displayEpisodeWMEShort

/**
 * displayWME
 *
 * Print a WME to console.
 *
 * @param wme A pointer to a WME to print
 */
void displayWME(WME* wme)
{
    printf("[%s:", wme->attr);
    if(wme->type == WME_INT)    printf("%d]",wme->value.iVal);
    if(wme->type == WME_CHAR)   printf("%c]",wme->value.cVal);
    if(wme->type == WME_DOUBLE) printf("%lf]",wme->value.dVal);
    if(wme->type == WME_STRING) printf("%s]",wme->value.sVal);
}//displayWME

/**
 * displayWMEList
 *
 * Given an vector of WME structs, this method prints their contents
 *
 * @arg    int* Sensors array of ints representing the sensors (must be
 *              of length NUM_SENSORS)
 * @return int that summarizes sensors
 */
void displayWMEList(Vector *sensors)
{
    int i;
	for(i = 0; i < sensors->size; i++)
	{
		displayWME((WME*)getEntry(sensors, i));
	}//for
}//displayWMEList

/**
 * episodeContainsAttr
 *
 * Searches an episode's WMEs to determine if a WME
 * exists of the desired attribute.
 *
 * @param ep A pointer to an episode
 * @param attr A pointer to a string containing the attribute name
 * @return int A boolean indicator of WME existence.
 */
int episodeContainsAttr(EpisodeWME* ep, char* attr)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp(attr, wme->attr) == 0) return TRUE;
    }//for
    return FALSE;
}//episodeContainsAttr

/**
 * episodeContainsReward
 *
 * This routine determines whether a given episode contains a
 * goal or reward. An episode with a WME attribute named 'reward' 
 * whose value is '0' is considered to be without a reward.
 *
 * @param ep A pointer to an episode
 * @return TRUE if the entry contains a reward and FALSE otherwise
 */
int episodeContainsReward(EpisodeWME* ep)
{
    int found;
    int reward = getINTValWME(ep, "reward", &found);
    if(reward == 0 || found == FALSE) return FALSE;
    else  return TRUE;
}//episodeContainsReward

/**
 * freeEpisodeWME
 *
 * This function frees the memory associated with an EpisodeWME.
 *
 * @param ep A pointer to an EpisodeWME
 */
void freeEpisodeWME(EpisodeWME* ep)
{
    Vector* sensors = ep->sensors;
    int i;
    for(i = 0; i < sensors->size; i++)
    {
        freeWME(getEntry(sensors, i));
    }//for
    freeVector(sensors);
    free(ep);
}//freeEpisodeWME

/**
 * freeWME
 * 
 * This function frees the memory associated with a WME.
 *
 * @param wme A pointer to a WME.
 */
void freeWME(WME* wme)
{
    free(wme->attr);
    free(wme);
}//freeWME

/**
 * getCHARValWME
 *
 * Retrieve the char value from an episode
 *
 * @param ep A pointer to an episode.
 * @param attr A pointer to a string with the desired attribute name
 *
 * @return char The desired value. NULL if not found
 */
char getCHARValWME(EpisodeWME* ep, char* attr, int* found)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp(attr, wme->attr) == 0) 
        {
            (*found) = TRUE;
            return wme->value.cVal;
        }
    }//for
    (*found) = FALSE;
    return '0';;
}//getCHARValWME

/**
 * getDOUBLEValWME
 *
 * Retrieve the double value from an episode
 *
 * @param ep A pointer to an episode.
 * @param attr A pointer to a string with the desired attribute name
 *
 * @return double The desired value. NULL if not found
 */
double getDOUBLEValWME(EpisodeWME* ep, char* attr, int* found)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp(attr, wme->attr) == 0) 
        {
            (*found) = TRUE;
            return wme->value.dVal;
        }
    }//for
    (*found) = FALSE;
    return 0;
}//getDOUBLEValWME

/**
 * getINTValWME
 *
 * Retrieve the int value from an episode
 *
 * @param ep A pointer to an episode.
 * @param attr A pointer to a string with the desired attribute name
 *
 * @return int The desired value. -1 if not found
 */
int getINTValWME(EpisodeWME* ep, char* attr, int* found)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp(attr, wme->attr) == 0) 
        {
            (*found) = TRUE;
            return wme->value.iVal;
        }
    }//for
    (*found) = FALSE;
    return -1;
}//getINTValWME

/**
 * getSTRINGValWME
 *
 * Retrieve the string value from an episode
 *
 * @param ep A pointer to an episode.
 * @param attr A pointer to a string with the desired attribute name
 *
 * @return string The desired value. NULL if not found
 */
char* getSTRINGValWME(EpisodeWME* ep, char* attr, int* found)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp(attr, wme->attr) == 0) 
        {
            (*found) = TRUE;
            return wme->value.sVal;
        }
    }//for
    (*found) = FALSE;
    return NULL;
}//getSTRINGValWME

/**
 * getNumMatches
 *
 * This function compares two episodes and returns
 * the number of WMEs in common.
 *
 * @param ep1 A pointer to an episode
 * @param ep2 A pointer to an episode
 * @param compareCMD A boolean indicating if the command is important to this match
 * @return int The number of WMEs in common
 */
int getNumMatches(EpisodeWME* ep1, EpisodeWME* ep2, int compareCMD)
{
    if(compareCMD && ep1->cmd != ep2->cmd) return -1;

    int i,j, count = 0;
    if(ep1->sensors->size > ep2->sensors->size)
    {
        for(i = 0; i < ep1->sensors->size; i++)
        {
            for(j = 0; j < ep2->sensors->size; j++)
            {
                WME* wme1 = (WME*)getEntry(ep1->sensors, i);
                WME* wme2 = (WME*)getEntry(ep2->sensors, j);
#if USE_WALL_MARKER
                if(compareWME(wme1, wme2))
                {
//                    count++;
                    if(wme1->containsWall) count++;
//                    if(wme1->isFree) count++;
                }
#else
                if(compareWME(wme1, wme2)) count++;
#endif
            }//for
        }//for
    }//if
    else
    {
        for(i = 0; i < ep2->sensors->size; i++)
        {
            for(j = 0; j < ep1->sensors->size; j++)
            {
                WME* wme1 = (WME*)getEntry(ep2->sensors, i);
                WME* wme2 = (WME*)getEntry(ep1->sensors, j);
#if USE_WALL_MARKER
                if(compareWME(wme1, wme2))
                {
//                    count++;
                    if(wme1->containsWall) count++;
                }
#else
                if(compareWME(wme1, wme2)) count++;
#endif
            }//for
        }//for
    }//else

    return count;
}//getNumMatches

/**
 * roombaSensorsToWME
 *
 * This function takes the sensor string received from a Roomba
 * and converts it into the WME vector used by Ziggurat (in the
 * near future).
 *
 * @param sensorInput a char string with Roomba sensor data
 * @return Vector* A vector of WMEs created from the Roomba data
 *                 NULL if error
 */
Vector* roombaSensorsToWME(char* dataArr)
{
    int i;
    Vector* wmeVec = newVector();
    // set the episodes sensor values to the sensor data
    for(i = 0; i < NUM_SENSORS; i++)
    {
        // convert char to int and return error if not 0/1
        int bit = (dataArr[i] - '0');
        if ((bit < 0) || (bit > 1))
        {
            printf("%s", dataArr);
            return NULL;     
        }

        // Create the WME for the current sensor
        WME* wme = (WME*)malloc(sizeof(WME));
        wme->type = WME_INT;
        wme->value.iVal = bit; 
        // Here we will set the IR bit attr name to 'reward' to be consistent
        // with how we expect to mark S/F from other state definitions.
        // All other sensor attr names will be named by their index
        switch(i)
        {
            case SNSR_IR:
                wme->attr = (char*)malloc(sizeof(char) * 7); // "reward\0"
                sprintf(wme->attr, "%s", "reward");
                break;
            case SNSR_CLIFF_RIGHT:
                wme->attr = (char*)malloc(sizeof(char) * 9); // "reward\0"
                sprintf(wme->attr, "%s", "cliff_rt");
                break;
            case SNSR_CLIFF_F_RIGHT:
                wme->attr = (char*)malloc(sizeof(char) * 11); // "reward\0"
                sprintf(wme->attr, "%s", "cliff_f_rt");
                break;
            case SNSR_CLIFF_F_LEFT:
                wme->attr = (char*)malloc(sizeof(char) * 11); // "reward\0"
                sprintf(wme->attr, "%s", "cliff_f_lt");
                break;
            case SNSR_CLIFF_LEFT:
                wme->attr = (char*)malloc(sizeof(char) * 9); // "reward\0"
                sprintf(wme->attr, "%s", "cliff_lt");
                break;
            case SNSR_CASTER:
                wme->attr = (char*)malloc(sizeof(char) * 7); // "reward\0"
                sprintf(wme->attr, "%s", "caster");
                break;
            case SNSR_DROP_LEFT:
                wme->attr = (char*)malloc(sizeof(char) * 8); // "reward\0"
                sprintf(wme->attr, "%s", "drop_lt");
                break;
            case SNSR_DROP_RIGHT:
                wme->attr = (char*)malloc(sizeof(char) * 8); // "reward\0"
                sprintf(wme->attr, "%s", "drop_rt");
                break;
            case SNSR_BUMP_LEFT:
                wme->attr = (char*)malloc(sizeof(char) * 8); // "reward\0"
                sprintf(wme->attr, "%s", "bump_lt");
                break;
            case SNSR_BUMP_RIGHT:
                wme->attr = (char*)malloc(sizeof(char) * 8); // "reward\0"
                sprintf(wme->attr, "%s", "bump_rt");
                break;
        }
        // Add the new WME to the vector
        addEntry(wmeVec, wme);
    }//for
    return wmeVec;
}//roombaSensorsToWME

/**
 * stringToWMES
 *
 * This function takes a string that contains a series of WMEs
 * and converts them into actual WMEs contained in a vector.
 *
 * @param senses A char* indicating a string defining several WMEs
 *
 * A WME is defined by 3 values: Attribute Name, Attribute Type, Attribute Value
 * These are delineated with a comma. Multiple WMEs are delineated with a semi-colon.
 * Types are a single char: i (integer), c (char), d (double), s (string {char*}).
 * {:name,type,value:}
 *
 * ex:		:size,i,25:name,s,john:open,i,1:pi,d,3.14159:
 *
 * Note: The first WME is preceded by a ':' and the final WME is succeded by a ':'
 *
 * @return Vector* A vector of WMEs derived from the sense string.
 *					NULL if error
 */
Vector* stringToWMES(char* senses)
{
    Vector* wmes = newVector();
    char delims[] = ":,";
    char* result = strtok(senses, delims);
    int i;
    WME* wme;
    while(result != NULL)
    {
        wme = (WME*)malloc(sizeof(WME));
        for(i = 0; i < 3; i++)
        {
            //switch on field index
            switch(i)
            {
                case 0:
                    wme->attr = (char*)malloc(sizeof(char) * (strlen(result) + 1));
                    sprintf(wme->attr, "%s", result);
                    break;
                case 1:
                    if(strcmp(result, "i") == 0) wme->type = WME_INT;
                    else if(strcmp(result, "c") == 0) wme->type = WME_CHAR;
                    else if(strcmp(result, "d") == 0) wme->type = WME_DOUBLE;
                    else if(strcmp(result, "s") == 0) wme->type = WME_STRING;
                    else return NULL;
                    break;
                case 2:
                    if(wme->type == WME_INT) 
                    {
                        wme->value.iVal = atoi(result);
#if USE_WALL_MARKER
                        if(strcmp(wme->attr, "UL") == 0   ||
                           strcmp(wme->attr, "UM") == 0   ||
                           strcmp(wme->attr, "UR") == 0   ||
                           strcmp(wme->attr, "LT") == 0   ||
                           strcmp(wme->attr, "RT") == 0   ||
                           strcmp(wme->attr, "LL") == 0   ||
                           strcmp(wme->attr, "LM") == 0   ||
                           strcmp(wme->attr, "LR") == 0   )
                        {
                             wme->containsWall = (wme->value.iVal == V_E_WALL);
//                             wme->isFree = (wme->value.iVal == V_E_EMPTY);
                        }
                        else 
                        {
                            wme->containsWall = FALSE;
//                            wme->isFree = FALSE;
                        }
#endif
                    }
                    else if(wme->type == WME_CHAR) wme->value.cVal = (char)result[0];
                    else if(wme->type == WME_DOUBLE) wme->value.dVal = atof(result);
                    else if(wme->type == WME_STRING)
                    {
                        wme->value.sVal = (char*)malloc(sizeof(char) * (strlen(result) + 1));
                        sprintf(wme->value.sVal, "%s", result);
                    }
                    else return NULL;
                    break;
            }//switch	
            result = strtok(NULL, delims);
        }//for
        addEntry(wmes, wme);
    }//while

    return wmes;
}//stringToWMES

