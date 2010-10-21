#include "supervisor.h"

/*
 * This file contains the code for the Supervisor. All the functions
 * that are needed for processing raw sensor data are contained in 
 * this file as well as those for determining new commands
 *
 * Authors:      Dr. Andrew Nuxoll, Zachary Paul Faltersack, Brian Burns
 * Last updated: October 13, 2010
 */

/*
 * Minor Maintenance To-Do List
 *
 * 1.  refactor the code so that it says "action" instead of "rule" and
 *     "sequence" instead of "sequence"
 * 2.  Review updateRules() and try to simplify it.  Break it up into parts?
 * 3.  Add a method to the vector that allows insert and delete and use it?  I'm
 *     not sure that this will make the code easier to read but it might help.
 */
 

#define DEBUGGING 1


// The chance of choosing a random move
int g_randChance = 100;
// global strings for printing to console
char* g_forward = "forward";
char* g_right   = "right";
char* g_left    = "left";
char* g_adjustR = "adjust right";
char* g_adjustL = "adjust left";
char* g_blink   = "blink";
char* g_no_op   = "no operation";
char* g_song    = "song";
char* g_unknown = "unknown";

char* g_forwardS = "FW";
char* g_rightS   = "RT";
char* g_leftS    = "LT";
char* g_adjustRS = "AR";
char* g_adjustLS = "AL";
char* g_blinkS   = "BL";
char* g_no_opS   = "NO";
char* g_songS    = "SO";
char* g_unknownS = "$$";

// Keep track of goals
int g_goalCount = 0;                                            // Number of goals found so far
int g_goalIdx[NUM_GOALS_TO_FIND];


/**
 * simpleTest
 *
 * This function is called at regular intervals and processes
 * the recent sensor data to determine the next action to take.
 *
 * @param sensorInput a char string wth sensor data
 * @return int a command for the Roomba (negative is error)
 */
