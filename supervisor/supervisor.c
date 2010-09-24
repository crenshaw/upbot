#include "supervisor.h"

/**
 * This file contains the code for the Supervisor. All the functions
 * that are needed for processing raw sensor data are contained in 
 * this file as well as those for determining new commands
 *
 * Author: Dr. Andrew Nuxoll and Zachary Paul Faltersack
 * Last Edit: September 13, 2010
 *
 */

#define DEBUGGING 0


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
            g_route->needsRecalc = TRUE;
        }
        else
        {
            // Use shortcircuiting to only call takeNextStep if a goal has been
            // found. This is because there is no route to follow until after
            // the first goal.
            //if(g_goalCount <= 0 || setCommand2(ep))
            //{
            ep->cmd = cmds[i];
            //}
        }

        //Debugging
        printf("Level 0 Action Rules>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        displayRules(g_actionRules->array[0], g_epMem->array[0]);
        /*    printf("Level 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
              displayRules(g_semMem->array[1], g_epMem->array[1]);
              printf("Level 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
              displayRules(g_semMem->array[2], g_epMem->array[2]);
              printf("Level 3 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
              displayRules(g_semMem->array[3], g_epMem->array[3]);
        */

        printf("Level 0 Sequences\n");
        displaySequences(g_sequenceRules->array[0]);

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
        g_route->needsRecalc = TRUE;
    }
    else
    {
        // Use shortcircuiting to only call takeNextStep if a goal has been
        // found. This is because there is no route to follow until after
        // the first goal.
        //if(g_goalCount <= 0 || setCommand2(ep))
        //{
        chooseCommand(ep);
        //}
    }
    
    //Debugging
    printf("Level 0 Action Rules>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displayRules(g_actionRules->array[0], g_epMem->array[0]);
/*    printf("Level 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displayRules(g_semMem->array[1], g_epMem->array[1]);
    printf("Level 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displayRules(g_semMem->array[2], g_epMem->array[2]);
    printf("Level 3 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    displayRules(g_semMem->array[3], g_epMem->array[3]);
*/

    printf("Level 0 Sequences\n");
    displaySequences(g_sequenceRules->array[0]);

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
 * the next episode which is it's RHS.  A semantic rule represents a hypothesis
 * that "that sequence of events leads to this event."
 *
 * CAVEAT: This is a rather complex method.  Breaking it up into smaller methods
 * would make it more complex, in our opinion.  Please rely upon our liberal
 * comments to guide you.  Also consult research notes.
 *
 * @arg level index into g_epMem and g_actionRules where the memory needs to updated    
 * @return int error code
 *
 */
int updateRules(int level)
{
    printf("Entering level %i\n", level);
    // Ensure that the level is within the accepted range for the vectors
    if(level < 0 || level >= MAX_META_DEPTH)
    {
        return -3;
    }

    // Create pointers to the two associated vectors we'll be working with
    Vector* ruleList = g_actionRules->array[level];
    Vector* episodeList = g_epMem->array[level];
    Vector* sequenceList = g_sequenceRules->array[level];
    Vector* currSequence;

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

/*    // If this is the first new action rule after finding a goal,
    // then we want to create a new sequence.
    if (episodeList->size > 2 &&
         containsGoal(episodeList->array[episodeList->size - 3], level))
    {
        // create the empty vector to hold the sequence
        addEntry(sequenceList, newVector());
    }
*/
    // we want to have a pointer to the most recent sequence in the list
    // so we can add new action rules to it as they're encountered
    currSequence = sequenceList->array[sequenceList->size - 1];

    //Create a candidate rule that we would create from this current
    //episode.  We won't add it to the rule list if an identical rule
    //already exists.
    Rule* newRule                   = (Rule*) malloc(sizeof(Rule));
    newRule->level                  = level;
    newRule->epmem                  = episodeList;
    newRule->outcome                = episodeList->size - 1;
    newRule->index                  = episodeList->size - 2;
    newRule->length                 = 1;
    newRule->freq                   = 1;
    newRule->overallFreq            = NULL;
    newRule->cousins                = NULL;
    newRule->isPercentageRule       = FALSE;
    printf("contains goal...!\n");
    fflush(stdout);

    printf("this line is crashing it\n");
    newRule->containsGoal           = episodeContainsGoal(episodeList->array[episodeList->size - 1], level);
    
    printf("candidate rule: ");
    fflush(stdout);
    displayRule(newRule);
    printf("wait no it's not\n");
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

        //         //%%%For Debugging:
        //         printf("begin comparison to rule #%i: ", i);
        //         displayRule(curr);
        //         printf("\n");

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
                    if(curr->isPercentageRule)
                    {
                        //%%%Debugging
#if DEBUGGING
                        printf("comparing cousins: \n");
#endif
                        int k;
                        //Iterate over cousins and find one with same outcome as candidate rule
                        for(k = 0; k < curr->cousins->size; k++)
                        {
                            Rule* cousin = curr->cousins->array[k];

                            //%%%Debugging
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
                            newRule->isPercentageRule = TRUE;
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
                            else if(curr->index - curr->length <= 0)
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
                                printf("new %i:   ");
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
                                curr->isPercentageRule = TRUE;
                                newRule->isPercentageRule = TRUE;
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

    // add most recently seen action rule to current sequence
    if(updateExistingRule != NULL)
    {
printf("Adding Rule: ");
displayRule(updateExistingRule);
printf(" to current sequence\n");
        addActionToSequence(currSequence, updateExistingRule);

        //

        
        // if the rule we just added is percentage rule or contains a
        //goal then end the current sequence and start a new one
        if (updateExistingRule->isPercentageRule
            || updateExistingRule->containsGoal)
        {
            assert(currSequence->size > 1); 
            
            // this newly completed sequence becomes the next episode in the
            // next level's episodic memory
            if (level + 1 < MAX_META_DEPTH)
            {
                episodeList = g_epMem->array[level + 1];
                addEntry(episodeList, currSequence);
            }
        
            // create an vector to hold the next sequence 
            currSequence = newVector();
            addEntry(sequenceList, currSequence);
            //typically the next sequence starts with the action that
            //ended the last sequence.  (Exception:  last action
            //contains a goal)
            if (!updateExistingRule->containsGoal)
            {
                addActionToSequence(currSequence, updateExistingRule);
            }

            // this sequence then becomes an episode in our next level
            // so we need recursive call to update.
            if(level + 1 < MAX_META_DEPTH)
            {
                updateRules(level + 1);
            }
        }
        
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


    
}

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
        displaySequence(sequences->array[i]);
        printf("\n");
    }
}

/**
 * displayRule                        *RECURSIVE*
 *
 * prints a human-readable version of a rule.  If the rule is a
 * meta-rule it makes recursive calls until it reaches the base rule.
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
    if(rule->isPercentageRule)
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
        else //meta-rule
        {
            printf("{ ");
            displaySequence((Vector*)rule->epmem->array[rule->index - i]);
            printf(" }");
        }

    }//for

}// displayRule

/**
 * chooseCommand
 *
 * This function takes a pointer to a new episode and chooses a command
 * that should accompany this episode
 *
 * @arg ep a pointer to the most recent episode
 * @return int the command that was chosen
 */
int chooseCommand(Episode* ep)
{
    int i, j;       // indices for loops

    // seed rand if first time called
    static int needSeed = TRUE;
    if(needSeed == TRUE)
    {
        needSeed = FALSE;
        srand(time(NULL));
    }

    // Determine the next command, possibility of random command
    // If a random command is chosen, then a command that would 
    // create a new rule is preferred. If no new rule can be made,
    // then just choose complete random.
    Vector* episodeList = g_epMem->array[0];
    int foundNewRule = 0;

    //%%%DEBUGGING: take random actions for now
    if((rand() % 100) < g_randChance || episodeList->size < NUM_TO_MATCH || g_goalCount == 0)
    {
/*     if(g_goalCount == 0) */
/*     { */
/*         // Use flag to jump out of loop if a command is found that will */
/*         // create a new rule. Due to order of processing loop statements, */
/*         // we must subtract 1 from i to ensure it stays on the correct */
/*         // command once out of the loop. */
/*         for(i = CMD_NO_OP; i < LAST_MOBILE_CMD && !foundNewRule; i++) */
/*         { */
/*             if(ruleMatch(i) == NULL) */
/*             { */
/*                 foundNewRule = TRUE; */
/*                 i -= 1; */
/*             } */
/*         } */

/*         // If we can make a new rule then do it, otherwise choose a random command */
/*         if(foundNewRule) */
/*         { */
/*             ep->cmd = i; */
/*         } */
/*         else */
/*         { */
            ep->cmd = (rand() % (LAST_MOBILE_CMD)) + CMD_NO_OP;
/*         } */
    }else
    {
        // loop on setCommand until a route is chosen that will lead to a successful cmd
        while(setCommand2(ep))
        {
            if(g_statsMode == 0)
            {
                printf("Failed to set a command\n");
            }
        }
    }
    //      printf("COMMAND TO BE SENT %s (%i)\n", interpretCommand(ep->cmd), ep->cmd);


    return ep->cmd;
}// chooseCommand

/**
 * displayRoute
 *
 * Display the current route. Prints the rules that make up
 * the route in reverse order to make it easier visually to
 * read the steps.
 */
void displayRoute()
{
    Vector* semMem = g_actionRules->array[0];
    Vector* route = g_route->route;

    printf("Current Route: \n");

    int i;
    for(i = g_route->numRules - 1; i >= 0; i--)
    {
        if(i == g_route->currRule)
        {
            printf("-->");
        }
        printf("\t{ ");
        displayRule(semMem->array[*((int*)route->array[i])]);
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
    if(g_route->needsRecalc || !nextStepIsValid())
    {
        printf("Route needs to be recalculated\n");
        fflush(stdout);
        planRoute(ep);
        printf("Route is recalculated\n", stderr);
        fflush(stdout);
    }
    else
    {
        printf("Route is good\n");
    }

    displayRoute();

    // attempt to follow the route and filter back success/error code
    return takeNextStep(ep);
}// setCommand2

/**
 * takeNextStep
 *
 * This function attempts to complete a rule. If any discrepencies
 * show up between the rule and actual sensor data, then the rule 
 * is abandoned for completion and an error code is returned
 *
 * @arg currEp a pointer to the current episode to be checked
 *
 * @return int An error code
 */
int takeNextStep(Episode* currEp)
{
    // set up useful variables

    Vector* episodeList = g_epMem->array[0];
    Vector* ruleList        = g_actionRules->array[0];
    Vector* route           = g_route->route;
    int     currRule        = g_route->currRule;
    int             currEpIR        = g_route->currEpInRule;
    Rule*   rule            = ruleList->array[*((int*)route->array[currRule])];
    Episode* nextStep       = episodeList->array[rule->index - (rule->length - 1 - currEpIR)];

    // I'm pretty sure this check is no longer needed because of how I changed
    // the structure of setCommand2, but just in case I'm leaving this here.
    // Once we know it's unnecessary we can remove it
/*
// We want to make sure that after completing the most recent command
// the expected state is equal to current state, otherwise we need to
// replan our route.
if(!nextStepIsValid())
{
printf("Next step is no longer valid, returning\n");
return 1;
}
*/

     // We had a good prediction so continue to follow Rule
     currEp->cmd = nextStep->cmd;

     // if next step is a goal episode, then set route needs recalc
     if(rule->containsGoal && currEpIR == rule->length-1) {
         g_route->needsRecalc = TRUE;
     }

     // Check if we've completed the Rule and update Route data accordingly
     if(currEpIR == rule->length - 1)
     {
         printf("This command finishes current rule, setting up for next rule in Route\n");
         g_route->currRule++;
         g_route->currEpInRule = 0;
     }
     else
     {
         g_route->currEpInRule++;
     }

     return 0;
}// takeNextStep

/**
 * nextStepIsValid
 *
 * Checks the current state versus the Route and determines
 * if the previous command's outcome matches with the expected
 * outcome of the Route.
 */
int nextStepIsValid()
{
    if(g_route->needsRecalc)
    {
        return FALSE;
    }

    Vector* episodeList = g_epMem->array[0];
    Vector* ruleList        = g_actionRules->array[0];
    Vector* route           = g_route->route;
    int     currRule        = g_route->currRule;
    int             currEpIR        = g_route->currEpInRule;
    Rule*   rule            = ruleList->array[*((int*)route->array[currRule])];
    Episode* nextStep       = episodeList->array[rule->index - (rule->length - 1 - currEpIR)];
    Episode* currEp         = episodeList->array[episodeList->size - 1];

    if(!equalEpisodes(currEp, nextStep, TRUE))
    {
        g_route->needsRecalc = TRUE;
        return FALSE;
    }

    return TRUE;
}// isNextStepValid

/**
 * setCommand
 *
 * Find the match scores for each of the available commands (currently condensed list)
 * If a goal has been found, then find the index of the best match (closest to subsequent goal)
 *
 * @arg ep pointer to new episode
 * @return int status code
 */
int setCommand(Episode* ep)
{       
    int tempIdx, tempDist;                                                  // temp vars
    int i,j,k;                                                                              // looping indices
    int bestMatch = CMD_NO_OP;
    double commandScores[NUM_COMMANDS];                             // Array to store scores for commands
    int commandIdxs[NUM_COMMANDS];
    int toggle = 0;
    Vector* episodeList = g_epMem->array[0];
    double partialScoreTable[episodeList->size];

    // initialize scores to 0
    for(i = 0; i < NUM_COMMANDS; i++)
    {
        commandScores[i] = 0;
        commandIdxs[i] = 0;
    }

    // generate partial score table
    generateScoreTable(episodeList, partialScoreTable);

    // determine index with top match per command
    for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
    {
        commandIdxs[i] = findTopMatch(partialScoreTable, commandScores, i);
    }

    // can only successfully search if at minimum history contains
    // NUM_TO_MATCH episodes
    if(g_goalCount > 0)
    {
        // Set distance to goal equal to largest possible distance
        tempDist = episodeList->size;
        // Test out the available commands
        for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
        {
            // find index closest to a subsequent goal
            tempIdx = commandIdxs[i];

            // If the goal has been found then determine which of the three episodes
            // with the greatest scores is closest to the goal

            for(j = 0; j < g_goalCount; j++)
            {
                /*                                              printf("dist: %i\n", g_goalIdx[j] - tempIdx);
                                                                printf("idx1: %i idx2: %i\n", g_goalIdx[j], tempIdx);
                */

                // Make sure distance is greater than 0 and 
                // If the distance between the episode and goal is less than previous
                // then save it
                if(g_goalIdx[i] - tempIdx > 0 && g_goalIdx[j] - tempIdx < tempDist)
                {
                    // keep track of the current best distance
                    tempDist = g_goalIdx[j] - tempIdx;
                    // keep track of which command gave the best distance so far
                    bestMatch = i;
                    toggle = 1;
                    //                                                      printf("Setting toggle, cmd: %s, tempDist: %i\n", interpretCommand(i), tempDist);
                }// if
            }// for
        }// for 
    }// if


    // If a goal has been found then we have an index of closest goal match
    if(g_goalCount > 0 && toggle == 1)
    {
        ep->cmd = bestMatch;
    }
    else
    { 
        // else we find cmd with greatest score
        int max = CMD_NO_OP;
        for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
        {
            //Debug lines
            /*                      double maxScore = NUM_TO_MATCH * NUM_SENSORS * 2;
                                    printf("Max score: %g out of: %g for command: %s\n", commandScores[i], maxScore, interpretCommand(i)); */
            if(commandScores[max] < commandScores[i])
            {
                max = i;
            }
        }
        ep->cmd = max;
    }

    // If not stats mode print scores for cmds
    if(g_statsMode == 0)
    {
        for(i = CMD_NO_OP; i < NUM_COMMANDS; i++)
        {
            printf("%s score: %f\n", interpretCommand(i), commandScores[i]);
        }
    }
    /*
      if(toggle != 1)
      {
      printf("Still have not found valid bestMatch, cmd: %s\n", interpretCommand(ep->cmd));
      }
      else
      {
      printf("Found valid bestMatch, cmd: %s\n", interpretCommand(ep->cmd));
      }
    */


    // Report malformed episode and location of error report
    if(ep->cmd <= CMD_ILLEGAL || ep->cmd >= NUM_COMMANDS)
    {
        printf("Episode is bad: setCommand %s (%i)\n", interpretCommand(ep->cmd), ep->cmd);
    }

    // return success
    return 0;
}// setCommand

/**
 * ruleMatch
 *
 * Inspect the LHS of the rules we've written and determine
 * if the current state is a match to a rule given a potential
 * action.  
 *
 * @arg action An integer that represents a command or action
 *
 * @return Rule* A pointer to the rule that matches, NULL if no
 *                              match is found
 */
Rule* ruleMatch(int action)
{
    // Create temporary pointers to working memory
    Vector* episodeList = g_epMem->array[0];
    Vector* ruleList        = g_actionRules->array[0];
    int i,j, isMatch;

    // Assign the action to our most recent episode
    ((Episode*)episodeList->array[episodeList->size - 1])->cmd = action;

    // Iterate through list of rules and see if any rule matches
    // current state
    for(i = 0; i < ruleList->size; i++)
    {
        Rule* rule = ruleList->array[i];
        // Assume rule is a match to current state, if not, search will continue. 
        // The first rule that matches to its full length will be returned
        // We know this is the right match because any other rule that has the
        //              same LHS to that point would have caused another episode to be added
        //              and increased the length. So matching to full rule length ensures
        //              a unique match to current state
        isMatch = 1;
        for(j = 0; j < rule->length && isMatch; j++)
        {
            if(!equalEpisodes(episodeList->array[rule->index - j],
                              episodeList->array[episodeList->size - 1 - j],
                              FALSE))
            {
                isMatch = 0;
            }
        }// for

        // Return the current episode's command to its original default value
        // and return the rule that matched
        if(isMatch)
        {
            ((Episode*)episodeList->array[episodeList->size - 1])->cmd = action;
            return rule;
        }
    }// for 

    return NULL;
}// ruleMatch

/*
 * equalEpisodes
 *
 * This method takes two episodes and returns a boolean indicating
 * if they are a match. A match is when both the sensor data and 
 * the action are the same in each episode.
 *
 * @arg ep1 A pointer to the first episode
 * @arg ep2 A pointer to the second episode
 *
 * @return int A boolean indicating if they are a full match
 */
int equalEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch)
{
    int i;
    // Ensure the sensor data match between episodes and return
    // false if not
    for(i = 0; i < NUM_SENSORS; i++)
    {
        if(ep1->sensors[i] != ep2->sensors[i]) return FALSE;
    }

    // Ensure episodes have same command, return false if not
    if(isCurrMatch == FALSE && ep1->cmd != ep2->cmd) return FALSE;

    return TRUE;
}// equalEpisodes


/**
 * planRoute
 *
 * Take a state and determine a route to a desired state by matching
 * the outcome of a rule to the sensors for the first episode on
 * the LHS of the follwing rule. It then returns this list of rules
 * to follow.
 *
 * @arg currEp The current state to map to goal
 *
 * @return int A error code
 */
int planRoute(Episode* currEp)
{
    // Set up pointers to episodic memory
    Vector* ruleList        = g_actionRules->array[0];
    Vector* epMem           = g_epMem->array[0];

    // Free previous route and initialize new vector for new route
    freeVector(g_route->route);
    g_route->route = newVector();

    // Reset rest of Route data
    g_route->currRule               = 0;
    g_route->currEpInRule   = 0;
    g_route->needsRecalc    = FALSE;
    g_route->numRules               = 0;


    // Find a rule that contains a goal in its outcome
    int goalRule = -1;
    int i;
    for(i = 0; i < ruleList->size; i++)
    {
        if((((Rule*)ruleList->array[i])->containsGoal, TRUE))
        {
            goalRule = i;
            break;
        }
    }

    Rule* goal = (Rule*)ruleList->array[goalRule];

    // If no rule with goal was found, then return failure
    if(goalRule == -1)
    {
        return 1;
    }

    // Check if goalRule happens to begin with currEp and if so, return it
    if(equalEpisodes(epMem->array[goal->index - (goal->length - 1)], currEp, TRUE))
    {
        addRuleToRoute(goalRule);
        return 0;
    }

    // Try to piece together rules that lead to each other and create the 
    // shortest route possible. Need to make sure the outcome of the first
    // rule matches the sensor data of the first episode in the following 
    // rule.
    for(i = 0; i < ruleList->size; i++)
    {
        // Get pointer to current rule and compare to currState
        // If matched, continue to see if it completes the route
        // by having the outcome match the goal Rule
        Rule* currRule = (Rule*)ruleList->array[i];
        if(equalEpisodes(epMem->array[currRule->index - (currRule->length - 1)], currEp, TRUE))
        {
            // If completes the route, save last rule, save the rule length and break
            if(equalEpisodes(epMem->array[goal->index - (goal->length - 1)], epMem->array[currRule->outcome], TRUE))
            {
                addRuleToRoute(i);
                break;
            }
            // Otherwise add the next rule, reset the rule index and continue
            else
            {
                addRuleToRoute(i);
                currEp = (Episode*)epMem->array[currRule->outcome];
                //                              i = 0;
            }//else

            if(g_route->numRules >= MAX_ROUTE_LEN)
            {
                break;
            }
        }// if
    }// for

    // Put the goal rule at the end of the route
    addRuleToRoute(goalRule);

    return 0;
}// planRoute

/**
 * addRuleToRoute
 *
 * This method takes an index to a rule to add to the route
 * and stores it in the vector in the Route
 *
 * @arg ruleIdx An int indexing into the Rule vector
 */
void addRuleToRoute(int ruleIdx)
{
    int* temp = (int*)malloc(sizeof(int));
    *temp = ruleIdx;
    addEntry(g_route->route, temp);
    g_route->numRules += 1;
}// addRuleToRoute

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
 * generateScoreTable
 *
 * search for a series of episodes that best matches the last NUM_TO_MATCH
 * episodes
 *
 * @arg vector the vector containing full history of episodes
 * @arg score a pointer to double we can use to store/return score
 * @return int Error code
 */
int generateScoreTable(Vector* vector, double* score)
{
    int i,j, returnIdx = 0;
    double tempScore = 0.0, discount = 1.0;
    Vector* episodeList = g_epMem->array[0];
    int start = episodeList->size - NUM_TO_MATCH - 1; // subtract 1 to begin 1 episode before curr state

    if(g_goalCount > 0)
    {
        start = g_goalIdx[g_goalCount - 1] - NUM_TO_MATCH;
    }

    // Iterate through vector and search from each index
    for(i = start; i >= 0; i--)
    {
        // reset tempscore
        tempScore = 0;

        for(j = NUM_TO_MATCH - 1; j >= 0; j--)
        {
            // If any state that occurs before the final episode in the match is a goal
            // state then break because that means our current final episode is being 
            // matched to one after a goal, which doesn't help to find the goal again
            if(((Episode*)(vector->array[i + j]))->sensors[SNSR_IR] == 1)
            {
                break;
            }

            // compare two episodes and add result to score with appropriate discount
            //                                      V-index iterate to beginning            V-index to current episode frame
            tempScore += (discount * compareEpisodes(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j], (j == NUM_TO_MATCH - 1 ? TRUE : FALSE)));
            discount *= DISCOUNT;
        }//for

        score[i + NUM_TO_MATCH - 1] = tempScore;
    }// for
    // return success
    return 0;
}// generateScoreTable

/**
 * compare
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an episode
 * @arg ep2 a pointer to another episode
 * @return double The score telling us how close these episodes match
 */
double compareEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch)
{
    int i;
    double match = 0;

    // Iterate through the episodes' sensor data and determine if they are matching episodes
    for(i = 0; i < NUM_SENSORS; i++)
    {
        if(ep1->sensors[i] == ep2->sensors[i])
        {
            match++;
        }
    }

    // Only add the value for command match if not the last episode in state
    if(isCurrMatch == FALSE)
    {
        // add num_sensors to give cmd 1/2 value
        if(ep1->cmd == ep2->cmd)
        {
            match += NUM_SENSORS;
        }
    }

    // return the total value of the match between episodes
    //      printf("Match score %g\n", match);
    return match;
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
        double matchScore = compareEpisodes(list->array[i1],
                                            list->array[i2], noRHS);

        //See if that match score is a perfect match
        if (noRHS)
        {
            return (matchScore == NUM_SENSORS);
        }
        else
        {
            return (matchScore == 2 * NUM_SENSORS);
        }   
    }
    else //meta-rule
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
        printf("sequence->size=%d\n", sequence->size);
        displaySequence(sequence);
        Rule *rule = (Rule *)sequence->array[sequence->size - 1];
        
        //For a meta-rule, a goal is indicated by "containsGoal"
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
    g_epMem                 = newVector();
    g_actionRules   = newVector();
    g_sequenceRules = newVector();

    int i;
    for(i = 0; i < MAX_META_DEPTH; i++)
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

    g_route                 = (Route*)malloc(sizeof(Route));
    g_route->route          = newVector();
    g_route->currRule       = 0;
    g_route->currEpInRule   = 0;
    g_route->needsRecalc    = TRUE;
    g_route->numRules       = 0;



    
    g_connectToRoomba       = 0;
    g_statsMode             = 0;
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
    for(i = MAX_META_DEPTH - 1; i >= 0; i--)
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

        //cleanup the episodes at this level
        for(j = 0; j < episodeList->size; j++)
        {
            //at level 0, this is an array of pointers to Episode structs
            //which need to be cleaned up.
            if (i == 0)
            {
                for(int k = 0; k < episodeList->size; k++)
                {
                    Episode *ep = (Episode *)episodeList->array[j];
                free(ep);
            }

            //now safe to free the list
            freeVector((Vector*)episodeList->array[j]);
        }//for
        
        
    }//for

    //free the global list
    freeVector(g_epMem);
    freeVector(g_actionRules);
    freeVector(g_route->route);

    free(g_route);
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

