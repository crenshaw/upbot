#include "supervisor.h"

/**
* This file contains the code for the Supervisor. All the functions
* that are needed for processing raw sensor data are contained in 
* this file as well as those for determining new commands
*
* Author: Dr. Andrew Nuxoll and Zachary Paul Faltersack
* Last Edit: July 5, 2010
*
*/

// The chance of choosing a random move
int g_randChance = 80;
// global strings for printing to console
char* g_forward = "forward";
char* g_right	= "right";
char* g_left	= "left";
char* g_adjustR	= "adjust right";
char* g_adjustL	= "adjust left";
char* g_blink	= "blink";
char* g_no_op	= "no operation";
char* g_song	= "song";
char* g_unknown	= "unknown";

char* g_forwardS = "FW";
char* g_rightS	 = "RT";
char* g_leftS	 = "LT";
char* g_adjustRS = "AR";
char* g_adjustLS = "AL";
char* g_blinkS	 = "BL";
char* g_no_opS	 = "NO";
char* g_songS	 = "SO";
char* g_unknownS = "$$";

// Keep track of goals
int g_goalCount = 0;						// Number of goals found so far
int g_goalIdx[NUM_GOALS_TO_FIND];




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
	Episode* ep = createEpisode(sensorInput);
	// Add new episode to the history
	addEpisode(g_epMem->array[0], ep);
	updateAllRules(0);

	// Send ep to receive a command
	// Will return -1 if no command could be set
	chooseCommand(ep);

    //Debugging
	printf("Level 0 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	displayRules(g_semMem->array[0], g_epMem->array[0]);
	printf("Level 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	displayRules(g_semMem->array[1], g_epMem->array[1]);
	printf("Level 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	displayRules(g_semMem->array[2], g_epMem->array[2]);
	printf("Level 3 >>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	displayRules(g_semMem->array[3], g_epMem->array[3]);

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
	}else
	{
		if(g_statsMode == 0)
		{
			printf("Sensor data successfully parsed into new episode\n");
		}
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
 * updateAllRules                    *RECURSIVE*
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
 * @arg level index into g_epMem and g_semMem where the memory needs to updated	
 * @return int error code
 *
 */
int updateAllRules(int level)
{
	printf("Entering level %i\n", level);
	// Ensure that the level is within the accepted range for the vectors
	if(level < 0 || level >= MAX_META_DEPTH)
	{
		return -3;
	}

	// Create pointers to the two associated vectors we'll be working with
	Vector* ruleList = g_semMem->array[level];
	Vector *episodeList = g_epMem->array[level];
	//You need a minimum of two episodes to make a rule
	if(episodeList->size <= 1)
	{
		return -1;
	}

    //Determine if this is a base rule or meta-rule
    int isBaseRule = (episodeList == g_epMem->array[0]);

    //If the most recent complete episode was a goal then return.  We
    //don't want any goals on the LHS of any rule.
    if (containsGoal(episodeList->array[episodeList->size - 2], isBaseRule))
    {
            return -2;
    }

	//Create a candidate rule that we would create from this current
	//episode.  We won't add it to the rule list if an identical rule
	//already exists.
	Rule* newRule 				= (Rule*) malloc(sizeof(Rule));
    newRule->isBaseRule         = isBaseRule;
    newRule->epmem              = episodeList;
	newRule->outcome			= episodeList->size - 1;
	newRule->index 				= episodeList->size - 2;
	newRule->length 			= 1;
	newRule->freq				= 1;
	newRule->overallFreq 		= NULL;
	newRule->cousins			= NULL;
	newRule->isPercentageRule 	= FALSE;
    newRule->containsGoal       = containsGoal(episodeList->array[episodeList->size - 1], isBaseRule);
    
    printf("candidate rule: ");
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

//         //%%%For Debugging:
//         printf("begin comparison to rule #%i: ", i);
//         displayRule(curr);
//         printf("\n");
        
		for(j = 0; j < newRule->length; j++)
		{
            //Find out if the j-th part of the LHS matches
			if (compare(episodeList, newRule->index - j, curr->index - j, isBaseRule))
			{
                printf("found match between %i-th entries of: ", j);
                displayRule(curr);
                printf(" and ");
                displayRule(newRule);
                printf("\n");
                fflush(stdout);

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
                printf("comparing cousins: \n");
						int k;
						//Iterate over cousins and find one with same outcome as candidate rule
						for(k = 0; k < curr->cousins->size; k++)
						{
							Rule* cousin = curr->cousins->array[k];

                //%%%Debugging
                printf("\t");
                displayRule(cousin);
                printf(" AND ");
                displayRule(newRule);
                printf("\n");
                fflush(stdout);

                            //If we find one with same outcome, increase
							//frequency and inform not to add rule
							if (compare(episodeList, newRule->outcome,
                                        cousin->outcome, isBaseRule))
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
                            printf("new cousin is unique.  Adding...\n");
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
					else	//Found a LHS match to a non-percentage rule
					{
						//Now see if the RHS of both rules match
                        if (compare(episodeList, newRule->outcome,
                                    curr->outcome, isBaseRule))
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
						else	//RHS does not match
						{
                            printf("LHS match but RHS doesn't while comparing to %i...\n", i);
                            fflush(stdout);
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

                            if (containsGoal(episodeList->array[newLHSEntryIndex],
                                             isBaseRule))
							{
                                printf("NewRule expands into goal at index: %i\n",
                                       newLHSEntryIndex);
                                fflush(stdout);
                                
                                //the new rule can't be expanded so we
                                //consider it degenerate so just abort
                                //and create no new rules or updates
                                matchComplete = TRUE;
                                addNewRule = FALSE;
                            }

                            //Check for reason #2: no room to expand
							else if(curr->index - curr->length <= 0)
							{
                                printf("avail space: %i,  curr expands outside goal\n",
                                       curr->index - curr->length);
                                fflush(stdout);
                                
                                //The current rule can't be expanded
                                //so we consider it degenerate and
                                //replace it with the new rule.
                                curr->index 				= newRule->index;
                                curr->outcome               = newRule->outcome;
                                curr->length 			    = newRule->length;
                                curr->freq				    = 1;

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
                                
                                printf("partial match with curr, extending new rule to %i\n",
                                       newRule->length);
                                fflush(stdout);
                                printf("new candidate: ");
                                displayRule(newRule);
                                printf("\n");
                                fflush(stdout);
                            }

                            //If the current rule can be expanded then
                            //expand both the current and candidate rules
                            else if(curr->length < MAX_LEN_LHS)
							{
                                printf("len of curr rule (%i) = %i < %i so increasing to %i\n",
                                       i, curr->length, MAX_LEN_LHS, curr->length+1);
                                fflush(stdout);
                                
                                //both current rule and new rule can
                                //be expanded so do so in hopes that they will
                                //end up different
								curr->length++;
                                curr->freq = 1;
								newRule->length++;

                                
                                printf("new %i:   ");
                                displayRule(curr);
                                printf("\n");
                                printf("new cand: ");
                                displayRule(newRule);
                                printf("\n");
                                fflush(stdout);

							}
							else  //current rule can't be expanded without
                                  //exceeding max length (reason #3)
							{
                                printf("cousins\n");
                                fflush(stdout);
                                
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
                        printf("newRule matches but is bigger than current rule.  Aborting.\n");
                        fflush(stdout);
                    }

                    //If the new rule can be expanded, try doing so to
					//see if that makes it unique
					else if(newRule->length < MAX_LEN_LHS)
					{
						newRule->length++;
                        
                        printf("expanded new rule to len %i\n",
                               newRule->length);
                        printf("new candidate: ");
                        displayRule(newRule);
                        printf("\n");
                        fflush(stdout);
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
		printf("Adding new rule\n");
		addRule(ruleList, newRule, FALSE);
		updateExistingRule = newRule;
	}
	else
	{
		free(newRule);
	}

	if(updateExistingRule != NULL && level + 1 < MAX_META_DEPTH)
	{
		episodeList = g_epMem->array[level + 1];;
		addEntry(episodeList, updateExistingRule);
		updateAllRules(level + 1);
	}

	return 0;
}// updateAllRules

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
 * addRule
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

    //determine if episodeList contains Rule structs or Episode structs
    int isEpList = ((Rule *)(ruleList->array[0]))->isBaseRule;
    
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
    if (rule->isBaseRule)
    {
		Vector* episodeList = (Vector*)g_epMem->array[0];
        printf("%i", interpretSensorsShort(((Episode*)episodeList->array[rule->outcome])->sensors));
    }
    else //meta-rule
    {
        printf("{ ");
        displayRule((Rule*)rule->epmem->array[rule->outcome]);
        printf(" }");
    }

    //Print the arrow 
	if(rule->isPercentageRule)
	{
		printf(" <--%2i-- ", rule->freq * 100 / *(rule->overallFreq));
	}
	else
	{
		printf(" <------ ");
	}

    //Print the LHS
	for(i = 0; i < rule->length; i++)
	{
        if (rule->isBaseRule)
        {
            printf("%i ", interpretSensorsShort(((Episode*)rule->epmem->array[rule->index - i])->sensors));	
            printf("%s, ", interpretCommandShort(((Episode*)rule->epmem->array[rule->index - i])->cmd));
        }
        else //meta-rule
        {
            printf("{ ");
            displayRule((Rule*)rule->epmem->array[rule->index - i]);
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
	int i, j;	// indices for loops

	// seed rand if first tiem called
	static int needSeed = TRUE;
	if(needSeed == TRUE)
	{
		needSeed = FALSE;
		srand(time(NULL));
	}

	// Determine the next command, possibility of random command
	Vector* episodeList = g_epMem->array[0];
	if((rand() % 100) < g_randChance || episodeList->size < NUM_TO_MATCH)
	{
		// Command 0 is now illegal command so adjust NUM_COMMANDS to account for this
		// Then increment to push back into valid command range
		ep->cmd = (rand() % LAST_MOBILE_CMD) + CMD_NO_OP;
	}else
	{
		// find the best match scores for the three commands
		// if no goal has been found (returns 0) then we take the command with the greatest score
		if(setCommand(ep) != 0)
		{
			if(g_statsMode == 0)
			{
				printf("Failed to set a Command");
			}
			return -1;
		}
	}
	//	printf("COMMAND TO BE SENT %s (%i)\n", interpretCommand(ep->cmd), ep->cmd);


	return ep->cmd;
}// chooseCommand

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
	int tempIdx, tempDist;							// temp vars
	int i,j,k;										// looping indices
	int bestMatch = CMD_NO_OP;
	double commandScores[NUM_COMMANDS];				// Array to store scores for commands
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
				/*						printf("dist: %i\n", g_goalIdx[j] - tempIdx);
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
					//							printf("Setting toggle, cmd: %s, tempDist: %i\n", interpretCommand(i), tempDist);
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
			/*			double maxScore = NUM_TO_MATCH * NUM_SENSORS * 2;
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
			//					V-index iterate to beginning		V-index to current episode frame
			tempScore += (discount * compareEpisodes(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j], (j == NUM_TO_MATCH - 1 ? TRUE : FALSE)));
			discount *= DISCOUNT;
		}//for

		score[i + NUM_TO_MATCH - 1] = tempScore;
	}// for
	// return success
	return 0;
}// match

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
//	printf("Match score %g\n", match);
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
 * @arg isBaseRule is TRUE if vec contains episodes and FALSE if it
 *                 contains rules
 * @return TRUE if the rules match and false otherwise
 */
int compare(Vector *list, int i1, int i2, int isBaseRule)
{
    if (isBaseRule)
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
 * containsGoal
 *
 * This routine determines whether a given Rule or Episode contains a goal.
 *
 * @arg entry      a pointer to an Episode or Rule struct
 * @arg isBaseRule is TRUE if entry is an Episode (false for a rule)
 * @return TRUE if the entry is a goal and FALSE otherwise
 */
int containsGoal(void *entry, int isBaseRule)
{
    if (isBaseRule)
    {
        Episode *ep = (Episode *)entry;
        
        //For base rules, a goal is indicated by the IR sensor on the episode
        return ep->sensors[SNSR_IR];
    }
    else //meta-rule
    {
        Rule *rule = (Rule *)entry;
        //For a meta-rule, a goal is indicated by "containsGoal" 
        return rule->containsGoal;
    }
}//containsGoal


/**
 * initSupervisor
 *
 * Initialize the Supervisor vectors
 * 
 */
void initSupervisor()
{
	g_epMem 		= newVector();
	g_semMem		= newVector();
	int i;
	for(i = 0; i < MAX_META_DEPTH; i++)
	{
		Vector* temp = newVector();
		addEntry(g_epMem, temp);

		temp = newVector();
		addEntry(g_semMem, temp);
	}

	g_connectToRoomba 	= 0;
	g_statsMode 		= 0;
}// initSupervisor

/**
 * endSupervisor
 *
 * Free the memory allocated for the Supervisor
 */
void endSupervisor()
{
	int i;
	for(i = 0; i < g_epMem->size; i++)
	{
		freeVector((Vector*)g_epMem->array[i]);
	}
	for(i = 0; i < g_semMem->size; i++)
	{
		freeVector((Vector*)g_semMem->array[i]);
	}
	freeVector(g_epMem);
	freeVector(g_semMem);
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