void simpleTest()
{
    char* noHit    = "0000000000                 ";  // no hit is zero
    char* rightHit = "0000000010                 ";  // right hit is 2
    char* leftHit  = "0000000001                 ";  // left hit is 1
    char* bothHit  = "0000000011                 ";  // both hit is 3
    char* goal     = "1000000000                 ";  // goal is 512

    char* sensors[] = {noHit, noHit, bothHit, noHit, bothHit, noHit,
                      noHit, noHit, noHit, noHit, bothHit, noHit,
                      noHit, noHit, goal};
    
    int cmds[] = {CMD_LEFT, CMD_FORWARD, CMD_RIGHT, CMD_FORWARD,
                 CMD_RIGHT, CMD_FORWARD, CMD_LEFT, CMD_LEFT,
                 CMD_LEFT, CMD_FORWARD, CMD_LEFT, CMD_FORWARD,
                 CMD_RIGHT, CMD_FORWARD};

    int i;

    for (i = 0; i < 15; i++)
    {
        // Create new Episode
        printf("Creating and adding episode...\n");
        Episode* ep = createEpisode(sensors[i]);
        // Add new episode to the history
        addEpisode(g_epMem->array[0], ep);
        printf("Episode created\n");

        updateRules(0);

        // If we found a goal, send a song to inform the world of success
        // and if not then send ep to determine a valid command
        if(episodeContainsGoal(ep, FALSE))
        {
            ep->cmd = CMD_SONG;
            if (g_plan != NULL)
            {
                Route *r = g_plan->array[MAX_LEVEL_DEPTH - 1];
                r->needsRecalc = TRUE;
            }
        }
        else
        {
            //issue the next command from the cmds array
            ep->cmd = cmds[i];
        }

#ifdef DEBUGGING
        printf("Level 0 Action Rules>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        displayRules(g_actionRules->array[0], g_epMem->array[0]);

        printf("Level 0 Sequences>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        displaySequences(g_sequenceRules->array[0]);
#endif
        
        // Print out the parsed episode if not in statsMode
        if(g_statsMode == 0)
        {
            displayEpisode(ep);
        }
    }
}// simpleTest

/**
 * tick
 *
 * This function is called at regular intervals and processes
 * the recent sensor data to determine the next action to take.
 *
 * @param sensorInput a char string wth sensor data
 * @return int a command for the Roomba (negative is error)
 */
int tick(char* sensorInput)
{
    // Create new Episode
    printf("Creating and adding episode...");
    Episode* ep = createEpisode(sensorInput);

    // Add new episode to the history
    addEpisode(g_epMem->array[0], ep);
    printf("Episode created\n");

	updateRules(0);

    // If we found a goal, send a song to inform the world of success
    // and if not then send ep to determine a valid command
    if(episodeContainsGoal(ep, FALSE))
    {
        ep->cmd = CMD_SONG;
        if (g_plan != NULL)
        {
            Route *r = g_plan->array[MAX_LEVEL_DEPTH - 1];
            r->needsRecalc = TRUE;
        }
    }
    else
    {
        //%%%AMN:  What is this code??
        // Use shortcircuiting to only call takeNextStep if a goal has been
        // found. This is because there is no route to follow until after
        // the first goal.
        //if(g_goalCount <= 0 || setCommand2(ep))
        //{
        ep->cmd = chooseCommand();
        //}
    }
    
#ifdef DEBUGGING
    printf("Level 0 Action Rules>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displayRules(g_actionRules->array[0], g_epMem->array[0]);

    printf("Level 0 Sequences>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displaySequences(g_sequenceRules->array[0]);
#endif

    // Print out the parsed episode if not in statsMode
    if(g_statsMode == 0)
    {
        displayEpisode(ep);
    }

    return ep->cmd;
}// tick

/**
 * createEpisode
 *
 * Takes a sensor data string and allocates space for episode
 * then parses the data and populates the episode and adds it
 * to the global episode list
 *
 * @arg sensorData char* filled with sensor information
 * @return Episode* a pointer to the newly added episode
 */
Episode* createEpisode(char* sensorData)
{
    // Allocate space for episode and score
    Episode* ep = (Episode*) malloc(sizeof(Episode));
    int retVal;     

    // If error in parsing print appropriate error message and exit
    if((retVal = parseEpisode(ep, sensorData)) != 0)
    {
        char errBuf[1024];
        sprintf(errBuf, "Error in parsing: %s\n", sensorData);
        perror(errBuf);
        exit(retVal);
    }
    return ep;
}// createEpisode

/**
 * parseEpisode
 *
 *        dataArr contains string of the following format
 *        0000000011  <will be timestamp> 
 *
 * Take a raw sensor packet from Roomba and parse information
 * out to an instance of Episode.
 *
 * @arg parsedData A pointer to an Episode to be populated
 * @arg dataArr the char array that contains the raw sensor data
 * @return int an error code
 *
 */
int parseEpisode(Episode * parsedData, char* dataArr)
{
    // temporary timestamp
    static int timeStamp = 0;
    int i; // index

    if(dataArr == NULL)
    {
        printf("data arr in parse is null");
        return -1;
    }

    // set the episodes sensor values to the sensor data
    for(i = 0; i < NUM_SENSORS; i++)
    {
        // convert char to int and return error if not 0/1
        int bit = (dataArr[i] - '0');
        if ((bit < 0) || (bit > 1))
        {
            printf("%s", dataArr);
            return -1;      
        }

        // else save sensor bit
        parsedData->sensors[i] = bit;
    }

    if(g_connectToRoomba == 1)
    {
        // Pull out the timestamp
        parsedData->now = timeStamp++;
    }else
    {
        // Alg for determining timestamp from string of chars
        int time = 0;
        for(i = NUM_SENSORS; dataArr[i] != '\0'; i++)
        {
            if(dataArr[i] != ' ')
            {
                time = time * 10 + (dataArr[i] - '0');
            }
            if(dataArr[i] == ' ' && time != 0)
            {
                break;
            }
        }
        // Store the time
        parsedData->now = time;
    }

    // Found a goal so decrease chance of random move
    if(parsedData->sensors[SNSR_IR] == 1)
    {
        DECREASE_RANDOM(g_randChance);
        g_goalIdx[g_goalCount] = parsedData->now;
        g_goalCount++;
    }

    // Command gets a default value for now
    parsedData->cmd = CMD_NO_OP;

    return 0;
}// parseEpisode

/**
 * updateRules                    *RECURSIVE*
 *
 * This method is designed to do a semantic rules update.  A semantic rule
 * consists of 1 or more episodes that make up the LHS of the rule followed by
 * the next episode which is its RHS.  A semantic rule represents a hypothesis
 * that "this sequence of events leads to this event."
 *
 * CAVEAT: This is a rather complex method.  Breaking it up into smaller methods
 * would make it more complex, in our opinion.  Please rely upon our liberal
 * comments to guide you.  Also consult research notes.
 *
 * @arg    level index into g_epMem and g_actionRules where the memory needs to updated    
 * @return int   error code
 *
 */
int updateRules(int level)
{
    printf("Entering level %i\n", level);
    // Ensure that the level is within the accepted range for the vectors
    if(level < 0 || level >= MAX_LEVEL_DEPTH)
    {
        return -3;
    }

    // Create pointers to the two associated vectors we'll be working with
    Vector* ruleList = g_actionRules->array[level];
    Vector* episodeList = g_epMem->array[level];
    Vector* sequenceList = g_sequenceRules->array[level];

    //You need a minimum of two episodes to make a rule
    if(episodeList->size <= 1)
    {
        return -1;
    }

    //If the most recent complete episode was a goal then return.  We
    //don't want any goals on the LHS of any rule.
    if (episodeContainsGoal(episodeList->array[episodeList->size - 2], level))
    {
        return -2;
    }

    //Create a candidate rule that we would create from this current
    //episode.  We won't add it to the rule list if an identical rule
    //already exists.
    Rule* newRule            = (Rule*) malloc(sizeof(Rule));
    newRule->level           = level;
    newRule->epmem           = episodeList;
    newRule->outcome         = episodeList->size - 1;
    newRule->index           = episodeList->size - 2;
    newRule->length          = 1;
    newRule->freq            = 1;
    newRule->overallFreq     = NULL;
    newRule->cousins         = NULL;
    newRule->isIndeterminate = FALSE;
    newRule->containsGoal    = episodeContainsGoal(episodeList->array[episodeList->size - 1], level);

    //initialize containsStart to TRUE if this will be the very first rule
    if (episodeList->size == 2)
    {
        newRule->containsStart          = TRUE;
    }
    //initialize containsStart to TRUE if the previous rule contained a goal
    else if (((Rule *)ruleList->array[ruleList->size - 1])->containsGoal)
    {
        episodeContainsGoal(episodeList->array[episodeList->size - 1], level);
        newRule->containsStart          = TRUE;
    }
    //default:  containsStart=FALSE;
    else
    {
        newRule->containsStart          = FALSE;
    }
    
    printf("candidate rule: ");
    fflush(stdout);
    displayRule(newRule);
    printf("\n");
    fflush(stdout);

    //Iterate over every rule in the list and compare it to our new
    //candidate rule.  If the candidate is unique, it'll be added to
    //the rule list.  If it's a partial match (same LHS , different
    //RHS) but can't be made unique without increasing size of LHS
    //then create a pool of percentage rules.  If the candidate
    //matches an existing rule, it'll be discarded and the existing
    //rule's frequency will be updated
    int i,j;
    int matchComplete = FALSE;
    int addNewRule = TRUE;
    Rule* updateExistingRule = NULL;
    for(i = 0; i < ruleList->size; i++)
    {
        //Compare the i-th rule to the candidate rule
        Rule* curr = (Rule*)ruleList->array[i];

        for(j = 0; j < newRule->length; j++)
        {
            //Find out if the j-th part of the LHS matches
            if (compare(episodeList, newRule->index - j, curr->index - j, level))
            {
#if DEBUGGING
                printf("found match between %i-th entries of: ", j);
                displayRule(curr);
                printf(" and ");
                displayRule(newRule);
                printf("\n");
                fflush(stdout);
#endif

                //If the LHS match so far but we haven't reached the end
                //of either rule then continue comparing them
                if (newRule->length > j+1 && curr->length > j+1)
                {
                    continue;
                }

                //If we've matched to the end and they are the same
                //length, then the LHS's match.
                if(newRule->length == curr->length)
                {
                    //If the candidate rule has matched a percentage
                    //rule then there may already be a matching cousin
                    //out there
                    if(curr->isIndeterminate)
                    {
#if DEBUGGING
                        printf("comparing cousins: \n");
#endif
                        int k;
                        //Iterate over cousins and find one with same outcome as candidate rule
                        for(k = 0; k < curr->cousins->size; k++)
                        {
                            Rule* cousin = curr->cousins->array[k];

#if DEBUGGING
                            printf("\t");
                            displayRule(cousin);
                            printf(" AND ");
                            displayRule(newRule);
                            printf("\n");
                            fflush(stdout);
#endif

                            //If we find one with same outcome, increase
                            //frequency and inform not to add rule
                            if (compare(episodeList, newRule->outcome,
                                        cousin->outcome, level))
                            {
                                
                                cousin->freq++;
                                addNewRule = FALSE;
                                updateExistingRule = cousin;
                                break;
                            }
                        }//for 

                        //If no cousins match candidate rule, add it
                        //as a new cousin
                        if(addNewRule)
                        {
#if DEBUGGING
                            printf("new cousin is unique.  Adding...\n");
#endif
                            newRule->isIndeterminate = TRUE;
                            newRule->overallFreq = curr->overallFreq;
                            newRule->cousins = curr->cousins;

                            addRule(newRule->cousins, newRule, FALSE);
                        }

                        // Regardless of whether candidate rule is
                        // unique we need to increase overall
                        // frequency for all cousins in the list and
                        // end the matching process
                        (*(curr->overallFreq))++;
                        matchComplete = TRUE;
                    }
                    else    //Found a LHS match to a non-percentage rule
                    {
                        //Now see if the RHS of both rules match
                        if (compare(episodeList, newRule->outcome,
                                    curr->outcome, level))
                        {
                            //We have a complete match between the
                            //candidate and an existing rule, so just
                            //update the existing rule
                            curr->freq++;

                            //Done with update
                            matchComplete = TRUE;
                            addNewRule = FALSE;
                            updateExistingRule = curr;
                        }
                        else    //RHS does not match
                        {
#if DEBUGGING
                            printf("LHS match but RHS doesn't while comparing to %i...\n", i);
                            fflush(stdout);
#endif
                            // We want to expand the newRule and curr
                            // to create (hopefully) distinct rules
                            // There are 3 reasons this may not work.

                            // 1. Expanding curr/newRule would include
                            //    a goal on LHS
                            // 2. Expanding current rule would
                            //    overflow episodic memory
                            // 3. Current rule is already maximum length

                            //Check for reason #1:  Expansion creates
                            //goal on LHS
                            int newLHSEntryIndex = newRule->index - newRule->length;

                            if (episodeContainsGoal(episodeList->array[newLHSEntryIndex],
                                             level))
                            {
#if DEBUGGING
                                printf("NewRule expands into goal at index: %i\n",
                                       newLHSEntryIndex);
                                fflush(stdout);
#endif

                                //the new rule can't be expanded so we
                                //consider it degenerate so just abort
                                //and create no new rules or updates
                                matchComplete = TRUE;
                                addNewRule = FALSE;
                            }

                            //Check for reason #2: no room to expand
                            else if(curr->index - curr->length < 0)
                            {
#if DEBUGGING
                                printf("avail space: %i,  curr expands outside goal\n",
                                       curr->index - curr->length);
                                fflush(stdout);
#endif

                                //The current rule can't be expanded
                                //so we consider it degenerate and
                                //replace it with the new rule.
                                curr->index                             = newRule->index;
                                curr->outcome               = newRule->outcome;
                                curr->length                        = newRule->length;
                                curr->freq                                  = 1;

                                //done with update
                                matchComplete = TRUE;
                                addNewRule = FALSE;
                            }

                            //if the newRule is currently shorter than
                            //the current rule, then it can safely be
                            //expanded 
                            else if (newRule->length < curr->length)
                            {
                                newRule->length++;

#if DEBUGGING
                                printf("partial match with curr, extending new rule to %i\n",
                                       newRule->length);
                                fflush(stdout);
                                printf("new candidate: ");
                                displayRule(newRule);
                                printf("\n");
                                fflush(stdout);
#endif
                            }

                            //If the current rule can be expanded then
                            //expand both the current and candidate rules
                            else if(curr->length < MAX_LEN_LHS)
                            {
#if DEBUGGING
                                printf("len of curr rule (%i) = %i < %i so increasing to %i\n",
                                       i, curr->length, MAX_LEN_LHS, curr->length+1);
                                fflush(stdout);
#endif

                                //both current rule and new rule can
                                //be expanded so do so in hopes that they will
                                //end up different
                                curr->length++;
                                curr->freq = 1;
                                newRule->length++;


#if DEBUGGING
                                printf("new curr:   ");
                                displayRule(curr);
                                printf("\n");

                                printf("new cand: ");
                                displayRule(newRule);
                                printf("\n");
                                fflush(stdout);
#endif

                            }
                            else  //current rule can't be expanded without
                            //exceeding max length (reason #3)
                            {
#if DEBUGGING
                                printf("cousins\n");
                                fflush(stdout);
#endif

                                // We need to convert both the current rule and
                                // the candidate rule into percentage rules

                                // allocate cousins list and add both peer
                                // percentage rules into same cousins list
                                curr->cousins = newVector();
                                addRule(curr->cousins, curr, TRUE);
                                addRule(curr->cousins, newRule, TRUE);
                                newRule->cousins = curr->cousins;

                                //Update rules
                                curr->isIndeterminate = TRUE;
                                newRule->isIndeterminate = TRUE;
                                curr->overallFreq = (int*) malloc(sizeof(int));
                                newRule->overallFreq = curr->overallFreq;
                                *(curr->overallFreq) = curr->freq + 1;

                                //We're done with this match
                                matchComplete = TRUE;
                                addNewRule = TRUE;
                            }// else
                        }// else
                    }// else
                }// if
                else // newRule and curr have different lengths
                {
                    //If we make it here, the candidate rule and
                    //current rule are different lengths but they do
                    //match up to the length of the shorter rule.

                    //If the candidate is longer then consider it a
                    //degenerate rule and stop
                    if (newRule->length > curr->length)
                    {
                        matchComplete = TRUE;
                        addNewRule = FALSE;
#if DEBUGGING
                        printf("newRule matches but is bigger than current rule.  Aborting.\n");
                        fflush(stdout);
#endif
                    }
//===============================================================================
// I think that this code needs to probably be expanded to prevent the newRule
// from expanding beyond its limits, as above.
                    //If the new rule can be expanded, try doing so to
                    //see if that makes it unique
                    else if(newRule->length < MAX_LEN_LHS)
                    {
                        //-----This is the part that I added
                        int newLHSEntryIndex = newRule->index - newRule->length;

                        if (episodeContainsGoal(episodeList->array[newLHSEntryIndex],
                                         level))
                        {
#if DEBUGGING
                            printf("2. NewRule expands into goal at index: %i\n",
                                   newLHSEntryIndex);
                            fflush(stdout);
#endif

                            //the new rule can't be expanded so we
                            //consider it degenerate so just abort
                            //and create no new rules or updates
                            matchComplete = TRUE;
                            addNewRule = FALSE;
                        }
                        //----------------------------------
                        else
                        {
                            newRule->length++;

#if DEBUGGING
                            printf("expanded new rule to len %i\n",
                                   newRule->length);
                            printf("new candidate: ");
                            displayRule(newRule);
                            printf("\n");
                            fflush(stdout);
#endif
                        }
//============================================================================                                          
                    }
                    else
                    {
                        //Should never happen 
                        printf("Nux was wrong\n");
                        fflush(stdout);
                    }
                }// else
            }// if
            else // j-th episodes in rules do not match
            {
                //The current rule's nth entry doesn't match so we can
                //abort the comparison even if the candidate rule has
                //more entries in its LHS
                break;
            }
        }// for

        // if matched rule break out of loop and free memory 
        if(matchComplete == TRUE)
        {
            break;
        }
    }// for

    //Add the new rule
    if(addNewRule == TRUE)
    {
        printf("Adding new rule: ");
        displayRule(newRule);
        printf("\n");
        addRule(ruleList, newRule, FALSE);

        // set this flag so that we recursively update the next level
        // with this rule
        updateExistingRule = newRule;
       
    }
    else
    {
        free(newRule);
    }

    //If we have added a new rule, or found an existing rule that matches the
    //current situation then updateExistingRule will contain a pointer to that
    //rule (otherwise NULL)
    if(updateExistingRule != NULL)
    {
printf("Adding Rule: ");
displayRule(updateExistingRule);
printf(" to current sequence\n");

        // add most recently seen action rule to current sequence
        Vector* currSequence = sequenceList->array[sequenceList->size - 1];
        addActionToSequence(currSequence, updateExistingRule);

        // if the rule we just added is percentage rule or contains a
        //goal then end the current sequence and start a new one
        if (updateExistingRule->isIndeterminate
            || updateExistingRule->containsGoal)
        {
            assert(currSequence->size > 1); 
                                	
			// if the sequence we just completed already exists then
			// reset the vector's size to 0
			// This will allow updateRules to reuse the same vector
			// without needing to free memory
            Vector* duplicate = containsSequence(sequenceList, currSequence, TRUE);
			if(duplicate != NULL)
			{
				currSequence->size = 0;
                // this duplicate sequence becomes the next episode in the
                // next level's episodic memory
                if (level + 1 < MAX_LEVEL_DEPTH)
                {
                    episodeList = g_epMem->array[level + 1];
                    addEntry(episodeList, duplicate);
                }
			}
			else
			{
                // this newly completed sequence becomes the next episode in the
                // next level's episodic memory
                if (level + 1 < MAX_LEVEL_DEPTH)
                {
                    episodeList = g_epMem->array[level + 1];
                    addEntry(episodeList, currSequence);
                }
            	// create an vector to hold the next sequence 
            	currSequence = newVector();
            	addEntry(sequenceList, currSequence);
			}
            
            //typically the next sequence starts with the action that
            //ended the last sequence.  (Exception:  last action
            //contains a goal)
            if (!updateExistingRule->containsGoal)
            {
                addActionToSequence(currSequence, updateExistingRule);
            }

            // this sequence then becomes an episode in our next level
            // so we need recursive call to update.
            if(level + 1 < MAX_LEVEL_DEPTH)
            {
                updateRules(level + 1);
            }
        }//if
        
    }//if

    return 0;
}// updateRules

/**
 * addEpisode
 *
 * Add new episode to episode history vector
 *
 * @arg episodes pointer to vector containing episodes
 * @arg item pointer to episode to be added
 * @return int status code (0 == success)
 */
int addEpisode(Vector* episodes, Episode* item)
{
    return addEntry(episodes, item);
}// addEpisode

/**
 * addActionToSequence
 *
 * Add an action rule to a sequence
 *
 * @arg sequence pointer to vector containing action rules in sequence
 * @arg action rule pointer to action rule to be added
 * @return int status code (0 == success)
 */
int addActionToSequence(Vector* sequence, Rule* action)
{
    return addEntry(sequence, action);
}// addActionToSequence

/**
 * addRule
 *
 * Add the given rule to the rules array and checks if the rule
 * already exists
 *
 * @arg rules A pointer to the vector of rules
 * @arg item A pointer to the Rule we're adding
 * @arg checkRedundant Boolean to determine if we need to check if rule exists
 *
 * @return int A status code
 */
int addRule(Vector* rules, Rule* item, int checkRedundant)
{
    if(checkRedundant && rules->size > 0)
    {
        int i;
        for(i = 0; i < rules->size - 1; i++)
        {
            if(rules->array[i] == item)
            {
                return -1;
            }
        }
    }

    return addEntry(rules, item);
}// addrule

/**
 * displayEpisode
 *
 * Display the contents of an episode
 *
 * @arg ep a pointer to an episode
 */
void displayEpisode(Episode * ep)
{
    int i;
    printf("\nSensors:    ");

    // iterate through sensor values and print to stdout
    for(i = 0; i < NUM_SENSORS; i++)
    {
        printf("%i", ep->sensors[i]);
    }

    // print rest of episode data to stdout
    printf("\nTime stamp: %i\nCommand:    %i\n\n", (int)ep->now, ep->cmd);
}// displayEpisode

/**
 * displayRules
 *
 * prints a human-readable version of a vector of rules along with the
 * last 20 episodes in episodic memory that the rules were generated from.
 *
 * @arg ruleList     the rules to display
 * @arg episodeList  the episodes used to create this list 
 */
void displayRules(Vector *ruleList, Vector *episodeList)
{
    //don't print empty lists
    if (ruleList->size == 0) return;

    int i;
    for(i = 0; i < ruleList->size; i++)
    {
        printf("%3i. ", i);
        displayRule(ruleList->array[i]);
        printf("\n");
    }

    //determine if episodeList contains sequences or Episode structs
    int isEpList = ((Rule *)(ruleList->array[0]))->level == 0;

    //print the last 20 episodic memories
    printf("EpMem: ");
    for(i = 1; i <= 20; i++)
    {
        if (i > episodeList->size) break;

        if (isEpList)
        {
            Episode *ep = (Episode*)episodeList->array[episodeList->size - i];
            printf("%i %s, ", interpretSensorsShort(ep->sensors), interpretCommandShort(ep->cmd));
        }
        else //episodeList contains Rule structs
        {
            displayRule(episodeList->array[episodeList->size - i]);
        }

    }//for
    printf("\n");
    printf("---------------\n");
}// displayRules

/**
 * displaySequence
 *
 * prints a human-readable version of a sequence
 *
 * @arg sequence a vector pointer to the sequence we want to display
 */
void displaySequence(Vector* sequence)
{
    int i; // counting variable
    // dont print an empty sequence
    if (sequence -> size < 1) return;

    printf("{");
    for(i = 0; i < sequence->size; i++)
    {
        displayRule(sequence->array[i]);
        printf(":");
    }
    printf("}\n");
}//displaySequence

/**
 * displaySequenceShort
 *
 * prints a human-readable version of a sequence in a condensed format
 *
 * @arg sequence a vector pointer to the sequence we want to display
 */
void displaySequenceShort(Vector* sequence)
{
    int i,j; // counting variable
    // dont print an empty sequence
    if (sequence -> size < 1) return;

    printf("{");
    for(i = 0; i < sequence->size; i++)
    {
		// grab the level from the current sequence
		int currLevel = ((Rule*)sequence->array[i])->level;
		// grab the associated action rules this sequence is composed from
		Vector* actionList = g_actionRules->array[currLevel];
		// search for the rule that the sequence is referring to
		for(j = 0; j < actionList->size; j++)
		{
			// print the index of the current action
			if(actionList->array[j] == sequence->array[i])
			{
				printf("%i", j);
				break;
			}
		}
		// delimiter
        printf(",");
    }
    printf("}\n");
}//displaySequenceShort

/**
 * displaySequences
 *
 * @arg sequences the sequences we want to print
 *
 */
void displaySequences(Vector* sequences)
{
    int i, j;
    for(i = 0; i < sequences->size; i++)
    {
        printf("%3i. ", i);
        displaySequenceShort(sequences->array[i]);
        printf("\n");
    }
}

/**
 * displayRule                        *RECURSIVE*
 *
 * prints a human-readable version of a rule.  If the rule is a
 * sequence it makes recursive calls until it reaches the base rule.
 * The rules are printed backward (LHS on the right and vice versa)
 * for easy comparison to other rules.
 */
void displayRule(Rule* rule)
{
    int i,j;


    //Print the RHS
    if (rule->level == 0)
    {
        Vector* episodeList = (Vector*)g_epMem->array[0];
        printf("%i", interpretSensorsShort(((Episode*)episodeList->array[rule->outcome])->sensors));
    }
    else
    {
        printf("{ ");
        displaySequence((Vector*)rule->epmem->array[rule->outcome]);
        printf(" }");
    }

    //Print the arrow 
    if(rule->isIndeterminate)
    {
        printf(" <-%2i- ", rule->freq * 100 / *(rule->overallFreq));
    }
    else
    {
        printf(" <---- ");
    }

    //Print the LHS
    for(i = 0; i < rule->length; i++)
    {
        if (rule->level == 0)
        {
            printf("%i", interpretSensorsShort(((Episode*)rule->epmem->array[rule->index - i])->sensors)); 
            printf("%s", interpretCommandShort(((Episode*)rule->epmem->array[rule->index - i])->cmd));
        }
        else //sequence
        {
            printf("{ ");
            displaySequence((Vector*)rule->epmem->array[rule->index - i]);
            printf(" }");
        }

    }//for

}// displayRule

/**
 * chooseCommand_SemiRandom
 *
 * This function selects a random command that would create a new action rule
 * based upon the agent's most recent sensing.  If no such new action rule can
 * be made, then it just chooses a random command without qualification.
 *
 * CAVEAT:  This routine assumes that CMD_NO_OP is the lowest numbered command
 * and that they commands are numbered consecutively afterwards.
 *
 */
int chooseCommand_SemiRandom()
{
    int i;                      // iterator
    
    //Make an array of boolean values (one per command) and init them all to
    //TRUE. This array will eventually indicates whether the given command would
    //create a unique episode (TRUE) or not (FALSE).
    int numCmds = LAST_MOBILE_CMD - CMD_NO_OP;       // number of commands
    int valid[numCmds];
    for(i = 0; i < numCmds; i++)
    {
        valid[i] = TRUE;        // innocent until proven guilty
    }
    
    //Find all the level 0 episodes whose sensor values match the most recent
    //sensing.  Mark the commands associated with those episodes as invalid
    Vector *epList = g_epMem->array[0];
    Episode *lastEpisode = (Episode*)epList->array[epList->size - 1];
    for(i = 0; i < epList->size - 2; i++) // iterate over all but most recent
    {
        Episode *currEpisode = (Episode *)epList->array[i];
        if (compareEpisodes(currEpisode, lastEpisode, FALSE))
        {
            int index = currEpisode->cmd - CMD_NO_OP;
            valid[index] = FALSE; // guilty!  (episode already exists)
        }
    }//for

    
    //Select a random starting position in the valid array.  This indicates our
    //default choice for a command.  NOTE: We start the search in a random
    //position so that the agent won't always default to the lowest numbered
    //command.
    int start = (rand() % numCmds); // random start

    //Starting with the random position and treating "valid" as a circular array
    //scan until the first valid command is found.
    for(i = 0; i < numCmds; i++)
    {
        int index = (start + i) % numCmds; 
        if (valid[index])
        {
            return index + CMD_NO_OP;
        }
    }

    //if no valid command is found, then just use the randomly selected default
    return start + CMD_NO_OP;

}//chooseCommand_SemiRandom

/**
 * chooseCommand
 *
 * This function decides what command to issue next.  Typically this selection
 * will be the next step in a plan.  However, the agent may decide to modify the
 * plan as part of the command selection process.  If no plan exists, a random
 * command is selected.
 *
 * TODO: This code needs to be modified in the future to support a possibility
 *        of modifying the current plan with a replacement rule before selecitn
 *        the next command in the plan.
 *
 * @return int the command that was chosen
 */
int chooseCommand()
{
    //If there is no plan, try to create one
    if (g_plan == NULL)
    {
        g_plan = initPlan();
        
    }//if

    //If there still is no plan at this point then that means the agent doesn't
    //have enough experience yet.  Select a semi-random command that would
    //create a new action rule.  
    if (g_plan == NULL)
    {
        return chooseCommand_SemiRandom();
    }//if

    //%%%If the current sensing violates the expectaitons of the plan, we need
    //%%%to replan.
    
    //If we've reached this point then there is a plan.  At this point, the
    //agent should select the next step in the plan.  
    return chooseCommand_WithPlan();

}// chooseCommand

/**
 * displayRoute
 *
 * Display the current route. Prints the rules that make up
 * the route in reverse order to make it easier visually to
 * read the steps.
 *
 */
void displayRoute()
{
    Vector* semMem = g_actionRules->array[0];
    Route * route = g_plan->array[0];
    Vector* actions = route->actions;

    printf("Current Route: \n");

    int i;
    for(i = actions->size - 1; i >= 0; i--)
    {
        if(i == route->currAction)
        {
            printf("-->");
        }
        printf("\t{ ");
        displayRule(semMem->array[*((int*)actions->array[i])]);
        printf(" }\n");
    }// for
    printf("\n");
}// displayRoute


/**
 * setCommand2
 *
 * This method sets the command for the most recent episode.
 * If the route needs to be recalculated, it does that and 
 * then it takes the next step on the route.
 *
 * @arg ep A pointer to the newest episode
 * @return int status code
 */
int setCommand2(Episode* ep)
{
    printf("Checking if route needs recalculating\n");
    fflush(stdout);

    // If last command did not result in expected outcome
///%%%% Temporarily commented out.  This needs to be fixed! -:AMN:
    // if(g_route->needsRecalc || !nextStepIsValid())
    // {
    //     printf("Route needs to be recalculated\n");
    //     fflush(stdout);
    //     planRoute(ep);
    //     printf("Route is recalculated\n", stderr);
    //     fflush(stdout);
    // }
    // else
    // {
    //     printf("Route is good\n");
    // }

    displayRoute();

    // attempt to follow the route and filter back success/error code
    return takeNextStep(ep);
}// setCommand2

/**
 * getStartAction
 *
 * Find out if any action in this sequence contains a start state and return the
 * index of that action
 *
 * @arg seq  the sequence to search
 * 
 * @return the index of an action in the sequence that contains a start or -1 if
 *         there is no such action
 */
int getStartAction(Vector *seq)
{
    int j;                      // loop iterator
    int result = -1;            // default: not found 

    //Iterate over each action in the sequence
    for(j = 0; j < seq->size; j++)
    {
        Rule *currRule = (Rule *)seq->array[j];

        //If this action contains a start state then record its index and break
        if (currRule->containsStart)
        {
            result = j;
            break;          // move on to the next sequence
        }
    }//for

    return result;
}//getStartAction

/**
 * getGoalAction
 *
 * Find out if any action in this sequence contains a goal and return the index
 * of that action
 *
 * @arg seq  the sequence to search
 * 
 * @return the index of an action in the sequence that contains a goal or -1 if
 *         there is no such action
 */
int getGoalAction(Vector *seq)
{
    int j;                      // loop iterator
    int result = -1;            // default: not found 

    //Iterate over each action in the sequence
    //(Note:  iteration is descending since the last rule is most likely to
    //have the goal)
    for(j = seq->size-1; j >= 0; j--)
    {
        Rule *currRule = (Rule *)seq->array[j];

        //If this action contains a goal then record its index and break
        if (currRule->containsGoal)
        {
            result = j;
            break;          // move on to the next sequence
        }
    }//for

    return result;
}//getGoalAction

/**
 * initRouteFromSequence
 *
 * this method initializes a given route with the rules in a given sequence.
 * The internal variables are set as if the agent is about to begin the
 * sequence.
 *
 * CAVEAT: The Route and its internal vectors should already be allocated.
 * CAVEAT: Any existing route will be lost
 *
 * @arg route the route to initialize
 * @arg seq   the sequence to initialize with with
 *
 */
void initRouteFromSequence(Route *route, Vector *seq)
{
    //Do the easy ones first
    route->currAction = 0;
    route->needsRecalc  = FALSE;

    //Calculate the level of this route by looking at a rule in the sequence
    Rule *r = (Rule *)seq->array[0];
    route->level        = r->level;

    //This route will contain only one sequence, the given one
    addEntry(route->sequences, seq);

    //Insert all the given sequence's actions into this route
    int i;
    for(i = 0; i < seq->size; i++)
    {
        r = (Rule *)seq->array[i];
        addEntry(route->actions, r);
    }
    
}//initRouteFromSequence


/**
 *initRoute
 *
 * This method uses Dijkstra's algorithm to find a shortest path from the
 * start state to the goal state at a given level.
 *
 * @arg level     is the level at which this route should exist
 * @arg newRoute  is the Route struct to populate with this new route.
 * 
 * @return a success/error code
 */
int initRoute(int level, Route* newRoute)
{
    // instance variables
    Vector* route;          // the ordered list of sequences stored as
                            // int indices into ruleList
    int i,j;                // counting variable
    Vector* sequenceRules;  // pointer to sequence rules in level
    Vector* candRoutes = newVector();  //candidate Route structs to return to caller

    // check that the requested level exists
    if (g_sequenceRules->size < level) return LEVEL_NOT_POPULATED;

    /*--------------------------------------------------------------------------
     * Find all the sequences at the given level that contain a goal.  Create an
     * incomplete route with each one that ends with this sequence
     */
    
    // Iterate over each sequence at the given level
    // (Note:  iteration is descending so as to give preference to the most
    // recently added sequences)
    sequenceRules = g_sequenceRules->array[level];
    for (i = sequenceRules->size-1; i >= 0; i--)
    {
        Vector *currSeq = (Vector*)sequenceRules->array[i];

        //If the sequence contains a goal, then reate a partial route from it
        //and add it to the candidates list
        int actionIdx = getGoalAction(currSeq);
        if (actionIdx >= 0)
        {
                //Create the route
                Route *route = (Route*)malloc(sizeof(Route));
                route->actions = newVector();
                route->sequences = newVector();
                initRouteFromSequence(route, currSeq);

                //add to the candidates list
                newEntry(candRoutes, route);

        }//if
    }//for

    //Make sure that at least one candidate route was found
    if (candRoutes->size == 0) return NO_GOAL_IN_LEVEL;

    /*--------------------------------------------------------------------------
     * Sort the candidate routes from shortest to longest (selection sort)
     */
    for(i = 0; i < candRoutes->size-1; i++)
    {
        int indexOfSmallest = i;
        int smallestLen = ((Route *)candRoutes->array[i])->actions->size;
        for(j = i+1; j < candRoutes->size; j++)
        {
            int len = ((Route*)candRoutes->array[i])->actions->size;
            if (len < smallestLen)
            {
                indexOfSmallest = j;
                smallestLen = len;
            }
        }

        //swap
        void *temp = candRoutes->array[i];
        candRoutes->array[i] = candRoutes->array[indexOfSmallest];
        candRoutes->array[indexOfSmallest] = temp;
    }//for
    
    /*--------------------------------------------------------------------------
     * Iterate over the candidate routes expanding them until the shortest
     * route to the goal is found (Dijkstra's algorithm)
     */
    //Iterate over all candidate routes
    //(Note: the size of the candRoutes vector will grow as the search continues)
    for(i = 0; i < candRoutes->size; i++)
    {
        //Find the shortest route that hasn't been examined yet
        Route *route = (Route *)candRoutes->array[i];
        int routeLen = route->actions->size;
        int routePos = i;
        for(j = i+1; j < candRoutes->size; j++)
        {
            Route *possiblyShorter = (Route *)candRoutes->array[j];
            int psLen = possiblyShorter->actions->size;

            //If a shorter one is found, update route, routeLen and routePos
            if (psLen < routeLen)
            {
                route = possiblyShorter;
                routeLen = psLen;
                routePos = j;
            }
        }

        //Move this shortest unexamined route to the current position in the
        //candRoutes vector via a swap
        if (routePos != i)
        {
            void *tmp = candRoutes->array[i];
            candRoutes->array[i] = candRoutes->array[j];
            candRoutes->array[j] = tmp;
        }
                

        //If the first sequence in this route contains a start state, we're
        //done.  Copy the details of this route to the newRoute struct we were
        //given and exit the loop.
        Vector *firstSeq = (Vector *)route->sequences->array[0];
        int actionIdx = getStartAction(firstSeq);
        if (actionIdx >= 0)
        {
            newRoute->level = route->level;
            newRoute->sequences = cloneVector(route->sequences);
            newRoute->actions = cloneVector(route->actions);
            newRoute->currAction = 0;
            newRoute->needsRecalc = FALSE;

            break;
        }

        //Search all the sequences to find any that meet these criteria:
        //1.  the last action in the sequence matches the first action of the first
        //    sequence in the current candidate route.  (In other words, a
        //    sequence that can extend that candidate route.)
        //2.  the sequence is not already in the candidate route
        Rule *firstRule = (Rule *)firstSeq->array[0];
        for (i = 0; i < sequenceRules->size; i++)
        {
            Vector *currSeq = (Vector*)sequenceRules->array[i];

            //If this sequence can't extend the candidate route then skip ite
            Rule *lastRule = (Rule *)currSeq->array[currSeq->size-1];
            if (lastRule != firstRule) continue;
            
            //Verify this sequence hasn't been used already (equivalent of "node
            //already visited" in Dijkstra's formal algorithm)
            if (findEntry(route->sequences, currSeq)) continue;

            //If we've reached this point, then we can create a new candidate
            //route that is an extension of the first one
            Route *newCand = (Route*)malloc(sizeof(Route));
            newCand->actions = newVector();
            newCand->sequences = newVector();
            initRouteFromSequence(newCand, currSeq);
            addVector(newCand->actions, route->actions);
            addVector(newCand->sequences, route->sequences);
            
            //Add this new candidate route to the candRoutes array
            addEntry(candRoutes, newCand);
        }//for
        
    }//for

    //Clean up the RAM in the candRoutes list
    for(i = 0; i < candRoutes->size; i++)
    {
        Route *route = (Route *)candRoutes->array[i];
        freeVector(route->actions);
        freeVector(route->sequences);
        free(route);
    }
    freeVector(candRoutes);
    
    return SUCCESS;
}//initRoute

/**
 * initPlan
 *
 * this method uses Djikstra's algorithm to find a shortest path from the
 * start state to the goal.  A plan is a vector of routes (one per level)
 * 
 * @return a pointer to the plan or NULL if no plan was found
 */
Vector* initPlan()
{
    int i;                      // iterator

    //find out what the highest level is that has any sequences
    int level = 0;
    for(i = MAX_LEVEL_DEPTH-1; i > 0; i--)
    {
        Vector *seqList = (Vector *)g_sequenceRules->array[i];
        if (seqList->size > 1)
        {
            level = i;
            break;
        }
    }//for


    //If there are no sequences above level 0, we can't make a plan
    if (level == 0)
    {
        return NULL;
    }

    //Initialize an empty plan.  This will eventually be our return value.
    Vector *resultPlan = newPlan();

    //Try to initialize the route at the current level
    int retVal = initRoute(level, (Route *)resultPlan->array[level]);
    if (retVal != 0)
    {
        //Give up if no route can be found
        freePlan(resultPlan);
        return NULL;
    }

    //Initialize the route at subsequent levels.  Each route is based on the
    //current sequence in the route at the previous level
    for(i = level - 1; i >= 0; i--)
    {
        //Get the first sequence in the previous level's route
        Route *parentRoute = (Route *)resultPlan->array[i+1];
        Vector *parentSeq = (Vector *)parentRoute->sequences->array[0];

        //initialize this route with it
        Route *currRoute = (Route *)resultPlan->array[i];
        initRouteFromSequence(currRoute, parentSeq);
    }//for

    return resultPlan;
}// initPlan



/**
 * newPlan()
 *
 * This method initializes a new, empty plan.  A plan is a Vector of Route
 * structs where each Route corresponds to a different level.  By convention,
 * the first entry in the vector is at the highest level.
 */
Vector *newPlan()
{
    int i;
    Vector *newPlan = newVector(); // return value
    for(i = 0; i < MAX_LEVEL_DEPTH; i++)
    {
        Route *r = (Route*)malloc(sizeof(Route));

        r->actions = newVector();
        r->sequences = newVector();
        r->currAction = 0;
        r->needsRecalc  = FALSE;	
        
        addEntry(newPlan, r);
    }//for
    
    
}//newPlan

/**
 * freePlan()
 *
 * This method frees the memory used by a plan.  It frees the following memory:
 *  - the vector that the plan is made of
 *  - the Route structs at each level of the plan
 *  - the vectors in the Route structs that contain pointers
 * 
 */
void freePlan(Vector *plan)
{
    int i;
    if (plan == NULL) return;

    //for each route in the plan
    for(i = 0; i < plan->size; i++)
    {
        //get a pointer to the route
        Route *r = (Route *)plan->array[i];
        if (r == NULL) continue;

        //free both the route and it's internal vectors
        if (r->actions != NULL) freeVector(r->actions);
        if (r->sequences != NULL) freeVector(r->sequences);
        free(r);
    }//for

    freeVector(plan);

}//freePlan


/**
 * findTopMatch
 */
int findTopMatch(double* scoreTable, double* indvScore, int command)
{
    int i, max;
    double maxVal = 0.0, tempVal = 0.0;
    Vector* episodeList = g_epMem->array[0];
    for(i = episodeList->size - 1; i >= 0; i--)
    {
        tempVal = scoreTable[i] + (((Episode*)(episodeList->array[i]))->cmd == command ? NUM_TO_MATCH : 0);

        if(tempVal > maxVal)
        {
            max = i;
            maxVal = tempVal;
        }
    }

    indvScore[command] = maxVal;

    return max;
}// findTopMatch

/**
 * compareEpisodes
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an episode
 * @arg ep2 a pointer to another episode
 * @arg compCmd  TRUE indicates the comparison should include the cmd. FALSE
 *               indicates that only the sensor array should be compared
 * @return TRUE if the episodes match and FALSE otherwise
 */
int compareEpisodes(Episode* ep1, Episode* ep2, int compCmd)
{
    int i;

    // Iterate through the episodes' sensor data and determine if they are
    // matching episodes
    for(i = 0; i < NUM_SENSORS; i++)
    {
        if(ep1->sensors[i] != ep2->sensors[i])
        {
            return FALSE;
        }
    }

    //Compare the commands if that's required
    if(compCmd)
    {
        // add num_sensors to give cmd 1/2 value
        if(ep1->cmd != ep2->cmd)
        {
            return FALSE;
        }
    }

    return TRUE;
}// compareEpisodes

/**
 * compareRules
 *
 * Compares two rules to each other and returns TRUE if they have the
 * same LHS and RHS.  This is a shallow comparison.
 *
 * @arg r1  first rule to compare
 * @arg r2  second rule to compare
 * @return TRUE if the rules match and false otherwise
 */
int compareRules(Rule* r1, Rule* r2)
{
    //Make sure that both rules use the same episodic memory
    if (r1->epmem != r2->epmem) return FALSE;

    //Make sure that both rules have the same index
    if (r1->index != r2->index) return FALSE;

    //Make sure that both rules have the same length
    if (r1->length != r2->length) return FALSE;

    return TRUE;
}//compareRules

/**
* containsEpisode
*
* Check a list of episodes for a previous occurence of a particular
* episode
*
* @arg episodeList A vector containing a series of sequences
* @arg ep  a pointer to the episode struct to search for
* @arg ignoreSelf If TRUE then if you find ep in episodeList ignore
*                 it (i.e., we are looking for a duplicate not itself)
* @return a pointer to the matching episode if it is found, NULL otherwise
*/
Episode* containsEpisode(Vector* episodeList, Episode* ep, int ignoreSelf)
{
	int i;
    
	for(i = 0; i < episodeList->size; i++)
	{
        Episode *toCompare = (Episode*)episodeList->array[i];

        //See if the episodes match
		if(compareEpisodes(toCompare, ep, TRUE))
        {
            //Handle the ignoreSelf parameter
            if (!ignoreSelf) return toCompare;
            if (ep != toCompare) return toCompare;
        }
	}
	// otherwise it's not there
	return NULL;
}//containsEpisode

/**
* containsSequence
*
* Check a list of sequences for a previous occurence of a particular
* sequence
*
* @arg sequenceList A vector containing a series of sequences
* @arg seq A vector containing our current sequence
* @arg ignoreSelf If TRUE then if you find seq in sequenceList ignore
*                 it (i.e., we are looking for duplicate not itself)
* @return a pointer to the sequence if it is found, NULL otherwise
*/
Vector* containsSequence(Vector* sequenceList, Vector* seq, int ignoreSelf)
{
	int i;
    
	// determine what the stopping point should be
	for(i = 0; i < sequenceList->size; i++)
	{
		// if we come across the sequence in the list then return 'found'
        Vector *toCompare = (Vector*)sequenceList->array[i];
		if(compareSequences(toCompare, seq))
        {
            if (!ignoreSelf) return toCompare;

            if (seq != toCompare) return toCompare;
        }
	}
	// otherwise it's not there
	return NULL;
}//containsSequence

/**
* compareSequences
*
* Compare two sequences and return TRUE if they contain the same
* sequence of action rules. 
*
* @arg seq1 A vector containing the first sequence
* @arg seq2 A vector containing the second sequence
* @return TRUE if they are a match
*/
int compareSequences(Vector* seq1, Vector* seq2)
{
	// make sure they contain the same number of rules
	if(seq1->size != seq2->size) return FALSE;
	// make sure they are at the same level
	if(((Rule*)seq1->array[0])->level != ((Rule*)seq2->array[0])->level) return FALSE;

	int i;
	// iterate through and compare corresponding action rules
	for(i = 0; i < seq1->size; i++)
	{
		// if the pointers are not the same then we have no match
		if(seq1->array[i] != seq2->array[i]) return FALSE;
	}
	// success, we have a match
	return TRUE;
}//compareSequences

/**
 * compare
 *
 * This is a general purpose compare that works with either Episode or
 * Rule structs.  It calls compareRules or compareEpisodes as needed.
 *
 * @arg list        a pointer to a Vector of either Episode or Rule structs
 * @arg i1         index of first entry to compare
 * @arg i2         index of second entry to compare
 * @arg level      is TRUE if vec contains episodes and FALSE if it
 *                 contains rules
 * @return TRUE if the rules match and false otherwise
 */
int compare(Vector *list, int i1, int i2, int level)
{
    if (!level)
    {
        //If it's a base rule we need to know if one of the entries
        //we're comparing has no RHS yet.
        int noRHS = (i1 == list->size - 1) || (i2 == list->size - 1);

        //Determine a match score
        return compareEpisodes(list->array[i1],
                               list->array[i2], !noRHS);
    }
    else //sequence
    {
        return compareRules(list->array[i1], list->array[i2]);
    }
}//compare

/**
 * episodeContainsGoal
 *
 * This routine determines whether a given episode contains a
 * goal. Depending on the level the episode may be an Episode struct
 * or a sequence.
 *
 * @arg entry      a pointer to an Episode or Rule struct
 * @arg level      is TRUE if entry is an Episode (false for a rule)
 * @return TRUE if the entry is a goal and FALSE otherwise
 */
int episodeContainsGoal(void *entry, int level)
{
    if (level == 0)
    {
        Episode* ep = (Episode *)entry;

        //For base rules, a goal is indicated by the IR sensor on the episode
        return ep->sensors[SNSR_IR];
    }
    else //sequence
    {
        Vector *sequence = (Vector *)entry;
        printf("sequence->size=%d\n", (int)(sequence->size));
        displaySequence(sequence);
        Rule *rule = (Rule *)sequence->array[sequence->size - 1];
        
        //For a sequence, a goal is indicated by "containsGoal"
        return rule->containsGoal;
    }
}//episodeContainsGoal


/**
 * initSupervisor
 *
 * Initialize the Supervisor vectors
 * 
 */
void initSupervisor()
{
    g_epMem         = newVector();
    g_actionRules   = newVector();
    g_sequenceRules = newVector();

    int i;
    for(i = 0; i < MAX_LEVEL_DEPTH; i++)
    {
        Vector* temp = newVector();
        addEntry(g_epMem, temp);

        temp = newVector();
        addEntry(g_actionRules, temp);

        temp = newVector();
        addEntry(g_sequenceRules, temp);

        // pad sequence rule vector to avoid crash on first call of updateRules()
        addEntry(g_sequenceRules->array[i], newVector());
    }

    g_plan = NULL;              // no plan can be made at this point

    g_connectToRoomba       = 0;
    g_statsMode             = 0;


    // seed rand 
    srand(time(NULL));
    
}// initSupervisor

/**
 * endSupervisor
 *
 * Free the memory allocated for the Supervisor
 */
void endSupervisor() 
{
    // counting variables
    int i, j, k;

    // assume that the number of sequences,
    // actions and episodes is the same, level-wise
    for(i = MAX_LEVEL_DEPTH - 1; i >= 0; i--)
    {
        // Create pointers to the two associated vectors we'll be working with
        Vector* ruleList = g_actionRules->array[i];
        Vector* episodeList = g_epMem->array[i];
        Vector* sequenceList = g_sequenceRules->array[i];
        
        // cleanup sequences at the current level
        for(j = 0; j < sequenceList->size; j++)
        {
            freeVector((Vector*)sequenceList->array[j]);  
        }
        freeVector(sequenceList);

        //cleanup actions at the current level
        for(j = 0; j < ruleList->size; j++)
        {
            //If this is an inconsistent rule we need to clean up
            //the cousins list before we deallocate the rules
            Vector* cousins = ((Rule*)ruleList->array[j])->cousins;
            if(cousins != NULL)
            {

                //Make sure no rule has a reference to the cousins
                //list anymore.
                for(k = 0; k < cousins->size; k++) 
                {
                    ((Rule*)cousins->array[k])->cousins = NULL;
                }

                //now it's save to free the list
                freeVector(cousins);
            }

            free((Rule*)ruleList->array[j]);
        }//for
        freeVector(ruleList);

        //cleanup the episodes at this level
        for(j = 0; j < episodeList->size; j++)
        {
            //at level 0, this is an array of pointers to Episode structs
            //which need to be cleaned up.
            if (i == 0)
            {
                for(k = 0; k < episodeList->size; k++)
                {
                    Episode *ep = (Episode *)episodeList->array[k];
                    free(ep);
                }
            }

            //now safe to free the list
        }//for
        freeVector((Vector*)episodeList->array[j]);
        
        
    }//for

    //free the global list
    freeVector(g_epMem);
    freeVector(g_actionRules);

    //%%%TODO: free g_plan
    
    printf("end of function\n");
}// endSupervisor

/**
 * interpretCommand
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg cmd Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommand(int cmd)
{
    switch(cmd)
    {
        case CMD_NO_OP:
            return g_no_op;
            break;
        case CMD_FORWARD:
            return g_forward;
            break;
        case CMD_LEFT:
            return g_left;
            break;
        case CMD_RIGHT:
            return g_right;
            break;
        case CMD_BLINK:
            return g_blink;
            break;
        case CMD_ADJUST_LEFT:
            return g_adjustL;
            break;
        case CMD_ADJUST_RIGHT:
            return g_adjustR;
            break;
        case CMD_SONG:
            return g_song;
            break;
        default:
            return g_unknown;
            break;
    }
}// interpretCommand

/**
 * interpretCommandShort
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg cmd Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommandShort(int cmd)
{
    switch(cmd)
    {
        case CMD_NO_OP:
            return g_no_opS;
            break;
        case CMD_FORWARD:
            return g_forwardS;
            break;
        case CMD_LEFT:
            return g_leftS;
            break;
        case CMD_RIGHT:
            return g_rightS;
            break;
        case CMD_BLINK:
            return g_blinkS;
            break;
        case CMD_ADJUST_LEFT:
            return g_adjustLS;
            break;
        case CMD_ADJUST_RIGHT:
            return g_adjustRS;
            break;
        case CMD_SONG:
            return g_songS;
            break;
        default:
            return g_unknownS;
            break;
    }
}// interpretCommandShort

/**
 * interpretSensorsShort
 *
 * Return an integer that summaries the sensor values in an episdode.
 * Since all sensors are binary we can combine them all into one
 * binary integer.
 *
 * @arg int* Sensors array of ints representing the sensors (must be
 *           of length NUM_SENSORS)
 * @return int that summarizes sensors
 */
int interpretSensorsShort(int *sensors)
{
    int i, result = 0;
    int sumval = 1;  //This is always = to 2^i
    for(i = NUM_SENSORS-1; i >= 0; i--)
    {
        if (sensors[i])
        {
            result += sumval;
        }

        sumval *= 2;
    }

    return result;
}// interpretSensorsShort

