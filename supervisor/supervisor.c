#include "supervisor.h"

/*
 * This file contains the code for the Supervisor. All the functions
 * that are needed for processing raw sensor data are contained in
 * this file as well as those for determining new commands
 *
 * Authors:      Zachary Paul Faltersack, Brian Burns, Andrew Nuxoll
 * Last updated: December 5, 2010
 */

/*
 * Outstanding Bugs and Issues
 *
 * 1.  Agent continually picks the same plan even though a replacement is being
 *     successfully applied and, thus, a slightly shorter plan should be
 *     discoverable.  (Also, there should be a mechanism that makes it explore
 *     other plans...)
 *
 * 2.  displayPlan doesn't recursively print multi-level plans properly
 *
 */

/*
 * Minor Maintenance To-Do List
 *
 * 1.  Review updateAll() and try to simplify it.  Break it up into parts?
 * 2.  Add a method to the vector that allows insert and delete and use it?  I'm
 *     not sure that this will make the code easier to read but it might help.
 * 3.  endSupervisor needs to be fixed.  We're hemmoraging RAM.
 * 4.  freePlan() is not being called when an existing plan is being replaced
 * 5.  It would be less cumbersome and less error prone if we literally did
 *     replace the appropriate sequence in a route when we do a replacement.
 *     The replSeq pointer would only be used for memory maintenance.
 * 6.  Remove code supporting the concept of a start state.  (Or leave it in to
 *     aid debugging?)
 * 7.  Add check for determinate rule in convertEpMatchToSequence() (see NOTE in
 *     that method's comment header)
 * 8.  Break this file up into multiple files.  One for unittests, one for
 *     debugging methods and keep the rest in supervisor.c.
 */

/*
 * Wish List
 *
 * 1.  profile the code for places where are spending the most time and then try
 *     to improve the algorithm there.
 * 2.  Consider removing the "now" field from the Episode struct so we can take
 *     advantage of the fact that many episodes are identical and simply reuse
 *     pointers like we do with the level 1+ episodic memory lists.
 * 3.  findRoute() is SLOW.  Make it faster?  Impose a max plan length?
 *     - one way to speed it up is to make sequences a struct so that each
 *       sequence can have a boolean associated that indicates whether it is a
 *       goal sequence.  You can also have another variable in place used for
 *       transitive closure when trying to avoid adding the same sequence to a
 *       route twice.
 * 4.  findRoute() should be using the cousins list!
 * 5.  Replacements can't be applied over sequence boundaries.  For example you
 *     can't apply this replacement:
 *           {s0RT ----> 0, 0LT ----> 0}==> 0NO ----> 0
 *     to this pair of sequences:
 *           26:0FW, 0RT-->0; 27:0LT, 0AL, 0FW-->512;
 *
 */

//Setting this turns on verbose output to aid debugging
#define DEBUGGING 1


//Particularly verbose debugging for specific methods
#ifdef DEBUGGING
// #define DEBUGGING_UPDATEALL 1
// #define DEBUGGING_UPDATEPLAN 1
// #define DEBUGGING_CHOOSECMD 1
// #define DEBUGGING_INITROUTE 1    //Expensive. Avoid activating this.
// #define DEBUGGING_INITPLAN 1
// #define DEBUGGING_FINDINTERIMSTART 1
// #define DEBUGGING_NSIV 1        // nextStepIsValid()
// #define DEBUGGING_FIND_REPL 1
// #define DEBUGGING_CONVERTEPMATCH 1  //convertEpMatchToSequence()
// #define DEBUGGING_KNN 1
#endif

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
int g_goalCount = 0;              // Number of goals found so far
int g_goalIdx[NUM_GOALS_TO_FIND];


/**
 * memTest
 *
 * This function tests whether the supervisor can build a simple episodic memory
 *
 * @param sensorInput a char string wth sensor data
 * @return int a command for the Roomba (negative is error)
 */
void memTest()
{
    char* noHit     = "0000000000                 ";  // no hit is zero
    char* rightHit  = "0000000010                 ";  // right hit is 2
    char* leftHit   = "0000000001                 ";  // left hit is 1
    char* bothHit   = "0000000011                 ";  // both hit is 3
    char* goal      = "1000000000                 ";  // goal is 512

    char* sensors[] = {noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, leftHit, noHit, leftHit, noHit, leftHit, noHit,
              noHit, noHit, leftHit, leftHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              bothHit, bothHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, bothHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, rightHit, noHit, rightHit, noHit, noHit, bothHit, bothHit,
              noHit, noHit, bothHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, rightHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, bothHit, noHit, bothHit, bothHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, rightHit,
              noHit, rightHit, rightHit, rightHit, noHit, noHit, noHit,
              rightHit, noHit, noHit, noHit, noHit, noHit, noHit, goal};
   
    int cmds[] = {CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_NO_OP,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_FORWARD,
              CMD_RIGHT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_FORWARD, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_NO_OP,
              CMD_NO_OP, CMD_NO_OP, CMD_RIGHT, CMD_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_FORWARD,
              CMD_RIGHT, CMD_NO_OP, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_RIGHT,
              CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_RIGHT, CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_NO_OP,
              CMD_RIGHT, CMD_FORWARD, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_FORWARD,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_FORWARD,
              CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD};

    int   i;

    printf("\t\t\t\t\t\t\tBEGIN MEMTEST\n");
   
    for (i = 0; i < 214; i++)
    {
        // Create new Episode
        printf("Creating and adding episode...\n");
        Episode* ep = createEpisode(sensors[i]);
        // Add new episode to the history
        addEpisode(ep);
        printf("Episode created\n");

        updateAll(0);   //*** magic number whats up?

        // If we found a goal, send a song to inform the world of success
        // and if not then send ep to determine a valid command
        if(episodeContainsGoal(ep, FALSE))
        {
            ep->cmd = CMD_SONG;
            if (g_plan != NULL)
            {
                Route *r = g_plan->array[0];
                r->needsRecalc = TRUE;
            }
        }
        else
        {
            //issue the next command from the cmds array
            ep->cmd = cmds[i];
        }

        // Print out the parsed episode if not in statsMode
        if(g_statsMode == 0)
        {
            displayEpisode(ep);
        }
    }

    printf("\t\t\t\t\t\t\tEND MEMTEST: SUCCESS!\n");
   
   
}//memTest

/**
 * planTest
 *
 * This function tests whether the supervisor constructs a correct plan
 *
 * @param sensorInput a char string wth sensor data
 * @return int a command for the Roomba (negative is error)
 */
void planTest()
{
    char* noHit     = "0000000000                 ";  // no hit is zero
    char* rightHit  = "0000000010                 ";  // right hit is 2
    char* leftHit   = "0000000001                 ";  // left hit is 1
    char* bothHit   = "0000000011                 ";  // both hit is 3
    char* goal      = "1000000000                 ";  // goal is 512

    char* sensors[] = {noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, leftHit, noHit, leftHit, noHit, leftHit, noHit,
              noHit, noHit, leftHit, leftHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              bothHit, bothHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, bothHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, rightHit, noHit, rightHit, noHit, noHit, bothHit, bothHit,
              noHit, noHit, bothHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, rightHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, bothHit, noHit, bothHit, bothHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, rightHit,
              noHit, rightHit, rightHit, rightHit, noHit, noHit, noHit,
              rightHit, noHit, noHit, noHit, noHit, noHit, noHit, goal};
   
    int cmds[] = {CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_NO_OP,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_FORWARD,
              CMD_RIGHT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_FORWARD, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_NO_OP,
              CMD_NO_OP, CMD_NO_OP, CMD_RIGHT, CMD_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_FORWARD,
              CMD_RIGHT, CMD_NO_OP, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_RIGHT,
              CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_RIGHT, CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_NO_OP,
              CMD_RIGHT, CMD_FORWARD, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_FORWARD,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_FORWARD,
              CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD};

    int i;

    //First call memTest twice to give it enough memory to make a plan
    memTest();
    memTest();

    printf("\t\t\t\t\t\t\tBEGIN PLANTEST\n");

    for (i = 0; i < 214; i++)
    {
        // Create new Episode
        printf("Creating and adding episode...\n");
        Episode* ep = createEpisode(sensors[i]);
        // Add new episode to the history
        addEpisode(ep);
        printf("Episode created\n");

        updateAll(0);

        // If we found a goal, send a song to inform the world of success
        // and if not then send ep to determine a valid command
        if(episodeContainsGoal(ep, FALSE))
        {
            ep->cmd = CMD_SONG;
            if (g_plan != NULL)
            {
                Route *r = g_plan->array[0];
                r->needsRecalc = TRUE;
            }
        }
        else
        {
            //issue the next command from the cmds array
            ep->cmd = chooseCommand();

            if (ep->cmd != cmds[i])
            {
                printf("ERROR:  plan failed at step %d\n", i);
                exit(-1);
            }
        }

    }//for

    printf("\t\t\t\t\t\t\tEND PLANTEST: SUCCESS!\n");

}//planTest

/**
 * replanTest
 *
 * This function tests whether the supervisor constructs a correct plan
 *
 * @param sensorInput a char string wth sensor data
 * @return int a command for the Roomba (negative is error)
 */
void replanTest()
{
    char* noHit     = "0000000000                 ";  // no hit is zero
    char* rightHit  = "0000000010                 ";  // right hit is 2
    char* leftHit   = "0000000001                 ";  // left hit is 1
    char* bothHit   = "0000000011                 ";  // both hit is 3
    char* goal      = "1000000000                 ";  // goal is 512

    char* sensors[] = {noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, leftHit, noHit, leftHit, noHit, leftHit, noHit,
              noHit, noHit, leftHit, leftHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              bothHit, bothHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, bothHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit, bothHit,
              noHit, rightHit, noHit, rightHit, noHit, noHit, bothHit, bothHit,
              noHit, noHit, bothHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, rightHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, bothHit, bothHit, noHit, bothHit, bothHit,
              noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit, noHit,
              noHit, noHit, noHit, noHit, noHit, noHit, bothHit, noHit, noHit,
              noHit, noHit, noHit, noHit, bothHit, noHit, noHit, rightHit,
              noHit, rightHit, rightHit, rightHit, noHit, noHit, noHit,
              /*rightHit*/ bothHit , noHit, noHit, noHit, noHit, noHit, noHit,
              goal};
   
    int cmds[] = {CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_NO_OP,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_FORWARD,
              CMD_RIGHT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_LEFT,
              CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_FORWARD, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_NO_OP,
              CMD_NO_OP, CMD_NO_OP, CMD_RIGHT, CMD_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_FORWARD, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_NO_OP, CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_LEFT, CMD_LEFT, CMD_RIGHT, CMD_FORWARD,
              CMD_RIGHT, CMD_NO_OP, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_RIGHT, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_FORWARD, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_FORWARD, CMD_NO_OP, CMD_FORWARD, CMD_RIGHT, CMD_RIGHT,
              CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_RIGHT, CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_NO_OP,
              CMD_RIGHT, CMD_FORWARD, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_LEFT, CMD_NO_OP, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_FORWARD, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT,
              CMD_NO_OP, CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_FORWARD,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_LEFT, CMD_ADJUST_LEFT,
              CMD_LEFT, CMD_NO_OP, CMD_NO_OP, CMD_NO_OP, CMD_LEFT, CMD_RIGHT,
              CMD_ADJUST_LEFT, CMD_LEFT, CMD_RIGHT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_ADJUST_LEFT,
              CMD_ADJUST_LEFT, CMD_NO_OP, CMD_ADJUST_LEFT, CMD_FORWARD,
              CMD_ADJUST_LEFT, CMD_RIGHT, CMD_FORWARD, CMD_NO_OP, CMD_FORWARD,
              CMD_FORWARD, CMD_FORWARD, CMD_LEFT, CMD_ADJUST_LEFT, CMD_RIGHT,
              CMD_FORWARD, CMD_RIGHT, CMD_NO_OP, CMD_RIGHT, CMD_NO_OP, CMD_LEFT,
              CMD_NO_OP, CMD_FORWARD};
    int i;

    //First call planTest to give it a solid plan
    planTest();

    printf("\t\t\t\t\t\t\tBEGIN REPLANTEST\n");

    for (i = 0; i < 214; i++)
    {
        // Create new Episode
        printf("Creating and adding episode...\n");
        Episode* ep = createEpisode(sensors[i]);
       
        // Add new episode to the history
        addEpisode(ep);
        printf("Episode created\n");

        updateAll(0);

        // If we found a goal, send a song to inform the world of success
        // and if not then send ep to determine a valid command
        if(episodeContainsGoal(ep, FALSE))
        {
            ep->cmd = CMD_SONG;
            if (g_plan != NULL)
            {
                Route *r = g_plan->array[0];
                r->needsRecalc = TRUE;
            }
        }
        else
        {
            //issue the next command from the cmds array
            ep->cmd = chooseCommand();

            if (ep->cmd != cmds[i])
            {
                printf("ERROR:  plan failed at step %d\n", i);
                exit(-1);

            }
        }

    }//for
   
    printf("\t\t\t\t\t\t\tEND REPLANTEST: SUCCESS!\n");

}//replanTest

/**
 * tickWME
 *
 * This function is called at regular intervals and processes
 * the recent sensor data to determine the next action to take.
 * This version is designed to handle a Vector of WMEs rather
 * than Roomba sensor data.
 *
 * Precondition: The Vector of WMEs is alphabetically sorted by
 *               attribute name.
 *
 * @param wmes A Vector* of WMEs representing agent state
 * @return int a command for the Roomba (negative is error)
 */
int tickWME(Vector* wmes)
{
    // Create new Episode
    EpisodeWME* ep = createEpisodeWME(wmes);

    // Add new episode to the history
    addEpisodeWME(ep);

        updateAll(0);
#if DEBUGGING_UPDATEALL
    printf("updateAll complete\n");
    fflush(stdout);
#endif
   
    // If we found a goal, send a song to inform the world of success
    // and if not then send ep to determine a valid command
    if(episodeContainsGoal(ep, FALSE))
    {
        printf("GOAL %d FOUND!\n", g_goalCount);
       
        ep->cmd = CMD_SONG;

        //If a a plan is in place, reward the agent and any outstanding replacements
        if (g_plan != NULL)
        {
            rewardReplacements();
            rewardAgent();
        }
       
        //Current, presumably successful, plan no longer needed
        if (g_plan != NULL)
        {
            freePlan(g_plan);
            g_plan = NULL;
        }
    }
    else
    {
        ep->cmd = chooseCommand();
    }

#if DEBUGGING
    // Print out the parsed episode if not in statsMode
    if(g_statsMode == 0)
    {
        displayEpisodeWME(ep);
    }
    fflush(stdout);
#endif

    return ep->cmd;
}//tickWME

/**
 * addEpisodeWME
 *
 * Add new level 0 episode to episode history vector
 *
 * @arg episodes pointer to vector containing episodes
 * @arg item pointer to episode to be added
 * @return int status code (0 == success)
 */
int addEpisodeWME(EpisodeWME* item)
{
    //Add the entry
    Vector* episodes = (Vector *)g_epMem->array[0];
    return addEntry(episodes, item);
}//addEpisodeWME

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

    int i;
    for(i = 0; i < ep1->sensors->size; i++)
    {
        if(!compareWME(getEntry(ep1->sensors, i), getEntry(ep2->sensors, i))) return FALSE;
    }//for

    //Compare the commands if that's required
    if(compCmd && ep1->cmd != ep2->cmd) return FALSE;

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
    // for copmarison
    if(strcmp(wme1->attr, wme2->attr) == 0 &&
       wme1->type == wme2->type)
    {
        if(wme1->type == WME_INT && wme1->value.iVal == wme2->value.iVal) return TRUE;
        if(wme1->type == WME_CHAR && wme1->value.cVal == wme2->value.cVal) return TRUE;
        if(wme1->type == WME_DOUBLE && wme1->value.dVal == wme2->value.dVal) return TRUE;
        if(wme1->type == WME_STRING && strcmp(wme1->value.sVal, wme2->value.sVal) == 0) return TRUE;
    }
    return FALSE;
}//compareWME

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

    printf("Senses:");
    int i;
    for(i = 0; i < sensors->size; i++) displayWME(getEntry(sensors, i));

    printf("\nCommand: %s", interpretCommandShort(ep->cmd));
    printf("\nTime: %d\n", ep->now);
}//displayEpisodeWME

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
    }

    // Allocate space for episode and score
    EpisodeWME* ep = (EpisodeWME*) malloc(sizeof(EpisodeWME));
    
    // Set EpisodeWME sensors vector to our WME vector
    ep->sensors = wmes;
    ep->now = timestamp++;  // Just set it to our timestamp for now
    ep->cmd = CMD_ILLEGAL;  // Default command for now
    
    /*
    This code is the old code that determines how to set the timestamp
    based on connection to roomba versus our virtual environment. We
    may (likely) need to use this again in the future.

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
        g_goalIdx[g_goalCount] = parsedData->now;
        g_goalCount++;
    }
*/
    return ep;
}//createEpisodeWME

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
        wme->attr = (char*)malloc(sizeof(char) * 2);// Account for null term.
        sprintf(wme->attr, "%i", i); // Just use the int equiv. of the sensor.
                                // Shouldn't matter since it'll be the same
                                // for each sensor string

        // Add the new WME to the vector
        addEntry(wmeVec, wme);
    }
    return wmeVec;
}//roombaSensorsToWME

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
    int i; // Is this here for a reason?...
    // Create new Episode
    Episode* ep = createEpisode(sensorInput);

    // Add new episode to the history
    addEpisode(ep);

        updateAll(0);
#if DEBUGGING_UPDATEALL
    printf("updateAll complete\n");
    fflush(stdout);
#endif

   
    // If we found a goal, send a song to inform the world of success
    // and if not then send ep to determine a valid command
    if(episodeContainsGoal(ep, FALSE))
    {
        printf("GOAL %d FOUND!\n", g_goalCount);
       
        ep->cmd = CMD_SONG;

        //If a a plan is in place, reward the agent and any outstanding replacements
        if (g_plan != NULL)
        {
            rewardReplacements();
            rewardAgent();
        }
       
        //Current, presumably successful, plan no longer needed
        if (g_plan != NULL)
        {
            freePlan(g_plan);
            g_plan = NULL;
        }

    }
    else
    {
        ep->cmd = chooseCommand();
    }

#if DEBUGGING
    // Print out the parsed episode if not in statsMode
    if(g_statsMode == 0)
    {
        displayEpisode(ep);
    }
    fflush(stdout);
#endif

    return ep->cmd;
}//tick

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
}//createEpisode

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
        parsedData->sensors[i] = bit;       //** what is this? ->
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
        g_goalIdx[g_goalCount] = parsedData->now;
        g_goalCount++;
    }

    // Command gets a default value for now
    parsedData->cmd = CMD_ILLEGAL;

    return 0;
}//parseEpisode

/**
 * updateAll                    *RECURSIVE*
 *
 * CAVEAT: This is a rather complex method.  Breaking it up into smaller methods
 * would make it more complex, in our opinion.  Please rely upon our liberal
 * comments to guide you.  Also consult research notes.
 *
 * @arg    level index into g_epMem and g_actions where the memory needs to updated   
 * @return int   success / error code
 *
 */
int updateAll(int level)
{
#if DEBUGGING_UPDATEALL
    printf("Entering level %i\n", level);
    fflush(stdout);

    printf("Level %d Episodes >>>>>>>>>>>>>>>>>>>>>>>>>>>\n", level);
    displayEpisodes(g_epMem->array[level], level);
    printf("\n");
    fflush(stdout);

    printf("Level %d Actions >>>>>>>>>>>>>>>>>>>>>>>>>>>\n", level);
    displayActions(g_actions->array[level]);
    fflush(stdout);

    printf("Level %i Sequences>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", level);
    displaySequences(g_sequences->array[level]);
    fflush(stdout);

    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< End of Level %i Data\n", level);
    fflush(stdout);
#endif
   
    // Ensure that the level is within the accepted range for the vectors
    if(level < 0 || level >= MAX_LEVEL_DEPTH)
    {
#if DEBUGGING_UPDATEALL
        printf("\tExiting updateAll(): level %d out of range\n", level);
        fflush(stdout);
#endif
        return -3;
    }

    // Create pointers to the two associated vectors we'll be working with
    Vector* actionList = g_actions->array[level];
    Vector* episodeList = g_epMem->array[level];
    Vector* sequenceList = g_sequences->array[level];

    // You need a minimum of two episodes to make an action
    if(episodeList->size <= 1)
    {
#if DEBUGGING_UPDATEALL
        printf("\tExiting updateAll(): insufficient episodes (%d) at level %d\n",
               (int)episodeList->size, level);
        fflush(stdout);
#endif
        return -1;
    }

    // We don't want any goals on the LHS of a rule, because the agent
    // "teleports" to the start state when this happens.  So abort if that's the
    // situation here.
    if (episodeContainsGoal(episodeList->array[episodeList->size - 2], level))
    {
#if DEBUGGING_UPDATEALL
        printf("\tExiting updateAll(): most recent episode is a goal\n", level);
        fflush(stdout);
#endif
        return -2;
    }

    // Create a candidate action from this current episode.  We won't add it to
    // the rule list if we later discover that an identical action already exists.
    Action* newAction          = (Action*) malloc(sizeof(Action));
    newAction->level           = level;
    newAction->epmem           = episodeList;
    newAction->outcome         = episodeList->size - 1;
    newAction->index           = episodeList->size - 2;
    newAction->length          = 1;
    newAction->freq            = 1;
    newAction->overallFreq     = NULL;
    newAction->cousins         = NULL;
    newAction->isIndeterminate = FALSE;
    newAction->containsGoal    = episodeContainsGoal(episodeList->array[episodeList->size - 1], level);

    // We need to properly initialize containsStart
    // containsStart is TRUE if this will be the very first action at
    // this level
    if (actionList->size == 0)
    {
        newAction->containsStart = TRUE;
    }
    //containsStart is TRUE if the previous action contained a goal
    else if (((Action *)actionList->array[actionList->size - 1])->containsGoal)
    {
        episodeContainsGoal(episodeList->array[episodeList->size - 1], level);
        newAction->containsStart = TRUE;
    }
    //default:  containsStart=FALSE;
    else
    {
        newAction->containsStart = FALSE;
    }

#if DEBUGGING_UPDATEALL
    printf("candidate action: ");
    fflush(stdout);
    displayAction(newAction);
    printf("\n");
    fflush(stdout);
#endif

    //Iterate over every action in the list and compare it to our new
    //candidate action.  If the candidate is unique, it'll be added to
    //the action list.  If it's a partial match (same LHS , different
    //RHS) but can't be made unique without increasing size of LHS
    //then create a pool of indeterminate actions.  If the candidate
    //matches an existing action, it'll be discarded and the existing
    //action's frequency will be updated
    int i,j;
    int matchComplete = FALSE;
    int addNewAction = TRUE;
    Action* updateExistingAction = NULL;
    for(i = 0; i < actionList->size; i++)
    {
        //Compare the i-th action to the candidate action
        Action* curr = (Action*)actionList->array[i];

        for(j = 0; j < newAction->length; j++)
        {
            //Find out if the j-th part of the LHS matches
            if (compareActOrEp(episodeList, newAction->index - j, curr->index - j, level))
            {
#if DEBUGGING_UPDATEALL
                printf("found match between %i-th entries of: ", j);
                displayAction(curr);
                printf(" and ");
                displayAction(newAction);
                printf("\n");
                fflush(stdout);
#endif

                //If the LHS match so far but we haven't reached the end
                //of either action then continue comparing them
                if (newAction->length > j+1 && curr->length > j+1)
                {
                    continue;
                }

                //If we've matched to the end and they are the same
                //length, then the LHS's match.
                if(newAction->length == curr->length)
                {
                    //If the candidate action has matched a percentage
                    //action then there may already be a matching cousin
                    //out there
                    if(curr->isIndeterminate)
                    {
#if DEBUGGING_UPDATEALL
                        printf("comparing cousins: \n");
                        fflush(stdout);
#endif
                        int k;
                        //Iterate over cousins and find one with same outcome as candidate action
                        for(k = 0; k < curr->cousins->size; k++)
                        {
                            Action* cousin = curr->cousins->array[k];

#if DEBUGGING_UPDATEALL
                            printf("\t");
                            displayAction(cousin);
                            printf(" AND ");
                            displayAction(newAction);
                            printf("\n");
                            fflush(stdout);
#endif

                            //If we find one with same outcome, increase
                            //frequency and inform not to add action
                            if (compareActOrEp(episodeList, newAction->outcome,
                                        cousin->outcome, level))
                            {
                               
                                cousin->freq++;
                                addNewAction = FALSE;
                                updateExistingAction = cousin;
                                break;
                            }
                        }//for

                        //If no cousins match candidate action, add it
                        //as a new cousin
                        if(addNewAction)
                        {
#if DEBUGGING_UPDATEALL
                            printf("new cousin is unique.  Adding...\n");
                            fflush(stdout);
#endif
                            newAction->isIndeterminate = TRUE;
                            newAction->overallFreq = curr->overallFreq;
                            newAction->cousins = curr->cousins;

                            addAction(newAction->cousins, newAction, FALSE);
                        }

                        // Regardless of whether candidate action is
                        // unique we need to increase overall
                        // frequency for all cousins in the list and
                        // end the matching process
                        (*(curr->overallFreq))++;
                        matchComplete = TRUE;
                    }
                    else    //Found a LHS match to a non-indeterminate action
                    {
                        //Now see if the RHS of both actions match
                        if (compareActOrEp(episodeList, newAction->outcome,
                                    curr->outcome, level))
                        {
                            //We have a complete match between the
                            //candidate and an existing action, so just
                            //update the existing action
                            curr->freq++;

                            //Done with update
                            matchComplete = TRUE;
                            addNewAction = FALSE;
                            updateExistingAction = curr;
                        }
                        else    //RHS does not match
                        {
#if DEBUGGING_UPDATEALL
                            printf("LHS match but RHS doesn't while comparing to %i...\n", i);
                            fflush(stdout);
#endif
                            // We want to expand the newAction and curr
                            // to create (hopefully) distinct actions
                            // There are 3 reasons this may not work.

                            // 1. Expanding curr/newAction would include
                            //    a goal on LHS
                            // 2. Expanding current action would
                            //    overflow episodic memory
                            // 3. Current action is already maximum length

                            //Check for reason #1:  Expansion creates
                            //goal on LHS
                            int newLHSEntryIndex = (newAction->index - newAction->length) - 1;

                            if (episodeContainsGoal(episodeList->array[newLHSEntryIndex],
                                             level))
                            {
#if DEBUGGING_UPDATEALL
                                printf("NewAction expands into goal at index: %i\n",
                                       newLHSEntryIndex);
                                fflush(stdout);
#endif

                                //the new action can't be expanded so we
                                //consider it degenerate so just abort
                                //and create no new actions or updates
                                matchComplete = TRUE;
                                addNewAction = FALSE;
                            }

                            //Check for reason #2: no room to expand
                            else if ((curr->length < MAX_LEN_LHS)
                                     && (curr->index - curr->length < 0))
                            {
#if DEBUGGING_UPDATEALL
                                printf("avail space: %i,  curr expands outside goal\n",
                                       curr->index - curr->length);
                                fflush(stdout);
#endif

                                //The current action can't be expanded so we
                                //consider it degenerate and replace it with the
                                //new action.

                                //AMN: We may need to handle this differently.
                                //It's pretty kludgey.
                                curr->index                             = newAction->index;
                                curr->outcome               = newAction->outcome;
                                curr->length                        = newAction->length;
                                curr->freq                                  = 1;

                                //done with update
                                matchComplete = TRUE;
                                addNewAction = FALSE;
                            }

                            //if the newAction is currently shorter than
                            //the current action, then it can safely be
                            //expanded
                            else if (newAction->length < curr->length)
                            {
                                newAction->length++;

#if DEBUGGING_UPDATEALL
                                printf("partial match with curr, extending new action to %i\n",
                                       newAction->length);
                                fflush(stdout);
                                printf("new candidate: ");
                                displayAction(newAction);
                                printf("\n");
                                fflush(stdout);
#endif
                            }

                            //If the current action can be expanded then
                            //expand both the current and candidate actions
                            else if(curr->length < MAX_LEN_LHS)
                            {
#if DEBUGGING_UPDATEALL
                                printf("len of curr action (%i) = %i < %i so increasing to %i\n",
                                       i, curr->length, MAX_LEN_LHS, curr->length+1);
                                fflush(stdout);
#endif

                                //both current action and new action can
                                //be expanded so do so in hopes that they will
                                //end up different
                                curr->length++;
                                curr->freq = 1;
                                newAction->length++;


#if DEBUGGING_UPDATEALL
                                printf("new curr:   ");
                                displayAction(curr);
                                printf("\n");

                                printf("new cand: ");
                                displayAction(newAction);
                                printf("\n");
                                fflush(stdout);
#endif

                            }
                            else  //current action can't be expanded without
                            //exceeding max length (reason #3)
                            {
#if DEBUGGING_UPDATEALL
                                printf("cousins\n");
                                fflush(stdout);
#endif

                                // We need to convert both the current action and
                                // the candidate action into indeterminate actions

                                // allocate cousins list and add both peer
                                // indetermiante actions into same cousins list
                                curr->cousins = newVector();
                                addAction(curr->cousins, curr, TRUE);
                                addAction(curr->cousins, newAction, TRUE);
                                newAction->cousins = curr->cousins;

                                //Update actions
                                curr->isIndeterminate = TRUE;
                                newAction->isIndeterminate = TRUE;
                                curr->overallFreq = (int*) malloc(sizeof(int));
                                newAction->overallFreq = curr->overallFreq;
                                *(curr->overallFreq) = curr->freq + 1;

                                //We're done with this match
                                matchComplete = TRUE;
                                addNewAction = TRUE;
                            }// else
                        }// else
                    }// else
                }// if
                else // newAction and curr have different lengths
                {
                    //If we make it here, the candidate action and
                    //current action are different lengths but they do
                    //match up to the length of the shorter action.

                    //If the candidate is longer then consider it a
                    //degenerate action and stop
                    if (newAction->length > curr->length)
                    {
                        matchComplete = TRUE;
                        addNewAction = FALSE;
#if DEBUGGING_UPDATEALL
                        printf("newAction matches but is bigger than current action.  Aborting.\n");
                        fflush(stdout);
#endif
                    }
//===============================================================================
// I think that this code needs to probably be expanded to prevent the newAction
// from expanding beyond its limits, as above.
                    //If the new action can be expanded, try doing so to
                    //see if that makes it unique
                    else if(newAction->length < MAX_LEN_LHS)
                    {
                        //-----This is the part that I added
                        int newLHSEntryIndex = (newAction->index - newAction->length) - 1;

                        if (episodeContainsGoal(episodeList->array[newLHSEntryIndex],
                                         level))
                        {
#if DEBUGGING_UPDATEALL
                            printf("2. NewAction expands into goal at index: %i\n",
                                   newLHSEntryIndex);
                            fflush(stdout);
#endif

                            //the new action can't be expanded so we
                            //consider it degenerate so just abort
                            //and create no new actions or updates
                            matchComplete = TRUE;
                            addNewAction = FALSE;
                        }
                        //----------------------------------
                        else
                        {
                            newAction->length++;

#if DEBUGGING_UPDATEALL
                            printf("expanded new action to len %i\n",
                                   newAction->length);
                            printf("new candidate: ");
                            displayAction(newAction);
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
            else // j-th episodes in actions do not match
            {
                //The current action's nth entry doesn't match so we can
                //abort the comparison even if the candidate action has
                //more entries in its LHS
                break;
            }
        }// for

        // if matched action break out of loop and free memory
        if(matchComplete == TRUE)
        {
            break;
        }
    }// for

    //Add the new action
    if(addNewAction == TRUE)
    {
#if DEBUGGING_UPDATEALL
        printf("Adding new action to level %i action list: ", level);
        displayAction(newAction);
        printf("\n");
#endif
        addAction(actionList, newAction, FALSE);

        // set this flag so that we recursively update the next level
        // with this action
        updateExistingAction = newAction;
      
    }
    else
    {
        free(newAction);
    }

    //Log that an update was completed at this level
    g_lastUpdateLevel = level;

    //If we have added a new action, or found an existing action that matches the
    //current situation then updateExistingAction will contain a pointer to that
    //action (otherwise NULL)
    if(updateExistingAction != NULL)
    {
        // add most recently seen action to current sequence
        Vector* currSequence = sequenceList->array[sequenceList->size - 1];
       
#if DEBUGGING_UPDATEALL
        printf("Adding action #%d: ", findEntry(actionList, updateExistingAction));
        displayAction(updateExistingAction);
        fflush(stdout);
        printf(" to current sequence:");
        displaySequenceShort(currSequence);
        printf("\n");
        fflush(stdout);
#endif
        addActionToSequence(currSequence, updateExistingAction);

        // if the action we just added is indeterminate or contains a
        // goal, then end the current sequence and start a new one
        // (Exception:  the agent's first action after finding a goal is
        // indeterminate and thus the sequence is only length 1)
        if (updateExistingAction->containsGoal
            || ( (updateExistingAction->isIndeterminate)
                && (currSequence->size > 1) ) )
        {
                        // if the sequence we just completed already exists then
                        // reset the vector's size to 0
                        // This will allow updateAll to reuse the same vector
                        // without needing to free memory
            Vector* duplicate = containsSequence(sequenceList, currSequence, TRUE);
            Vector *parentEpList = g_epMem->array[level + 1];
                        if(duplicate != NULL)
                        {
                                currSequence->size = 0;
                // this duplicate sequence becomes the next episode in the
                // next level's episodic memory
                if (level + 1 < MAX_LEVEL_DEPTH)
                {
                    addEntry(parentEpList, duplicate);
                }
                        }
                        else
                        {
                // this newly completed sequence becomes the next
                // episode in the next level's episodic memory UNLESS:
                // - the required level doesn't exist
                // - the sequence only contains one entry (i.e., its
                //   sole action contains a path from start to goal)
                // We've removed this test because at this point we
                //
                // think it's reasonable to include an episode that is a
                // sequence from start to goal in the next level's epMem.
                // This is becuase, even though it is a complete path, it was
                // still an even in our past as thus in our episodic memeory
                if (level + 1 < MAX_LEVEL_DEPTH)
                {
#if DEBUGGING_UPDATEALL
                    printf("Creating a new level %i episode with sequence: ", level + 1);
                    displaySequence(currSequence);
                    printf("\n");
                    fflush(stdout);
#endif
                    addEntry(parentEpList, currSequence);
                }
                // create an vector to hold the next sequence
                currSequence = newVector();
                addEntry(sequenceList, currSequence);

                        }//else

            // typically the next sequence starts with the action that
            // ended the last sequence.  (Exception:  last action
            // contains a goal)
            if (!updateExistingAction->containsGoal)
            {
                addActionToSequence(currSequence, updateExistingAction);
            }

            // this sequence has become an episode in our next level so make a
            // recursive call to update.
            if(level + 1 < MAX_LEVEL_DEPTH)
            {
                updateAll(level + 1);
            }
        }//if
       
    }//if

#if DEBUGGING_UPDATEALL
        printf("\tExiting updateAll(): completed update on level %d\n", level);
        fflush(stdout);
#endif
    return 0;
}//updateAll

/**
 * addEpisode
 *
 * Add new level 0 episode to episode history vector
 *
 * @arg episodes pointer to vector containing episodes
 * @arg item pointer to episode to be added
 * @return int status code (0 == success)
 */
int addEpisode(Episode* item)
{
    //Add the entry
    Vector* episodes = (Vector *)g_epMem->array[0];
    return addEntry(episodes, item);
}// addEpisode

/**
 * addActionToSequence
 *
 * Add an action to a sequence
 *
 * @arg sequence pointer to vector containing actions in sequence
 * @arg action pointer to action to be added
 * @return int status code (0 == success)
 */
int addActionToSequence(Vector* sequence, Action* action)
{
    return addEntry(sequence, action);
}//addActionToSequence

/**
 * addSequenceAsEpisode
 *
 * Add an sequence at level N as an episode at level N+1
 *
 * @arg sequence   pointer to vector containing actions in sequence
 * @return int status code (0 == success)
 */
int addSequenceAsEpisode(Vector* sequence)
{
    //An empty sequence should not be added!
    assert(sequence->size > 0);
   
    //determine the level of this sequence be examining one of its actions
    Action *action = (Action *)sequence->array[0];
    int level = action->level;

    //Make sure the level isn't too high
    if (level + 1 > MAX_LEVEL_DEPTH)
    {
        return LEVEL_NOT_POPULATED;
    }

    //Retrieve the episode list for the next level up
    Vector *epList = (Vector *)g_epMem->array[level + 1];

    //Insert the sequence
    return addEntry(epList, sequence);
}//addSequenceAsEpisode

/**
 * addAction
 *
 * Adds the given action to the actions array and checks if the action
 * already exists
 *
 * @arg actions        pointer to the vector of actions
 * @arg item           pointer to the Action we're adding
 * @arg checkRedundant Boolean to determine if we need to check if action exists
 *
 * @return int A status code
 */
int addAction(Vector* actions, Action* item, int checkRedundant)
{
    if(checkRedundant && actions->size > 0)
    {
        int i;
        for(i = 0; i < actions->size - 1; i++)
        {
            if(actions->array[i] == item)
            {
                return -1;
            }
        }
    }

    return addEntry(actions, item);
}//addAction

/**
 * displayEpisode
 *
 * Display the contents of an Episode struct in a verbose human readable format
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
    printf("\nTime stamp: %i\nCommand:    %s\n\n",
           (int)ep->now, interpretCommand(ep->cmd));
}//displayEpisode

/**
 * displayEpisodeShort
 *
 * Display the contents of an Episode struct in an abbreviated human readable
 * format
 *
 * @arg ep a pointer to an episode
 */
void displayEpisodeShort(Episode * ep)
{
    if (ep == NULL)
    {
        printf("<null episode!>");
        return;
    }
   
    printf("%i%s", interpretSensorsShort(ep->sensors), interpretCommandShort(ep->cmd));
}//displayEpisodeShort

/**
 * displayEpisodes
 *
 * prints a human-readable version of a vector that contains either Episodes
 * structs (level 0) or Sequences (level 1+). 
 *
 * @arg episodeList  the episode list to print
 * @arg level        the level of this episode list
 */
void displayEpisodes(Vector *episodeList, int level)
{
    //Handle an empty list
    if (episodeList->size == 0)
    {
        printf("<no episodes>\n");
        return;
    }
   
    int i;
    for(i = 0; i < episodeList->size; i++)
    {
        //Print the index number
        printf("%d.", i);

        if (level == 0)
        {
            Episode *ep = (Episode*)episodeList->array[i];
            displayEpisodeShort(ep);
            if (i + 1 < episodeList->size) printf(", "); // delimiter
        }
        else //episodeList contains sequences
        {
            //Figure out what sequence at level-1 this episode is
            Vector *epSeq = (Vector *)episodeList->array[i];
            Vector *seqList = (Vector *)g_sequences->array[level - 1];
            int index = findEntry(seqList, epSeq);
            printf("%d", index);
            if (i + 1 < episodeList->size) printf(", "); // delimiter
        }
    }//for

}//displayEpisodes

/**
 * displayActions
 *
 * prints a human-readable version of a vector of actions
 *
 * @arg actionList   the actions to display
 */
void displayActions(Vector *actionList)
{
    //Handle an empty list
    if (actionList->size == 0)
    {
        printf("<no actions>\n");
        return;
    }
   

    int i;
    for(i = 0; i < actionList->size; i++)
    {
        printf("%3i. ", i);
        displayAction(actionList->array[i]);
        printf("\n");
    }

}//displayActions

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

    // check that NULL wasn't passed
    if (sequence == NULL)
    {
        printf("Null pointer passed to displaySequence\n");
        return;
    }
   
    // handle empty sequences here
    if (sequence -> size < 1)
    {
        printf("{ <empty sequence> }");
        return;
    }

    printf("{");
    for(i = 0; i < sequence->size; i++)
    {
        displayAction(sequence->array[i]);
        printf(",");
    }

    printf("}");
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
    // handle empty sequences here
    if (sequence -> size < 1)
    {
        printf("{ <empty sequence> }");
        return;
    }


    printf("{");
    for(i = 0; i < sequence->size; i++)
    {
                // grab the level from the current sequence
                int currLevel = ((Action*)sequence->array[i])->level;
                // grab the associated actions this sequence is composed from
                Vector* actionList = g_actions->array[currLevel];
                // search for the action that the sequence is referring to
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
        if (i + 1 < sequence->size) printf(",");
    }
    printf("}");
}//displaySequenceShort

/**
 * displaySequences
 *
 * @arg sequences the sequences we want to print
 *
 */
void displaySequences(Vector* sequences)
{
    //Handle an empty list
    if (sequences->size == 0)
    {
        printf("<no sequences>\n");
        return;
    }
   
    int i, j;
    for(i = 0; i < sequences->size; i++)
    {
        printf("%3i. ", i);
        displaySequenceShort(sequences->array[i]);
        printf("\n");
    }
}//displaySequences

/**
 * displayReplacement
 *
 * prints a human-readable version of a Replacement to stdout
 *
 * @arg repl  replacement to display
 */
void displayReplacement(Replacement *repl)
{
    if (repl == NULL)
    {
        printf("{ <null replacement> }");
        return;
    }

    displaySequence(repl->original);
    printf("==>");
    displayAction(repl->replacement);
    printf(" (conf=%g)", repl->confidence);
   
}//displayReplacement

/**
 * displayAction                        *RECURSIVE*
 *
 * prints a human-readable version of an action.  If the action is a
 * sequence, it makes recursive calls until it reaches the base action.
 * The actions are printed backward (LHS on the right and vice versa)
 * for easy comparison to other actions.
 */
void displayAction(Action* action)
{
    int i,j;

    //Indicate if this is a start action
    if (action->containsStart) printf("s");
    else printf(" ");

    //Print the LHS
    for(i = 0; i < action->length; i++)
    {
        if (action->level == 0)
        {
            displayEpisodeShort((Episode*)action->epmem->array[action->index - i]);
        }
        else //sequence
        {
            //Get the episode that corresponds to the LHS of this action
            Vector *epSeq = (Vector *)action->epmem->array[action->index - i];
               
            //Lookup the index of this sequence in the list of all sequences at
            //the next level down
            Vector *seqList = (Vector *)g_sequences->array[action->level - 1];
            int index = findEntry(seqList, epSeq);

            //print the index
            printf("%i", index);
        }

    }//for

    //Print the arrow
    if(action->isIndeterminate)
    {
        printf(" -%2i-> ", action->freq * 100 / *(action->overallFreq));
    }
    else
    {
        printf(" ----> ");
    }

    //Print the RHS
    if (action->level == 0)
    {
        Vector* episodeList = (Vector*)g_epMem->array[0];
        Episode *outcomeEp = (Episode*)episodeList->array[action->outcome];

        printf("%i", interpretSensorsShort(outcomeEp->sensors));

        //Remove this (used to be used but is confusing)
        //displayEpisodeShort(outcomeEp);
    }
    else
    {
        //Get the episode at the next level down that corresponds to the
        //LHS of this action
        Vector *epSeq = (Vector *)action->epmem->array[action->outcome];
               
        //Lookup the index of this sequence in the global list of all
        //sequences
        Vector *seqList = (Vector *)g_sequences->array[action->level - 1];
        int index = findEntry(seqList, epSeq);

        //print the index
        printf("%i", index);
    }

    //Indicate if this is a goal action
    if (action->containsGoal) printf("g");


}//displayAction

/**
 * chooseCommand_SemiRandom
 *
 * This function selects a random command that would create a new action
 * based upon the agent's most recent sensing.  If no such new action can
 * be made, then it just chooses a random command without qualification.
 *
 * CAVEAT:  This routine assumes that CMD_NO_OP is the lowest numbered command
 * and that they commands are numbered consecutively afterwards.
 *
 */
int chooseCommand_SemiRandom()
{
    int i;                      // iterator
   
#if DEBUGGING
    printf("Choosing a random action: ");
    fflush(stdout);
#endif

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
    if (epList->size >= 2)
    {
        for(i = 0; i < (epList->size - 2); i++) // iterate over all but most recent
        {
            Episode *currEpisode = (Episode *)epList->array[i];
            if (compareEpisodes(currEpisode, lastEpisode, FALSE))
            {
                int index = currEpisode->cmd - CMD_NO_OP;
                valid[index] = FALSE; // guilty!  (episode already exists)
            }
        }//for
    }//if

   
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
#if DEBUGGING
            printf("%d\n", index + CMD_NO_OP);
            fflush(stdout);
#endif
            return index + CMD_NO_OP;
        }
    }

    //if no valid command is found, then just use the randomly selected default
#if DEBUGGING
    printf("%d\n", start + CMD_NO_OP);
    fflush(stdout);
#endif
    return start + CMD_NO_OP;

}//chooseCommand_SemiRandom

/**
 * nextStepIsValid
 *
 * Checks the current state versus the Route and determines
 * if the previous command's outcome matches with the expected
 * outcome of the Route.
 *
 */
int nextStepIsValid()
{
    //If there is no plan then there is no next step
    if (g_plan == NULL) return FALSE;
   
    //Get the current action that we are about to execute
    Route* level0Route = (Route *)g_plan->array[0];

    // if this is a brand new route, then the next step is always valid
    if (level0Route->currActIndex == 0 && level0Route->currSeqIndex == 0)
    {
        return TRUE;
    }//if

#if DEBUGGING_NSIV
    printf("Retrieving current action\n");
    fflush(stdout);
#endif

   
    //Retrieve the action we are about to execute
    Vector *currSequence = (Vector *)level0Route->sequences->array[level0Route->currSeqIndex];
   
#if DEBUGGING_NSIV
    printf("Current Sequence:\n");
    fflush(stdout);
    displaySequenceShort(currSequence);
    printf("\n");
    fflush(stdout);
#endif

    Action* currAction = currSequence->array[level0Route->currActIndex];

#if DEBUGGING_NSIV
    printf("Current Action at index %d of sequence at index %d: ",
           level0Route->currActIndex, level0Route->currSeqIndex);
    displayAction(currAction);
    printf("\n");
    fflush(stdout);
#endif

    //Get the current sensing from the episode list
    Vector *episodeList = g_epMem->array[0];
    Episode* currEp     = episodeList->array[episodeList->size - 1];
   
    // compare the current sensing to the sensing on the LHS of the
    // to-be-executed action
    Episode* nextStep = currAction->epmem->array[currAction->index];

#if DEBUGGING
    fflush(stdout);
    printf("comparing the current sensing:");
    printf("%i ", interpretSensorsShort(currEp->sensors));
    fflush(stdout);
    printf(" to the expected sensing: ");
    displayEpisodeShort(nextStep);
    printf("\n");
    fflush(stdout);
#endif

#if DEBUGGING_NSIV
    printf("Drawn from route: ");
    displayRoute(level0Route, FALSE);
    printf("\n");
#endif
   
    return compareEpisodes(currEp, nextStep, FALSE);
   
}//nextStepIsValid

/**
 * rewardAgent
 *
 * This method increases the agent's overall confidence so that its distance
 * from 1.0 is half of what it was.
 */
void rewardAgent()
{
#if DEBUGGING
    printf("Overall confidence increased from %g to ", g_selfConfidence);
    fflush(stdout);
#endif
   
    g_selfConfidence += (1.0 - g_selfConfidence) / 2.0;
   
#if DEBUGGING
    printf("%g\n", g_selfConfidence);
    fflush(stdout);
#endif
}//rewardAgent   

/**
 * penalizeAgent
 *
 * This method halves the agent's overall confidence.
 */
void penalizeAgent()
{
#if DEBUGGING
    printf("Overall confidence decreased from %g to ", g_selfConfidence);
    fflush(stdout);
#endif
   
    g_selfConfidence /= 2.0;
   
#if DEBUGGING
    printf("%g\n", g_selfConfidence);
    fflush(stdout);
#endif
}//penalizeAgent   

/**
 * rewardReplacements
 *
 * increases the confidence of all active replacements so that their
 * distance from 1.0 is half of what it was.  Overall confidence is
 * also increased using the same formula.
 *
 * NOTE:  If these calculations end up being expensive, we could switch to an
 * integer based confidence and use bit shift operations for division.
 *
 */
void rewardReplacements()
{
    int i;               // iterator

    //Adjust replacement rule confidences (if any)
    for(i = 0; i < g_activeRepls->size; i++)
    {
        Replacement *repl = (Replacement *)g_activeRepls->array[i];
        repl->confidence += (1.0 - repl->confidence) / 2.0;
       
#if DEBUGGING
        printf("Replacement succeeded:  ");
        displayReplacement(repl);
        printf("\n");
        fflush(stdout);
#endif
    }//for
   
    //Reset the active replacements list
    //(no need to deallocate anything since a complete list of
    //replacements is in g_replacements)
    g_activeRepls->size = 0;
   
}//rewardReplacements

/**
 * penalizeReplacements
 *
 * reviews all replacements that have been applied to a route and halves their
 * confidence.  The base formula is to halve it also but only at the topmost
 * level.  For each level below the top, the amount of decrease is half what it
 * would be at the top level.  In other words, penalizeReplacments() has the
 * exact opposite effect on Replacement rules as rewardReplacements().  The
 * method for decreasing overall confidence is different.  Overall confidence
 * only decreases for level 0 routes but when it does so it does using the
 * opposite formula as rewards.
 *
 */
void penalizeReplacements()
{
    int i;

    //Adjust replacement rule confidences (if any)
    for(i = 0; i < g_activeRepls->size; i++)
    {
        Replacement *repl = (Replacement *)g_activeRepls->array[i];
        repl->confidence = repl->confidence / 2.0;

#if DEBUGGING
        printf("Replacement failed:  ");
        displayReplacement(repl);
        printf("\n");
        fflush(stdout);
#endif
       
    }//for
   
    //Reset the active replacements list
    //(no need to deallocate anything since a complete list of
    //replacements is in g_replacements)
    g_activeRepls->size = 0;

}//penalizeReplacements

/**
 * initRouteFromParent
 *
 * examines the parent route of a given level and based upon its current action
 * creates a route the the given level containing the appropriate sequence.  The
 * existing route at the given level is deallocated.
 *
 * CAVEAT:  Caller is responsible for guaranteeing that the parent route exists!
 *
 * @arg level  the level that needs to be updated.
 * @arg LHS    specifies whether the sequence should be extracted from the
 *             left-hand-side (TRUE) or right-hand-side (FALSE) of the parent
 *             action.  Usually you will want to set this to TRUE.
 */
void initRouteFromParent(int level, int LHS)
{
    assert(level + 1 < MAX_LEVEL_DEPTH);
   
#if DEBUGGING_UPDATEPLAN
    printf("Entering initRouteFromParent() at level %d\n", level);
    fflush(stdout);
#endif
   
    //Extract the current action from the parent route. 
    Route *parentRoute = (Route *)g_plan->array[level + 1];
    Vector *parentSeq = (Vector *)parentRoute->sequences->array[parentRoute->currSeqIndex];
    Action *parentAct = (Action *)parentSeq->array[parentRoute->currActIndex];

#if DEBUGGING_UPDATEPLAN
    printf("\tretrieving route from %s of action:", LHS ? "LHS" : "RHS");
    fflush(stdout);
    displayAction(parentAct);
    printf("\n");
    fflush(stdout);
#endif
   
    //Get the correct episode from that action based on the LHS parameter.  In
    //either case, it will always be a sequence since the parent action is at
    //least a level 1 action
    Vector *seq = NULL;
    if (LHS)
    {
        seq = (Vector *)parentAct->epmem->array[parentAct->index];
    }
    else // retVal == PLAN_ON_OUTCOME
    {
        seq = (Vector *)parentAct->epmem->array[parentAct->outcome];
    }

    //create a new one based on the extracted sequence
    Route *newRoute = (Route*)malloc(sizeof(Route));
    newRoute->sequences = newVector();
    initRouteFromSequence(newRoute, seq);
   
    //replace the old route with the new
    Route *oldRoute = (Route *)g_plan->array[level];
    if (oldRoute != NULL)
    {
        freeRoute(oldRoute);
    }
    g_plan->array[level] = newRoute;

#if DEBUGGING_UPDATEPLAN
    printf("Updated level %d route with help from parent at level %d\n",
           level, level + 1);
    printf("Updated route: ");
    displayRoute(newRoute, FALSE);
    fflush(stdout);
#endif
   
}//initRouteFromParent

/**
 * updatePlan                              *RECURSIVE*
 *
 * This method is called whenever the agent completes a sequence in the level 0
 * route of a plan.  It adjusts replacement rule confidences and adjusts the
 * plan so that it's ready to continue with the next level 0 sequence.
 *
 * @arg level   the current level being updated
 *
 * @return SUCCESS or an error code
 */
int updatePlan(int level)
{
   
#if DEBUGGING_UPDATEPLAN
    printf("-----===== begin updatePlan at level %d =====-----\n", level);
    fflush(stdout);
#endif
   
    //If there is no route at this level, report this
    if (level+1 >=  MAX_LEVEL_DEPTH)
    {
        return LEVEL_NOT_POPULATED;
    }

    //Get the route at this level
    Route* route = (Route *)g_plan->array[level];
#if DEBUGGING_UPDATEPLAN
    printf("Updating Level %d Route:\n", level);
    displayRoute(route, FALSE);
    printf("\n");
    fflush(stdout);
#endif

    //Make sure the route at this level is valid
    if ( (route == NULL)
         || (route->needsRecalc)
         || (route->sequences == NULL)
         || (route->sequences->size == 0)
         || (route->sequences->size < route->currSeqIndex) )
    {
        route->needsRecalc = TRUE;
        return LEVEL_NOT_POPULATED;
    }

    // Increment the action index:   The most important step!
    (route->currActIndex)++;

    //At this point we want to decide whether we should stay in this sequence.
    //We want to switch when currActIndex points to the last action in the
    //sequence and there is a next sequence available to switch to.
    Vector *currSequence =  (Vector *)route->sequences->array[route->currSeqIndex];
    if (route->currActIndex < currSequence->size - 1)
    {
#if DEBUGGING_UPDATEPLAN
        printf("Updated level %d route successfully.\n", level);
        fflush(stdout);
#endif
        return SUCCESS;
    }

#if DEBUGGING_UPDATEPLAN
    printf("Reached the last action in this sequence at level %d\n", level);
    fflush(stdout);
#endif
   
    //======================================================================
    //If we reach this point, then we're ready to move to the next sequence if
    //possible. 
    //----------------------------------------------------------------------

    //First, see if there is a next sequence at this level
    if (route->currSeqIndex + 1 < route->sequences->size)
    {
        //Just move to the next sequence at this level
        (route->currSeqIndex)++;
        route->currActIndex = 0;

#if DEBUGGING_UPDATEPLAN
        //Make sure this next sequence is valid
        currSequence = (Vector *)route->sequences->array[route->currSeqIndex];
        assert(currSequence->size > 0);

        printf("Moved to sequence #%d of %d at level %d.\n",
               route->currSeqIndex + 1, (int)route->sequences->size, level);
        fflush(stdout);
#endif
       
        return SUCCESS;
    }//if

    //Since there is no next sequence, recursively call update plan if there is
    //a parent plan that can provide us with a next sequence
    int retVal = updatePlan(level+1);

    //Create a new route at this level based upon the newly updated parent route
    if (retVal == SUCCESS)
    {
        initRouteFromParent(level, retVal == SUCCESS);
        return SUCCESS;
    }

    //======================================================================
    //If we reach this point, then the parent route is unavailable and exhausted
    //----------------------------------------------------------------------

    //If there is one more action left on this last sequence, let it go 
    if (route->currActIndex == currSequence->size - 1)
    {
        return SUCCESS;
    }
    
    //Since there is nothing left to execute, we must report that this level is
    //done.  The lower level should still perform the outcome sequence
    //associated with the last action at this level.  
    (route->currActIndex)--;    // make this a valid value, just for safety
    route->needsRecalc = TRUE;
    return PLAN_ON_OUTCOME;


}//updatePlan

/**
* compareReplacements
*
* Compare two distinct replacements and return TRUE if they are equivalent
*
* @arg repl1   replacement to compare
* @arg repl2   replacement to compare
*
* @return TRUE if they are a match
*/
int compareReplacements(Replacement* repl1, Replacement* repl2)
{
    if (! compareSequences(repl1->original, repl2->original))
    {
        return FALSE;
    }

    return compareActions(repl1->replacement, repl2->replacement);
   
}//compareReplacements

/**
 * replacementExists
 *
 * searches g_replacements to see if the equivalent of a given replacement is
 * already present.
 *
 * CAVEAT:  Caller is responsible for providing a valid replacement
 *
 * @arg repl  the replacement to search for
 *
 * @return TRUE if the equivalent is present and FALSE otherwise
 */
int replacementExists(Replacement *repl)
{
    int i;

    //get the vector associated with this relpacement's level
    Vector *replacements = (Vector *)g_replacements->array[repl->level];

    //Iterate through the list looking for matches
    for(i = 0; i < replacements->size; i++)
    {
        Replacement *currRepl = (Replacement *)replacements->array[i];

        if (compareReplacements(currRepl, repl))
        {
            return TRUE;
        }
       
    }//for

    return FALSE;
   
}//replacementExists

/**
 * makeNewReplacement
 *
 * this method examines the current plan and creates a new replacement rule at
 * the lowest available level such that a) the rule would apply to the current
 * position in the plan and b) the rule is not a duplicate of any existing one
 * in g_replacements.
 *
 * CAVEAT:  Caller is responsible for guaranteeing that g_plan is valid
 * CAVEAT:  Caller is responsible for deallocating the returned value.
 *
 * @return a new Replacement struct (or NULL if something goes wrong)
 *
 */
Replacement *makeNewReplacement()
{
    int i,j;                    // iterators

    //This will eventually be our return value
    Replacement *result = malloc(sizeof(Replacement));
    result->confidence  = INIT_REPL_CONFIDENCE;

    //Search all levels starting at the bottom
    for(i = 0; i < MAX_LEVEL_DEPTH; i++)
    {
        //Make sure it's even worth searching
        if (! replacementPossible(i))
        {
#if DEBUGGING_FIND_REPL
            printf("A replacement can't be applied at level %d\n", i);
            fflush(stdout);
#endif
            continue;
        }

        //Extract the current sequence from the route at this level
        Route*  route   = (Route*)(g_plan->array[i]);
        Vector *currSeq = ((Vector*)route->sequences->array[route->currSeqIndex]);

        //Extract the next two actions from the current sequence
        Action *act1 = (Action*)currSeq->array[route->currActIndex];
        Action *act2 = (Action*)currSeq->array[route->currActIndex + 1];

#if DEBUGGING_FIND_REPL
        printf("Constructing a new replacment for these two actions:");
        fflush(stdout);
        displayAction(act1);
        printf(", ");
        displayAction(act2);
        printf("\n");
        fflush(stdout);
#endif
       
        //Preinit the return value
        result->level = i;
        result->original    = newVector();
        addEntry(result->original, act1);
        addEntry(result->original, act2);

        //Pick a random starting position in actions list for this level. NOTE:
        //We start the search in a random position so that the agent won't
        //always default to the lowest numbered command.
        Vector *actList = (Vector *)g_actions->array[i];
        int start = (rand() % actList->size); // random start

        //Starting at the random start position, try all possible actions until
        //we find one that creates a new, unique replacement
        for(j = 0; j < actList->size; j++)
        {
            //Retrieve the candidate action
            int index = (start + j) % actList->size;
            Action *candAct = actList->array[index];

#if DEBUGGING_FIND_REPL
        printf("\ttrying ");
        fflush(stdout);
        displayAction(candAct);
        printf("...");
        fflush(stdout);
#endif
       
            //See if the candidate is compatible with these to-be-replaced
            //actions.  This comparision is done differently at level 0 than
            //other levels
            if (i == 0)         // level 0
            {
                //The LHS sensors of candidate must match the LHS sensors of
                //act1
                Episode *candLHS = (Episode *)candAct->epmem->array[candAct->index];
                Episode *act1LHS = (Episode *)act1->epmem->array[act1->index];
                if (! compareEpisodes(candLHS, act1LHS, FALSE))
                {
#if DEBUGGING_FIND_REPL
                    printf("LHS sensors don't match.\n");
                    fflush(stdout);
#endif
       
                    continue;   // bad match, try a different candidate
                }

                //the RHS sensors of the candidate must match the RHS sensors of
                //act2.
                Episode *candRHS = (Episode *)candAct->epmem->array[candAct->outcome];
                Episode *act2RHS = (Episode *)act2->epmem->array[act2->outcome];
                if (! compareEpisodes(candRHS, act2RHS, FALSE))
                {
#if DEBUGGING_FIND_REPL
                    printf("RHS sensors don't match.\n");
                    fflush(stdout);
#endif
                    continue;   // bad match, try a different candidate
                }
            }//if
            else                // level 1 or higher
            {
                //The first action of the LHS subsequence must match
                Vector *candLHS = (Vector *)candAct->epmem->array[candAct->index];
                Vector *act1LHS = (Vector *)act1->epmem->array[act1->index];
                Action *candLHSSubAct = (Action *)candLHS->array[0];
                Action *act1LHSSubAct = (Action *)act1LHS->array[0];
                if (candLHSSubAct != act1LHSSubAct)
                {
#if DEBUGGING_FIND_REPL
                    printf("First LHS action doesn't match.\n");
                    fflush(stdout);
#endif
                    continue;   // bad match, try a different candidate
                }

                //the last action of the RHS subsequence must match
                Vector *candRHS = (Vector *)candAct->epmem->array[candAct->outcome];
                Vector *act2RHS = (Vector *)act2->epmem->array[act2->outcome];
                Action *candRHSSubAct = (Action *)candRHS->array[candRHS->size - 1];
                Action *act2RHSSubAct = (Action *)act2RHS->array[act2RHS->size - 1];
                if (candRHSSubAct != act2RHSSubAct)
                {
#if DEBUGGING_FIND_REPL
                    printf("Last RHS action doesn't match.\n");
                    fflush(stdout);
#endif
                    continue;   // bad match, try a different candidate
                }

            }//else

            //If we reach this point then the candidate is compatible.  Fill in
            //the replacement with it and see if that yields a duplicate
            result->replacement = candAct;
            if (replacementExists(result))
            {
#if DEBUGGING_FIND_REPL
                    printf("Duplicate (already exists).\n");
                    fflush(stdout);
#endif
                continue;
            }

            //All checks passed. Success!
            return result;

        }//for
           
    }//for

    // No new replacement can be made.  This happens when replacmeents are only
    // possible at some levels and at those levels all valid candidates already
    // exist with low confidence.
    free(result);
    return NULL;               
}//makeNewReplacement

/**
 * considerReplacement
 *
 * See if there is a replacement rule that the agent is confident enough to
 * apply to the current plan and apply it.
 *
 */
void considerReplacement()
{
    int i;                      // iterator

#if DEBUGGING_FIND_REPL
    printf("Entering considerReplacement...\n");
    fflush(stdout);
#endif
   
    //Avoid making more replacements per plan than allowed
    if (g_activeRepls->size >= MAX_REPLS) return;

    //Avoid being more risky with repls than allowed
    double risk = 0.0;
    for(i = 0; i < g_activeRepls->size; i++)
    {
        Replacement *repl = g_activeRepls->array[i];
        risk += (1.0 - repl->confidence);
    }
    if (risk > MAX_REPL_RISK)
    {
#if DEBUGGING_FIND_REPL
        printf("\trisk (%g) is too high for new replacement.\n", risk);
        fflush(stdout);
#endif

        return;
    }
   
    /*----------------------------------------------------------------------
     * Find the best replacement that can be applied
     *----------------------------------------------------------------------
     */
    //Retrieve the best matching existing replacement 
    Replacement *repl = findBestReplacement();

    //Also make a new replacement if the agent is confident enough
    Replacement *newRepl = NULL;
    if ( g_selfConfidence >= (1.0 - INIT_REPL_CONFIDENCE))
    {
        newRepl = makeNewReplacement();
        if (newRepl == NULL)
        {
#if DEBUGGING_FIND_REPL
            printf("Agent confidence (%g) is high enough for a new replacement but none could be made.\n", g_selfConfidence);
            fflush(stdout);
#endif
        }
    }//if

    //Choose between the best existing replacement and the new candidate
    //replacement
    if (repl == NULL)
    {
        //If both null give up
        if (newRepl == NULL)
        {
#if DEBUGGING
            printf("No valid replacement found.\n", g_selfConfidence);
            fflush(stdout);
#endif
            return;
        }
        else
        {                       // If only the newRepl is available, use it
            // add this new one to the list
            Vector *replList = (Vector *)g_replacements->array[newRepl->level];
            addEntry(replList, newRepl);

            repl = newRepl;
        }
    }
    else
    {
        //If I'm more confident in something new, try it instead
        if ((newRepl != NULL) && (newRepl->confidence > repl->confidence))
        {
            // add this new one to the list
            Vector *replList = (Vector *)g_replacements->array[newRepl->level];
            addEntry(replList, newRepl);

            repl = newRepl;
        }
    }//else
        

    //Make sure the agent is confident enough to use the selected replacement
    if (g_selfConfidence < (1.0 - repl->confidence))
    {
#if DEBUGGING
        printf("No valid replacement found.  Agent confidence (%g) too low for new replacement.\n", g_selfConfidence);
        fflush(stdout);
#endif
        //Agent is not confident enough to do a replacement
        return;
    }

    /*----------------------------------------------------------------------
     * Apply the replacement (repl) to g_plan
     *----------------------------------------------------------------------
     */
    //This guy does all the work
    applyReplacementToPlan(g_plan, repl);
   
    //Log that the replacement is active
    addEntry(g_activeRepls, repl);
   
}//considerReplacement


/**
 * chooseCommand_WithPlan
 *
 * This function increments to the next action in the current plan and extracts
 * the associated cmd to return to the caller.
 *
 * CAVEAT:  g_plan should contain a valid plan that does not need recalc
 */
int chooseCommand_WithPlan()
{
    int i;                      // iterator

#if DEBUGGING
    printf("Choosing command from plan:\n");
    fflush(stdout);
    displayPlan();
    fflush(stdout);
    printf("\n");
    fflush(stdout);
#endif
   
    //Before executing the next command in the plan, see if there is a
    //replacement rule that the agent is confident enough to apply to the
    //current plan and apply it.
    considerReplacement();

    //Get the level 0 route from from the plan
    Route* level0Route = (Route *)g_plan->array[0];

    //Extract the current action
    Vector *currSequence = (Vector *)level0Route->sequences->array[level0Route->currSeqIndex];
    Action* currAction = currSequence->array[level0Route->currActIndex];
    Episode* nextStep = currAction->epmem->array[currAction->index];

    //move the "current action" pointer to the next action as a result
    //of taking this action
    updatePlan(0);

    //return the command prescribed by the current action
    return nextStep->cmd;

}//chooseCommand_WithPlan


/**
 * chooseCommand
 *
 * This function decides what command to issue next.  Typically this selection
 * will be the next step in a plan.  However, the agent may decide to modify the
 * plan as part of the command selection process.  If no plan exists, a random
 * command is selected.
 *
 * @return int the command that was chosen
 */
int chooseCommand()
{
    int i,j;                    // iterators

#if DEBUGGING_CHOOSECMD
            printf("Entering chooseCommand\n");
            fflush(stdout);
#endif
           
    //If the agent has taken a wrong step, then it loses confidence in itself
    //and in the recently applied replacements
    if (g_plan != NULL)
    {
#if DEBUGGING_CHOOSECMD
            printf("checking to see if plan is still valid\n");
            fflush(stdout);
#endif

        if (! nextStepIsValid())
        {
#if DEBUGGING
            printf("Current plan invalid.  Replanning...:\n");
            fflush(stdout);
#endif
            //"We now consecrate the bond of obedience."  The agent and all
            //active replacements are now to be penalized for causing this
            //failure.
            penalizeReplacements();
            penalizeAgent();
           
            //Since the plan has failed, create a new one
            freePlan(g_plan);
            g_plan = initPlan(TRUE);
       
#if DEBUGGING
            if (g_plan != NULL)
            {
                printf("Replan:\n");
                fflush(stdout);
                displayPlan();
                printf("\n");
                fflush(stdout);
            }
#endif
        }//if
        else                    // The plan is going swimmingly! 
        {
#if DEBUGGING_CHOOSECMD
            printf("Plan successful so far.\n");
            fflush(stdout);
#endif
            //If a sequence has been completed then the active replacements need
            //to be applied to the new current sequence
            Route *topRoute = getTopRoute(g_plan);
            for(i = topRoute->level; i >= 0; i--)
            {
                //Has the route at this level just begun a new sequence?
                Route *currRoute = (Route *)g_plan->array[0];
                if ((currRoute->currSeqIndex > 0) && (currRoute->currActIndex <= 1))
                {
                    //MEMORY LEAK: If currRoute->replSeq is set, it points to
                    //memory that needs to be deleted at some point.
                    //Unfortunately, it can't be deleted now *and* the pointer
                    //needs to be re-used for the current sequence.  It's not a
                    //hard problem to solve but not trivial either. I'm letting
                    //it go for the short term. -:AMN:, 23 Jan 2011
                    currRoute->replSeq = NULL;
                   
                    //Reapply all active replacements at this level
                    for(j = 0; j < g_activeRepls->size; j++)
                    {
                        Replacement *currRepl = (Replacement *)g_activeRepls->array[j];
                        if (currRepl->level == i)
                        {
                            applyReplacementToPlan(g_plan, currRepl);
                        }
                    }//for
                }//if
            }//for
           
            //If a level 0 sequence has just completed then the agent's
            //confidence is increased due to the partial success
            //(Note: Index 1 rather than 0 is used due to overlap between
            //sequences.)
            Route *currRoute = (Route *)g_plan->array[0];
            if ((currRoute->currSeqIndex > 0) && (currRoute->currActIndex == 1))
            {
                rewardAgent();
            }

        }//else
       
    }//if

    //If the current plan is invalid then we first need to make a new plan
    if ( (g_plan == NULL)
         || planNeedsRecalc(g_plan) )
    {
        g_plan = initPlan(FALSE);
#if DEBUGGING
        if (g_plan != NULL)
        {
            printf("New plan:\n");
            fflush(stdout);
            displayPlan();
            printf("\n");
            fflush(stdout);
        }
#endif
    }//if


    //If there still is no plan at this point then that means the agent doesn't
    //have enough experience yet.  Select a semi-random command that would
    //create a new action. 
    if (g_plan == NULL)
    {

#if DEBUGGING
        printf("No valid plan available.  Taking a random action.\n");
        fflush(stdout);
#endif
        return chooseCommand_SemiRandom();
    }//if

    //%%%TEMPORARY:  For Dustin and Ben
    //  adding a % chance of random action depending upon how long it's been
    //  since we've reached the goal
    int randDelay = 1000;
    Vector *level0Eps = (Vector *)g_epMem->array[0];
    Episode *latest = (Episode *)level0Eps->array[level0Eps->size - 1];
    int stepsSoFar = latest->now - g_goalIdx[g_goalCount];
    if (g_goalCount == 0)
    {
        stepsSoFar = latest->now;
    }
    if (stepsSoFar > randDelay)
    {
        int rNum = (rand() % 1000); // random number 0..999
        if (stepsSoFar - randDelay > rNum)
        {
            return chooseCommand_SemiRandom();
        }
    }
    

    //If we've reached this point then there is a working plan so the agent
    //should select the next step with that plan.
    return chooseCommand_WithPlan();

}//chooseCommand

/**
 * displayRoute                 *RECURSIVE*
 *
 * prints the contents of a Route struct to stdout.  A user readable vertical
 * tree format is used to print the entire route.  Recursive calls are made to
 * handle lower levels.
 *
 *
 * @arg route     ptr to the route to print
 * @arg recurse   indicates whether the routine should recurse into sub-level
 *                sequences (if any)
 */
void displayRoute(Route *route, int recurse)
{
    Vector* sequences = route->sequences;

    //handle the empty case
    if (sequences->size == 0)
    {
        printf("\t<empty route>\n");
        return;
    }

    int i,j;
    for(i = 0; i < sequences->size; i++)
    {
        //this is the sequence that will be printed
        Vector *seq = (Vector *)sequences->array[i];
       
        //at level 0, print the index number of this sequence
        if (route->level == 0)
        {
            Vector *seqList = (Vector *)g_sequences->array[0];
            int index = findEntry(seqList, seq);
            printf("%d:", index);
        }

        //Print each entry in the sequence
        for(j = 0; j < seq->size; j++)
        {
            Action *action = seq->array[j];
           
            //At level 0, we're dealing with actual Episode structs
            //So just print a short version of the sensor data
            if (route->level == 0)
            {
                Episode *ep = (Episode*)action->epmem->array[action->index];
                displayEpisodeShort(ep);

                //If the command we just printed is the most recent
                //command then append an asterisk to indicate that
                if((i == route->currSeqIndex) && (j == route->currActIndex))
                {
                    printf("*");
                }

                //comma separator (if not the last entry)
                if (j < seq->size - 1)
                {
                    printf(", ");
                }
            }//if

            //For level 1 and higher, just print the sequence number
            //and use a recursive call to print its constituent subsequences (if
            //the user requested it)
            else
            {
                //Get the episode at the next level down that corresponds to the
                //LHS of this action
                Vector *epSeq = (Vector *)action->epmem->array[action->index];
               
                //Lookup the index of this sequence in the global list of all
                //sequences
                Vector *seqList = (Vector *)g_sequences->array[route->level - 1];
                int index = findEntry(seqList, epSeq);

                //print the index
                //indent based on level to make a vertical tree
                int indent = 2*(MAX_LEVEL_DEPTH - route->level);
                printf("%*sSequence #%d on level %d", indent, "", index, route->level - 1);
                if (recurse) printf(": ");

                //for level 2+ we want a newline now to get the tree format to
                //look right
                if (route->level >= 2) printf("\n");
                fflush(stdout);
                   
                //If the user has requested a recursive print, do that here
                if (recurse)
                {
                    //AMN: Temporarily removed.  This if-statement causes
                    //displayRoute to print the same intermediate steps for all
                    //of the members of the plan.  By using only the default
                    //behavior (else clause) the output won't reflect any
                    //replacements that have been applied.  I've decided this is
                    //the lesser of two evils.  - 22 Jan 2011
                   
                    //If this is the current sequence in the route and there
                    //really is an active plan, then the current sequence may
                    //have had replacements applied to it so we should print
                    //that so it'll be accurate
                    // if ((i == route->currSeqIndex) && (g_plan != NULL))
                    // {
                    //     //print the actual route at the next level down
                    //     Route *toPrint = (Route *)g_plan->array[route->level - 1];
                       
                    //     //Recursive call
                    //     displayRoute(toPrint, TRUE);
                    // }
                    // else

                   
                    {

                        //construct a temporary Route that represents the next
                        //level down
                        Route *tmpRoute = (Route*)malloc(sizeof(Route));
                        tmpRoute->sequences = newVector();
                        initRouteFromSequence(tmpRoute, epSeq);
                        tmpRoute->currActIndex = -1; // prevent asterisk printf
                        tmpRoute->currSeqIndex = -1;
                       
                        //Recursive call
                        displayRoute(tmpRoute, TRUE);
                       
                        //Discard the temporary route once it's been printed
                        freeRoute(tmpRoute);
                    }//else
                }//if (recurse)

                //for level 1 we want a newline now to get the tree format to
                //look right
                if (route->level == 1) printf("\n");

            }//else

            fflush(stdout);

        }// for each action

       
        //Also print the final outcome episode of the last action in the
        //sequence
        if (seq->size < 1) continue; //  (unless it's empty)
        Action *lastAction = seq->array[seq->size - 1];
        if (route->level == 0)
        {
            //just print the sensors
            Episode *ep = (Episode*)lastAction->epmem->array[lastAction->outcome];
            printf("-->%i; ", interpretSensorsShort(ep->sensors));
        }
        else // level 1+
        {
            //Get the episode at the next level down that corresponds to the
            //RHS of this action
            Vector *epSeq = (Vector *)lastAction->epmem->array[lastAction->outcome];
               
            //Lookup the index of this sequence in the global list of all
            //sequences
            Vector *seqList = (Vector *)g_sequences->array[route->level - 1];
            int index = findEntry(seqList, epSeq);

            //print the index
            //indent based on level to make a vertical tree
            int indent = 2*(MAX_LEVEL_DEPTH - route->level);
            printf("%*sSequence #%d on level %d", indent, "", index, route->level - 1);

            //for level 2+ we want a newline now to get the tree format to look
            //right
            if (route->level >= 2) printf("\n");
                   
            //If the user has requested a recursive print, then we need to
            //construct a temporary Route that represents the next level down
            if (recurse)
            {
                Route *tmpRoute = (Route*)malloc(sizeof(Route));
                tmpRoute->sequences = newVector();
                initRouteFromSequence(tmpRoute, epSeq);

                //Recursive call
                displayRoute(tmpRoute, recurse);

                //Discard the temporary route once it's been printed
                freeRoute(tmpRoute);
            }

            //for level 1 we want a newline now to get the tree format to
            //look right
            if (route->level == 1) printf("\n");
           
        }//else (print outcome episode at level 1+)

    }//for each sequence
   
}//displayRoute

/**
 * displayPlan
 *
 * Display the current route. Prints the actions that make up
 * the route in reverse order to make it easier visually to
 * read the steps.
 *
 */
void displayPlan()
{
    //Make sure there is a plan to print!
    if (g_plan == NULL)
    {
        printf("NO PLAN!\n");
        return;
    }
   
    //Find the highest level route in the plan that's not empty
    Route* r = getTopRoute(g_plan);

    //Make sure this plan has at least one route
    if (r == NULL)
    {
        printf("EMPTY PLAN!\n");
        return;
    }

    //Calculate and print the plan length
    int length = routeLength(r);
    printf("(level %d; %d steps) \n", r->level, length);
    fflush(stdout);
   
    displayRoute(r, TRUE);

   
}//displayPlan

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
    //(Note:  iteration is descending since the last action is most likely to
    //have the goal)
    for(j = seq->size-1; j >= 0; j--)
    {
        Action *currAction = (Action *)seq->array[j];

        //If this action contains a goal then record its index and break
        if (currAction->containsGoal)
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
 * this method initializes a given route with the actions in a given sequence.
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
    route->replSeq      = NULL;
    route->currActIndex = 0;
    route->currSeqIndex = 0;
    route->needsRecalc  = FALSE;

    //Calculate the level of this route by looking at a action in the sequence
    Action *r = (Action *)seq->array[0];
    route->level        = r->level;

    //This route will contain only one sequence, the given one
    addEntry(route->sequences, seq);

}//initRouteFromSequence

/**
 * freeRoute()
 *
 * This method frees the memory used by a route.
 *
 */
void freeRoute(Route *r)
{
    if (r == NULL) return;
    if (r->sequences != NULL) freeVector(r->sequences);
    if (r->replSeq != NULL) free(r->replSeq);
    free(r);

}//freeRoute


/**
 * sequenceLength                    *RECURSIVE*
 *
 * calculates the length of a sequence (counted as the number of level 0 actions
 * in the sequence).  If the given sequence is not at level 0, a recursive call
 * is made for each of its subsequences.
 *
 * @arg seq   the sequence to calculate the length of
 * @arg level the level of this sequence
 *
 * @return result
 */
int sequenceLength(Vector *seq, int level)
{
    int i;                      // iterator
    int result = 0;             // counter to sum the lengths of the sequences

    //If we're at level 0 we can just return the length
    if (level == 0)
    {
        return seq->size;
    }

    //At higher levels, sum the the lengths of the subsequences
    for(i = 0; i < seq->size; i++)
    {
        Action *action = (Action *)seq->array[i];
        Vector *subSeq = (Vector *)action->epmem->array[action->index];
       
        result += sequenceLength(subSeq, level - 1);
    }//for

    //Since the sequences overlap by one action each, result needs to be
    //decreased to avoid double counting
    result -= (seq->size - 1);

    return result;
}//sequenceLength

/**
 * routeLength
 *
 * calculates the length of a route (counted as the number of level 0 actions in
 * all the sequences that make up the route.
 *
 * NOTE:  It's not only faster but maybe beneficial to just count the number of
 * high level sequences to determine route length.  Simpler routes may be longer
 * at first but more likely to distill into shorter plans in the long run?
 *
 * @arg r the route to calculate the length of
 *
 * @return result
 */
int routeLength(Route *r)
{
    int i;                      // iterator
    int result = 0;             // counter to sum the lengths of the sequences
    for(i = 0; i < r->sequences->size; i++)
    {
        Vector *seq = (Vector *)r->sequences->array[i];
        result += sequenceLength(seq, r->level);
    }//for

    return result;
}//routeLength

/**
 * findRoute
 *
 * This method uses a breadth-first search to find a shortest path from a given
 * start state to a goal state at a given level.
 *
 * CAVEAT:  initRoute does not verify that the given sequence and route are
 *          valid/allocated
 *
 * @arg newRoute  is the Route struct to populate with this new route.
 * @arg startSeq  must be the first sequence in the route
 *
 * @return a success/error code
 */
int findRoute(Route* newRoute, Vector *startSeq)
{
    // instance variables
    Vector* route;      // the ordered list of sequences stored as
                        // int indices into actionList
    int i,j;            // counting variable
    Vector* sequences;  // pointer to sequences in level
    Vector* candRoutes = newVector();  //candidate Route structs to return to caller

#if DEBUGGING_INITROUTE
        //print the current shortest candidate
        printf("Entering initRoute with start sequence: ");
        displaySequenceShort(startSeq);
        printf("\n");
        fflush(stdout);
#endif
    /*--------------------------------------------------------------------------
     * Use the starting sequence to create a partial route.  This is the
     * first candidate
     */

    //Determine what level the route is at by looking at the first action in the
    //start sequence.
    Action *act = (Action *)startSeq->array[0];
    int level = act->level;
    assert(level+1 < MAX_LEVEL_DEPTH); // can't build plan without level+1 actions

    //Create an incomplete candidate route using the given start sequence
    Route *initCand = (Route*)malloc(sizeof(Route));
    initCand->sequences = newVector();
    initRouteFromSequence(initCand, startSeq);

    //add to the candidates list
    addEntry(candRoutes, initCand);

    /*--------------------------------------------------------------------------
     * Iterate over the candidate routes expanding them until the shortest
     * route to the goal is found (breadth-first search)
     */
    //(Note: the size of the candRoutes vector will grow as the search
    //continues.  Each candidate is a partial route.)
    int bSuccess = FALSE;
    int routeLen = -1;           // length of shortest route so far
    for(i = 0; i < candRoutes->size; i++)
    {
#ifdef DEBUGGING
        printf(".");
        fflush(stdout);
#endif

        //Find the shortest route that hasn't been examined yet
        Route *route = (Route *)candRoutes->array[i];
        routeLen = routeLength(route);
        int routePos = i;
        for(j = i+1; j < candRoutes->size; j++)
        {
            Route *possiblyShorter = (Route *)candRoutes->array[j];
            int psLen = routeLength(possiblyShorter);

            //If a shorter one is found, update route, routeLen and routePos
            if (psLen < routeLen)
            {
                route = possiblyShorter;
                routeLen = psLen;
                routePos = j;
            }//if
        }//for

        //To avoid long delays, give up on planning after examining N candidate routes
        if (i > MAX_ROUTE_CANDS)
        {
            break;
        }
        
        //%%%I tried this as an alternative to the above.  It seems to be worse!
        //%%%This needs to be investigated.
        // //To avoid long delays, give up on planning after examining N candidate routes
        // if (i > MAX_ROUTE_CANDS)
        // {
        //     break;
        // }

        //Move this shortest unexamined route to the current position in the
        //candRoutes vector via a swap
        if (routePos != i)
        {
            void *tmp = candRoutes->array[i];
            candRoutes->array[i] = candRoutes->array[routePos];
            candRoutes->array[routePos] = tmp;
        }

#if DEBUGGING_INITROUTE
        //print the current shortest candidate
        printf("examining next shortest unexamined candidate %d at %ld of size %d:\n",
               i, (long)route, routeLen);
        fflush(stdout);
#endif
       
        //If the last sequence in this route contains the goal state, we're
        //done.  Copy the details of this route to the newRoute struct we were
        //given and exit the loop.
        Vector *lastSeq = (Vector *)route->sequences->array[route->sequences->size - 1];
        int actionIdx = getGoalAction(lastSeq);
        if (actionIdx >= 0)
        {
            newRoute->level = route->level;
            newRoute->sequences = cloneVector(route->sequences);
            newRoute->currSeqIndex = 0;
            newRoute->currActIndex = 0;
            newRoute->needsRecalc = FALSE;

            bSuccess = TRUE;
            break;
        }//if

        /*----------------------------------------------------------------------
         * Search for sequences to find any that meet both these criteria:
         * 1.  The sequence is the right-hand-side of an action at
         *     level+1 such that the left-hand-side sequence is the
         *     one most recently added to the candidate route
         * 2.  the sequence is not already in the candidate route
         *
         * Then build new candidate routes by adding all sequences to the
         * current candidate route that meet these criteria .
         */

        //Iterate over all actions at level+1
        Vector *parentActions = (Vector *)g_actions->array[level + 1];
        for(j = 0; j < parentActions->size; j++)
        {
            Action *act = (Action *)parentActions->array[j];

            //If the left-hand-side of this action doesn't match the
            //last sequence in the current candidate route then skip.
            Vector *lhsSeq = (Vector *)act->epmem->array[act->index];
            if (lhsSeq != lastSeq) continue;

            //Verify this sequence isn't already in the route
            Vector* rhsSeq = (Vector *)act->epmem->array[act->outcome];
            if (findEntry(route->sequences, rhsSeq) != -1) continue;

#if DEBUGGING_INITROUTE
            //report the new route
            printf("\textending candidate with action: ");
            fflush(stdout);
            displayAction(act);
            printf("\n");
            fflush(stdout);
#endif
       
            //If we've reached this point, then we can create a new candidate
            //route that is an extension of the current one
            Route *newCand = (Route*)malloc(sizeof(Route));
            newCand->level = route->level;
            newCand->sequences = cloneVector(route->sequences);
            addEntry(newCand->sequences, rhsSeq);
            newCand->replSeq      = NULL;
            newCand->currSeqIndex = 0;
            newCand->currActIndex = 0;
            newCand->needsRecalc  = FALSE;     
       
            //Add this new candidate route to the candRoutes array
            addEntry(candRoutes, newCand);
        }//for
       
#if DEBUGGING_INITROUTE
        //delimit the loop iteration with an update
        int lastSeqIndex = findEntry(g_sequences->array[route->level], lastSeq);
        printf("\tdone searching for ways to extend from episode: %d\n",
               lastSeqIndex);
        fflush(stdout);
#endif
       
    }//for
   
#ifdef DEBUGGING
    printf("\n");
#endif

    //Clean up the RAM in the candRoutes list
    for(i = 0; i < candRoutes->size; i++)
    {
        Route *route = (Route *)candRoutes->array[i];
        freeVector(route->sequences);
        free(route);
    }
    freeVector(candRoutes);

    if (bSuccess) return SUCCESS;

    return PLAN_NOT_FOUND;
}//findRoute

/**
 * initPlan
 *
 * this method creates a plan for reaching the goal state from the starting
 * state.  A plan is a vector of routes (one per level)
 *
 *
 * @arg  isReplan   Am I creating a new plan due to a previous plan failure (TRUE/FALSE)?
 *
 * @return a pointer to the plan or NULL if no plan was found
 */
Vector* initPlan(int isReplan)
{
    int i;                      // iterator
    int offset = -1;            // if I plan from a partial match, this will
                                // be the index of the starting action

#if DEBUGGING_INITPLAN
    printf("entering initPlan for %s\n", isReplan ? "replan" : "plan");
    fflush(stdout);
#endif
       
    //Try to figure out where I am.  I can't make plan without this.
    Vector *startSeq = (Vector *)findInterimStart_NO_KNN();
    if (startSeq == NULL)
    {
        //Try a partial match
        startSeq = findInterimStartPartialMatch_NO_KNN(&offset);
        if (startSeq == NULL)
        {
            return NULL;        // I give up
        }
    }//if

   

    //Figure out what level the startSeq is at
    Action *act = (Action *)startSeq->array[0];
    int level = act->level;
   
    //Initialize an empty plan.  This will eventually be our return value.
    Vector *resultPlan = newPlan();

    //Try to initialize the route at the same level as the start sequence
    int retVal = findRoute((Route *)resultPlan->array[level], startSeq);
    if (retVal != SUCCESS)
    {
#if DEBUGGING_INITPLAN
        printf("initRoute failed\n");
        displaySequenceShort(startSeq);
        fflush(stdout);
#endif
       
        //Give up if no route can be found
        freePlan(resultPlan);
        return NULL;
    }//if

#if DEBUGGING_INITROUTE
        printf("Success: found route to goal at level: %d.\n", level);
        fflush(stdout);
        displayRoute((Route *)resultPlan->array[level], TRUE);
        printf("\n");
        fflush(stdout);
#endif
       
    //Initialize the route at subsequent levels.  Each route is based on the
    //current sequence in the route at the previous level
    for(i = level - 1; i >= 0; i--)
    {
        //Get the first sequence in the previous level's route
        Route *parentRoute = (Route *)resultPlan->array[i+1];
        Vector *parentSeq = (Vector *)parentRoute->sequences->array[0];

        //Get the first action from that sequence
        Action *parentAct = (Action *)parentSeq->array[0];

        //Get the first episode from that action.  Since the parent action is at
        //least a level 1 action, this episode will always be a sequence
        //(Vector*) whose level is the current level.
        Vector *seq = (Vector *)parentAct->epmem->array[parentAct->index];
       
        //initialize this route with the extracted sequence
        Route *currRoute = (Route *)resultPlan->array[i];
        initRouteFromSequence(currRoute, seq);
    }//for

    //If I'm replanning after the failure of a previous plan that means that the
    //first command in the new plan may not be the very first command in the
    //first sequence.
    if (isReplan)
    {
        Route *route = (Route *)resultPlan->array[0];

        //If the route was based on a partial match at level 0 then the offset
        //variable was set by findInterimStartPartialMatch
        if (offset > -1) 
        {
            route->currActIndex = offset;
        }
        //At level 1+ just skip the first command
        else                   
        {
            route->currActIndex = 1;
        }
    }//if


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

        r->level        = i;
        r->sequences    = newVector();
        r->replSeq      = NULL;
        r->currSeqIndex = 0;
        r->currActIndex = 0;
        r->needsRecalc  = FALSE;       
       
        addEntry(newPlan, r);
    }//for

    return newPlan;
   
}//newPlan

/**
 * freePlan()
 *
 * This method frees the memory used by a plan.  It frees the following memory:
 *  - the vector that the plan is made of
 *  - the Route structs at each level of the plan
 *  - the vectors in the Route structs that contain pointers
 *
 * @arg plan   the plan to free
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
        freeRoute(r);
    }//for

    freeVector(plan);

}//freePlan


/**
 * planNeedsRecalc()
 *
 * This determines whether the current plan is valid
 *
 * @arg plan   the plan to
 *
 * @return TRUE if the plan needs to be recalcualted, false otherwise
 */
int planNeedsRecalc(Vector *plan)
{
    //If the plan doesn't exist is certainly isn't valid
    if (plan == NULL) return TRUE;

    //If the lowest route of the plan needs a recalc then the whole plan does
    Route *r = (Route *)g_plan->array[0];
    return r->needsRecalc;

}//planNeedsRecalc

/**
 * getTopRoute()
 *
 * This method retrieves the highest level route in a given plan that's not
 * empty.
 *
 * @param plan  the plan to to find the top-level route in.
 *
 * @return returns a pointer to the requested route or NULL on failure
 */
Route *getTopRoute(Vector *plan)
{
    //Check for bad argument
    if (plan == NULL) return NULL;
   
    //Find the highest level route in the plan that's not empty
    int i;
    Route* r = NULL;
    for(i = MAX_LEVEL_DEPTH - 1; i >= 0; i--)
    {
        r = plan->array[i];
        if (r == NULL) continue;
        assert(r->sequences != NULL);
        if (r->sequences->size > 0)
        {
            break;
        }
           
    }//for

    return r;
}//getTopRoute

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
}//findTopMatch

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
}//compareEpisodes

/**
 * compareActions
 *
 * Compares two actions to each other and returns TRUE if they have the
 * same LHS and RHS.  This is a shallow comparison.
 *
 * @arg r1  first action to compare
 * @arg r2  second action to compare
 * @return TRUE if the actions match and false otherwise
 */
int compareActions(Action* r1, Action* r2)
{
    //Make sure that both actions use the same episodic memory
    if (r1->epmem != r2->epmem) return FALSE;

    //Make sure that both actions have the same index
    if (r1->index != r2->index) return FALSE;

    //Make sure that both actions have the same length
    if (r1->length != r2->length) return FALSE;

    return TRUE;
}//compareActions

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
* sequence of actions.
*
* @arg seq1 A vector containing the first sequence
* @arg seq2 A vector containing the second sequence
* @return TRUE if they are a match
*/
int compareSequences(Vector* seq1, Vector* seq2)
{
        // make sure they contain the same number of actions
        if(seq1->size != seq2->size) return FALSE;
        // make sure they are at the same level
        if(((Action*)seq1->array[0])->level != ((Action*)seq2->array[0])->level) return FALSE;

        int i;
        // iterate through and compare corresponding action
        for(i = 0; i < seq1->size; i++)
        {
                // if the pointers are not the same then we have no match
                if(seq1->array[i] != seq2->array[i]) return FALSE;
        }
        // success, we have a match
        return TRUE;
}//compareSequences

/**
 * compareActOrEp
 *
 * This is a general purpose compare that works with either Episode or
 * Action structs.  It calls compareActions or compareEpisodes as needed.
 *
 * @arg list        a pointer to a Vector of either Episode or Action structs
 * @arg i1         index of first entry to compare
 * @arg i2         index of second entry to compare
 * @arg level      is TRUE if vec contains episodes and FALSE if it
 *                 contains actions
 * @return TRUE if the actions match and false otherwise
 */
int compareActOrEp(Vector *list, int i1, int i2, int level)
{
    if (!level)
    {
        //If it's a base action we need to know if one of the entries
        //we're comparing has no RHS yet.
        int noRHS = (i1 == list->size - 1) || (i2 == list->size - 1);

        //Determine a match score
        return compareEpisodes(list->array[i1],
                               list->array[i2], !noRHS);
    }
    else //sequence
    {
        return compareActions(list->array[i1], list->array[i2]);
    }
}//compareActOrEp

/**
 * compareVecOrEp
 *
 * This is a general purpose compare for comparing two episodes that might
 * either be Episode structs or Vectors.
 *
 * @arg list       a pointer to a Vector of either Episode structs or Vectors
 * @arg i1         index of first entry to compare
 * @arg i2         index of second entry to compare
 * @arg level      is TRUE if vec contains vectors and FALSE if it
 *                 contains episodes (can pass the level for this)
 * @return TRUE if the items match and false otherwise
 */
int compareVecOrEp(Vector *list, int i1, int i2, int level)
{
    if (!level)                 // level 0
    {
        //If either i1 or i2 is the last episode in the list then its command
        //has not been set yet.  Detect that special case here
        int compCmd = TRUE;
        if ((i1 == list->size - 1) || (i2 == list->size - 1))
        {
            compCmd = FALSE;
        }

        //Do comparison
        return compareEpisodes(list->array[i1],
                               list->array[i2], compCmd);
    }
    else //sequence
    {
        //pointer comparison is sufficient since level 1+ episode duplicates are
        //discovered at insertion time.
        return list->array[i1] == list->array[i2];
    }
}//compareVecOrEp

/**
 * episodeContainsGoal
 *
 * This routine determines whether a given episode contains a
 * goal. Depending on the level the episode may be an Episode struct
 * or a sequence.
 *
 * @arg entry      a pointer to an Episode or Action struct
 * @arg level      is TRUE if entry is an Episode (false for an action
)
 * @return TRUE if the entry is a goal and FALSE otherwise
 */
int episodeContainsGoal(void *entry, int level)
{
    if (level == 0)
    {
        Episode* ep = (Episode *)entry;

        //For base actions, a goal is indicated by the IR sensor on the episode
        return ep->sensors[SNSR_IR];
    }
    else //sequence
    {
        //For a sequence, a goal is indicated by the "containsGoal" field being
        //TRUE on the last action in the sequence
        Vector *sequence = (Vector *)entry;
        Action *action = (Action *)sequence->array[sequence->size - 1];
        return action->containsGoal;
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

    // member variables
    int     i;          // loop iterator
    Vector* temp;       // used in init loop below

    // initialize variables
    g_epMem           = newVector();
    g_actions         = newVector();
    g_sequences       = newVector();
    g_replacements    = newVector();
    g_plan            = NULL;        // no plan can be made at this point
    g_activeRepls     = newVector();
    g_connectToRoomba = 0;
    g_statsMode       = STATS_MODE;           // no output optimization
    g_selfConfidence  = INIT_SELF_CONFIDENCE;
    g_lastUpdateLevel = -1;

    for(i = 0; i < MAX_LEVEL_DEPTH; i++)
    {
        temp = newVector();
        addEntry(g_epMem, temp);

        temp = newVector();
        addEntry(g_actions, temp);

        temp = newVector();
        addEntry(g_sequences, temp);

        temp = newVector();
        addEntry(g_replacements, temp);

        // pad sequence vector to avoid crash on first call of updateAll()
        addEntry(g_sequences->array[i], newVector());
    }

    // seed rand (sow some wild oats)
    srand(time(NULL));

//<<<<<<< local
   
//=======
//>>>>>>> other
}//initSupervisor

/**
 * endSupervisor
 *
 * Free the memory allocated for the Supervisor
 */
void endSupervisor()
{
    // loop iterators
    int i, j, k;

    // temporaries for loop below
    Vector *actionList, *episodeList, *sequenceList, *replacementList;

    //%%%TODO:  Added for now to avoid crashing.  Remove this when this method
    //%%%       is fixed
    if (g_epMem != NULL) return;
   
    // assume that the number of sequences,
    // actions and episodes is the same, level-wise
    for(i = MAX_LEVEL_DEPTH - 1; i >= 0; i--)
    {
        // Create pointers to the vectors we'll be working with
        actionList      =  g_actions->array     [i];
        episodeList     =  g_epMem->array       [i];
        sequenceList    =  g_sequences->array   [i];
        //replacementList =  g_replacements->array[i];
       
        // clean up sequences at the current level
        for(j = 0; j < sequenceList->size; j++)
        {
            freeVector((Vector*)sequenceList->array[j]); 
        }
        freeVector(sequenceList);

        //clean up actions at the current level
        for(j = 0; j < actionList->size; j++)
        {
            //If this is an inconsistent action we need to clean up
            //the cousins list before we deallocate the actions
            Vector* cousins = ((Action*)actionList->array[j])->cousins;
            if(cousins != NULL)
            {

                //Make sure no action has a reference to the cousins
                //list anymore.
                for(k = 0; k < cousins->size; k++)
                {
                    ((Action*)cousins->array[k])->cousins = NULL;
                }

                //now it's save to free the list
                freeVector(cousins);
            }

            free((Action*)actionList->array[j]);
        }//for
        freeVector(actionList);

        //clean up the episodes at this level
        for(j = 0; j < episodeList->size; j++)
        {
            //at level 0, this is an array of pointers to Episode structs
            //which need to be cleaned up.
            if (i == 0)
            {
                for(k = 0; k < episodeList->size; k++)
                {
                    Episode *ep = (Episode *)episodeList->array[k];

                    /*
                     * I'm commenting out this line for now.  It creates a huge
                     * memory leak but leaving it in creates a double free
                     * somewhere that I can't readily diagnose.  I am going to
                     * focus on the functionality right now.  -:AMN: 03 Nov 2010
                     */
                    //%%%FIXME: free(ep);
                }
            }

            //now safe to free the list
        }//for
        freeVector((Vector*)episodeList->array[j]);

       
       
    }//for

    //free the global list
    freeVector(g_epMem);
    freeVector(g_actions);

    //%%%TODO: free g_plan
    //%%%TODO: clean up g_replacements and g_activeRepls
   
    printf("end of function\n");
}//endSupervisor

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
}//interpretCommandShort

/**
 * interpretSensorsShort
 *
 * Return an integer that summaries the sensor values in an episdode.
 * Since all sensors are binary we can combine them all into one
 * binary integer.
 *
 * @arg    int* Sensors array of ints representing the sensors (must be
 *              of length NUM_SENSORS)
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
}//interpretSensorsShort

/**
 * replacementPossible
 *
 * Examines g_plan to determine whether a replacement is possible at this time
 * at a given level
 *
 * @arg level the level to check
 *
 * @return TRUE if it's possible and FALSE otherwise
 */
int replacementPossible(int level)
{
    if (g_plan == NULL) return FALSE;
   
    // check that a route exists at this level
    if (g_plan->array[level] == NULL) return FALSE;

    //Verify the route is valid
    Route*  route   = (Route*)(g_plan->array[level]);
    if ((route == NULL)
        || (route->needsRecalc)
        || (route->sequences == NULL)
        || (route->sequences->size == 0))
    {
        return FALSE;
    }

    //Verify that the current sequence exists
    Vector *currSeq = ((Vector*)route->sequences->array[route->currSeqIndex]);
    if (currSeq == NULL) return FALSE;

    // check to make sure that there are at least two actions left in the
    // currently executing sequence in the route (otherwise there's not
    // enough left to replace)
    if ( route->currActIndex + 1 >= currSeq->size ) return FALSE;

    //all checks passed
    return TRUE;

}//replacementPossible

/**
 * findBestReplacement
 *
 * Find a replacements in g_replacements that could be applied to the current
 * sequence in g_plan. If there are multiple such replacements, the one with the
 * highest confidence is returned.  The replacement isn't applied, only
 * enumerated.
 *
 * CAVEAT: This function does not find replacements across adjacent sequences in
 *         g_plan.  This might be something to consider in the future.
 *
 * @return Vector* of possible replacements; NULL is returned if no replacements
 *                 were found.
 */
Replacement* findBestReplacement()
{
    // instance variables
    Replacement *result = NULL;  // this will hold the return value
    int i, j, k, x;              // four(!) loop iterators

    assert(g_plan != NULL);

#ifdef DEBUGGING_FIND_REPL
    printf("Searching existing replacements...\n");
    fflush(stdout);
#endif
               
    // iterate through each level of replacements and routes, adding found
    // replacements to replacements as we go
    for(i = MAX_LEVEL_DEPTH - 1; i >= 0; i--)
    {
        //Make sure it's even worth searching
        if (! replacementPossible(i)) continue;

#ifdef DEBUGGING_FIND_REPL
    printf("\treplacement possible at level %d...\n", i);
    fflush(stdout);
#endif
               
        //Extract the current sequence from the route at this level
        Route*  route   = (Route*)(g_plan->array[i]);
        Vector *currSeq = ((Vector*)route->sequences->array[route->currSeqIndex]);

        //Iterate over the replacement rules for this level in search of a match
        Vector *replacements = (Vector*)g_replacements->array[i];
        Replacement *match = NULL;
        for (j = 0; j < replacements->size; j++)
        {
            //Extract this Replacement to prep for the loop below
            Replacement *candRepl = (Replacement*)replacements->array[j];

            //%%%DEBUGGING
            printf("\tconsidering ");
            displayReplacement(candRepl);
            printf("...");
            fflush(stdout);

            //Iterate over all remaining subsequences of the current sequence
            //that are the same lenght as the LHS of the replacment rule
            int stopPoint = currSeq->size - candRepl->original->size;
            for(k = route->currActIndex; k <= stopPoint; k++)
            {
                //Compare each action in the current subsequence to the
                //corresponding action in the replacement rule.  Abort on
                //a mismatch.
                match = candRepl; // default is success
                for(x = 0; x < candRepl->original->size; x++)
                {
                    Action *planAct = (Action*)currSeq->array[k + x];
                    Action *replAct = candRepl->original->array[x];
                    if (planAct != replAct)
                    {
                        match = NULL;
                        break;
                    }
                }//for

                //If a match was found we can stop searching
                if (match != NULL) break;
               
            }//for

#ifdef DEBUGGING_FIND_REPL
            if (match != NULL)
            {
                printf("match!\n");
            }
            else
            {
                printf("no match.\n");
            }
            fflush(stdout);
                
                
#endif
   
            //If we found a match, then log it if it's the best match so far
            if ( (match != NULL)
                && ((result == NULL)
                    || (match->confidence >= result->confidence)) )
            {

                result = match;
            }
                                     
        }//for

        //If a match has been found at this level then we're done
        //(NOTE:  If we ever want to remove the preference for higher level
        //replacement rules over lower ones, we can simply remove this
        //if-statement and the routine will return the replacement with the
        //highest confidence at any level.)
        if (result != NULL) break;
       
    }//for (each level)

#ifdef DEBUGGING_FIND_REPL
    printf("\tbest match: ");
    displayReplacement(result);
    printf("\n");
    fflush(stdout);
#endif
   
    //If a match was never found, then result will still be NULL at this point
    return result;
}//findBestReplacement


/**
 * applyReplacementToSequence
 *
 * Take the specified replacement and apply it to the specified
 * sequence to produce a new sequence.  The original, given, sequence is not
 * modified.
 *
 * CAVEAT: This function depends on a Replacement replacing exactly 2 Actions.
 *
 * @arg    sequence    a sequence of actions over which to apply a replacement
 * @arg    replacement the Replacement to apply
 * @return Vector*     copy of original sequence with the replacement applied
 */
Vector* applyReplacementToSequence(Vector* sequence, Replacement* replacement)
{
    // instance variables
    Vector* withReplacement;  // will hold our sequence with the replacement
                              // applied
    int i;                    // loop iterator


    // check to make sure the level of each of our replacement and sequence
    // match 
    assert(sequence->array != NULL);
    assert(((Action*)sequence->array[0])->level == replacement->level);

    //Don't bother doing replacements on sequences with insufficient actions
    //(currently hardcoded to two)
    if (sequence->size < 2) return sequence;
   
    // initialize instance variables;
    withReplacement = newVector();

    // iterate through original vector, adding elements to
    // withReplacement not applicable to replacement and substituting
    // replacements where appropriate
    int replCount = 0;
    for(i = 0; i < sequence->size; i++)
    {
        // if the next two actions in sequence match the original
        // actions in replacement, then substitute the replacement
        if ( (i < sequence->size - 1)
             && (sequence->array[i] == replacement->original->array[0])
             && (sequence->array[i+1] == replacement->original->array[1]) )
        {
            addActionToSequence(withReplacement, replacement->replacement);
            i++;  // increment an extra space so we skip over these two when the
                  // loop repeats
            replCount++;        // count how many changes were made
        }
        else
        {   // just add the next action in sequence to withReplacement
            addActionToSequence(withReplacement, (Action*)sequence->array[i]);
        }
    }//for

#if DEBUGGING_FIND_REPL
    printf("Number of changes made: %d\n", replCount);
    fflush(stdout);
#endif


    return withReplacement;
}//applyReplacementToSequence

/**
 * applyReplacementToPlan
 *
 * This method does all the bookeeping and maangement assocaited with actually
 * applying a replacment to a plan.  applyReplacementToSequence is a helper method for this
 * one.
 *
 * @param plan  the plan to apply it to
 * @param repl  the replacement to apply
 */
void applyReplacementToPlan(Vector *plan, Replacement *repl)
{
    int i;                      // loop iterator
   
#if DEBUGGING
    printf("Applying this level %d replacement to current plan:  ", repl->level);
    displayReplacement(repl);
    fflush(stdout);
    printf("\n");
#endif

    //Retrieve the route and sequence that will have the replacement applied to
    //it.
    Route* replRoute = (Route *)plan->array[repl->level];
    Vector *currSequence = (Vector *)replRoute->sequences->array[replRoute->currSeqIndex];

    //Create the new sequence that results from this replacement
    Vector *newSeq = applyReplacementToSequence(currSequence, repl);

    //Special Case:  The current action index now points off of the sequence
    if (replRoute->currActIndex >= newSeq->size)
    {
        replRoute->currActIndex = newSeq->size-1;
    }

    // free the old replacement sequence if it exists
    if (replRoute->replSeq != NULL)
    {
        free(replRoute->replSeq);
    }

    //Install the new route
    replRoute->replSeq = newSeq;
    replRoute->sequences->array[replRoute->currSeqIndex] = replRoute->replSeq;
   
    //Adjust the agent's confidence based upon its confidence in this
    //replacement
    //%%%old code: penalizeAgent();
    if (g_selfConfidence > repl->confidence)
    {
#if DEBUGGING
        printf("Overall confidence reduced from %g to ", g_selfConfidence);
        fflush(stdout);
#endif
       
        g_selfConfidence -= (g_selfConfidence - repl->confidence) / 2;
   
#if DEBUGGING
        printf("%g\n", g_selfConfidence);
        fflush(stdout);
#endif
    }

   
    //Now each lower level plan will need to be updated to reflect the
    //replacement that has been made here
    for(i = repl->level - 1; i >= 0; i--)
    {
        initRouteFromParent(i, TRUE);
    }
   
#if DEBUGGING
        printf("Adjusted plan:\n");
        fflush(stdout);
        displayPlan();
        printf("\n");
        fflush(stdout);
#endif

}//applyReplacementToPlan

/**
 * convertEpMatchToSequence
 *
 * this method takes a given subsequence of the level 0 episodes and finds a
 * sequential set of level 1 episodes that best matches it.  When it finds this
 * best match, it returns its immediate successor.
 *
 * NOTE:  If the first entry in the subsequence is not indeterminate, there will
 * be no match.  A good way to speed this method up is to add an explicit check
 * for that case.
 *
 * @arg index is the index of the last entry in the level 0 subsequence
 * @arg len   is the length of the subsequence
 */
Vector *convertEpMatchToSequence(int index, int len)
{
    int i,j;                    // iterators
    Vector *level0Eps = (Vector *)g_epMem->array[0];
    Vector *level1Eps = (Vector *)g_epMem->array[1];
    int bestMatchLen = 0;
    int bestMatchIndex = -1;

#ifdef DEBUGGING_FINDINTERIMSTART
    printf("Entering convertEpMatchToSequence() index=%d len=%d\n",
           index, len);
    fflush(stdout);
#endif
   
    //Examine all but the most recent episode at level 1.  This is done in
    //reverse order so that ties will be broken by recency.  The most recent
    //episode is not examined because we want to guarnatee that any match found
    //will have a successor for this method to return.
    for (i = level1Eps->size - 2; i >= 0; i--)
    {
#ifdef DEBUGGING_CONVERTEPMATCH
    printf("\tstarting with level 1 episode %d\n", i);
    fflush(stdout);
#endif
        int currEp1Index = i;   // level 1 episode we're currently examining
        int matchLen = 0;       // length of the current match

        //Starting at episode i, iterate backward looking for a match
        while(matchLen < len)
        {
            //Get the level 0 sequence that comprises this episode
            Vector *currSeq = (Vector *)level1Eps->array[currEp1Index];
           
#ifdef DEBUGGING_CONVERTEPMATCH
            printf("\t\texamining episode %d:", currEp1Index);
            displaySequenceShort(currSeq);
            printf("\n");
            fflush(stdout);
#endif
           
            //Compare this sequence to the level 0 match
            for(j = currSeq->size - 1; j >= 0; j--)
            {
                Episode *ep1 = (Episode *)level0Eps->array[index - matchLen];
                Action  *act = (Action *)currSeq->array[j];
#ifdef DEBUGGING_CONVERTEPMATCH
            printf("\t\textracting action %d:", act->index);
            displayAction(act);
            printf("\n");
            fflush(stdout);
#endif
                Episode *ep2 = (Episode *)act->epmem->array[act->index];

#ifdef DEBUGGING_CONVERTEPMATCH
    printf("\t\tcomparing level 0 episodes: ");
    displayEpisodeShort(ep2);
    printf(" to ");
    displayEpisodeShort(ep1);
    printf("\n");
    fflush(stdout);
#endif
                if (! compareEpisodes(ep1, ep2, TRUE))
                {
                    //Mismatch.  See if this is the best partial match that
                    //spans at least one entire level 1 episode
                    //NOTE:  One alternative is to change the if-statement below
                    //to:
                    //     if ((currEp1Index != i) && (currOffset > bestMatchLen))
                    //This would mean a lot less matches, but would encourage
                    //more random exploration and may lead to better behavior in
                    //the long run?
                    if (matchLen > bestMatchLen)
                    {
                        bestMatchLen = matchLen;
                        bestMatchIndex = i;
                    }

                    //Move on to the next episode at level 1
                    matchLen = len; // just to get out of the outer for-loop
                    break;
                }

                //Increment the level 0 offset
                matchLen++;
                if (matchLen >= len)
                {
                    break;
                }
            }//for

            //Increment the index to continue the match
            currEp1Index--;
            if (currEp1Index < 0) break;
           
        }//while

    }//for
   

    //If a match was found, return its successor
    if (bestMatchIndex != -1)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("\tConversion yielded episode at index %d in level 1:  ", bestMatchIndex+1);
        fflush(stdout);
        displaySequenceShort(level1Eps->array[bestMatchIndex + 1]);
        printf(" which matches %d of %d episodes in the level 0 match at index %d.\n",
               bestMatchLen, len, index);
        fflush(stdout);
#endif
       
        return level1Eps->array[bestMatchIndex + 1];
    }//if
           
       
#ifdef DEBUGGING_FINDINTERIMSTART
    printf("convertEpMatchToSequence failed: no corresponding sequence(s)\n");
    fflush(stdout);
#endif
   
   
    return NULL;
}//convertEpMatchToSequence

/*
* displayNeighborhood
*
* Print each member within the given KN_Neighborhood.
*
* @arg nbrHood a KN_Neighborhood* to the neighborhood being displayed
* @arg bEps    whether this neighborhood contains episodes (TRUE) or vectors (FALSE)
*/
void displayNeighborhood(KN_Neighborhood* nbrHood, int bEps)
{
    int i;
    for(i = 0; i < nbrHood->numNeighbors; i++)
    {
        printf("[Neighbor %i has NM %i] ", i, nbrHood->nValues[i]);

        if (bEps)
        {
            displayEpisodeShort((Episode *)nbrHood->neighbors[i]);
        }
        else
        {
            displaySequenceShort((Vector *)nbrHood->neighbors[i]);
        }
    }//for
}//displayNeighborhood

/**
 * evaluateNeighborhood
 *
 * examines a given neighborhood or pointers to unique entities and determines
 * which entity occurs most frequently.  Ties are broken by neighborhood
 * metric.
 *
 * @arg hood - neighborhood to evaluate
 * @arg bEps - does this neighborhood contains Episode structs (TRUE) or should
 *             the pointers themselves be compared (FALSE)
 *
 * @return the index of entry in the neighborhood that represents the "best" choice
 *
 */
int evaluateNeighborhood(KN_Neighborhood *hood, int bEps)
{
    //Create an array to store the frequency of each entry
    int* freqs = (int *)malloc(sizeof(int) * (hood->numNeighbors));

    //Determine frequencies (voting) and track highest freq
    int i;
    int highestIndex = 0;
    for(i = 0; i < hood->numNeighbors; i++)
    {
        freqs[i] = 1;           // initial value

        int j;
        for(j = i+1; j < hood->numNeighbors; j++)
        {
            //Compare the two entities differently based upon whether they are
            //episodes orn not
            if (bEps)
            {
                Episode *ep1 = hood->neighbors[i];
                Episode *ep2 = hood->neighbors[j];
                if (compareEpisodes(ep1, ep2, TRUE))
                {
                    (freqs[i])++;
                }
            }
            else                // just compare the void pointers (probably Vectors)
            {
                if (hood->neighbors[i] == hood->neighbors[j])
                {
                    (freqs[i])++;
                }
            }
                
        }//for

        //See if this entry is the new most frequent
        if (freqs[i] > freqs[highestIndex])
        {
            highestIndex = i;
        }
    }//for

#ifdef DEBUGGING_KNN
    printf("Evaluating %s hood: highest freq=%d Metric=%d index=%d of %d\n",
           bEps ? "episode" : "sequence",
           freqs[highestIndex],
           hood->nValues[highestIndex],
           highestIndex,
           hood->numNeighbors);
    displayNeighborhood(hood, bEps);
    printf("\n");
    fflush(stdout);
#endif
    
    
    //Return the entity with the highest frequency
    return highestIndex;

}//evaluateNeighborhood


/**
 * findInterimStart_KNN
 *
 * This method locates a past episode that is a best match for the agent's
 * current "location". This is an episode that follows the longest series of
 * episodes that match match the ones most recently created by the agent. This
 * is highly analagous to McCallum's NSM match.  The returned sequence can be
 * used as the "start" sequence for creating a plan.  (See initPlan().)
 *
 * NOTE:  This method does not search level 0 episodes.
 *        See findInterimStartPartialMatch()
 *
 * @return the "start" sequence that was found
 *         or NULL if the most recently completed sequence is unique in every
 *         level.
 */
Vector* findInterimStart_KNN()
{
    int level, i, j;              // loop iterators
    Vector *currLevelEpMem;       // the epmem list for the level being searched
    int lastIndex;                // the index of the last entry in currLevelEpMem
    int matchLen = 0;             // length of current match

#ifdef DEBUGGING_FINDINTERIMSTART
    printf("Entering findInterimStart()\n");
    fflush(stdout);
#endif
    
    //Create a KNN pool for managing our best matches
    KN_Neighborhood* hood = KN_initNeighborhood(0, K_NEAREST);
   
    //Iterate over all levels that are not the very top or bottom
    for(level = g_lastUpdateLevel; level >= 1; level--)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("\tsearching Level %d\n", level);
        fflush(stdout);
#endif
        //Set the current episode list and its size for this iteration
        currLevelEpMem = g_epMem->array[level];
        lastIndex = currLevelEpMem->size - 1;

        //starting with the penultimate level iterate backwards looking for a
        //subsequence that matches the current position
        for(i = lastIndex - 1; i >= 0; i--)
        {
            //Count the length of the match at this point
            matchLen = 0;
            while(currLevelEpMem->array[i - matchLen]
                  == currLevelEpMem->array[lastIndex - matchLen])
            {
                matchLen++;

                //don't fall off the edge
                if (i - matchLen < 0) break;
            }

            //Add this match to the neighborhood if it's cool enough
            KN_addNeighbor(hood, currLevelEpMem->array[i + 1], matchLen);
            
        }//for

        //If any match was found at this level, then stop searching
        if (hood->numNeighbors >= MIN_NEIGHBORS)
        {
            break;
        }

        //Reset the neighborhood for the next level
        KN_cleanNeighborhood(hood);
    }//for

    //Check for no match found
    if (hood->numNeighbors < MIN_NEIGHBORS)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("findInterimStart failed: all new sequences are unique\n");
        fflush(stdout);
#endif
        return NULL;
    }

    //***If we reach this point, we've found a match.
    int resultIndex = evaluateNeighborhood(hood, FALSE);
    Vector *result = (Vector *)hood->neighbors[resultIndex];
    KN_destroyNeighborhood(hood);

#ifdef DEBUGGING_FINDINTERIMSTART
        printf("\tSearch Result of length %d in level %d:  ",
               hood->nValues[0], level);
        fflush(stdout);
        displaySequenceShort(result);
        // printf(" which comes after: ");
        // displaySequenceShort(currLevelEpMem->array[bestMatchIndex]);
        // printf(" and which matches: ");
        // displaySequenceShort(currLevelEpMem->array[currLevelEpMem->size - 1]);
        printf("\n");
        fflush(stdout);
#endif
        return result;
   
}//findInterimStart_KNN

/**
 * findInterimStartPartialMatch_KNN
 *
 * Like findInterimStart(), this method searches episodes for the best match to
 * the present.  However, it only searches level 0 and since the planning
 * routines need a sequences to build plans, it takes its best match and returns
 * it as a level 0 sequence and an offset into that sequence.  The plan is built
 * from the sequence but begins where the match left off
 *
 * @arg offset is the index of the action in the returned sequence that a new
 * plan should start with
 *
 * @return the "start" sequence that was found or NULL if there was no partial
 *         match
 */
Vector *findInterimStartPartialMatch_KNN(int *offset)
{
    int i,j,k;                    // iterators
    Vector *level0Eps = (Vector *)g_epMem->array[0];
    Vector *level1Eps = (Vector *)g_epMem->array[1];
    int lastIndex = level0Eps->size-1; // where the match begins
    int matchLen = 0;             // length of current match

    //Must be at least two level 1 episodes to do a partial match
    if (level1Eps->size < 2) return NULL;
   
#ifdef DEBUGGING_FINDINTERIMSTART
    printf("Entering findInterimStartPartialMatch()\n");
    fflush(stdout);
#endif

    //Create a KNN pool for managing our best matches
    KN_Neighborhood* hood = KN_initNeighborhood(0, K_NEAREST);
   
    /*======================================================================
     * Step 1: Find the best match by comparing the level 0 episode sequence to
     *         itself
     *----------------------------------------------------------------------
     */
    
    //We want to start the search at the second-to-last level 1 episode since
    //our return value will be the "next" episode.  Calculate what offset in the
    //level 0 episodes that correponds to.  This means skipping the
    //currently-incomplete last level 0 sequence and the last level 1 sequence.
    int startingOffset = level0Eps->size;
    //Reduce by the size of last level 0 sequence
    Vector* level0Seqs = (Vector *)g_sequences->array[0];
    Vector* lastLevel0Seq = (Vector *)level0Seqs->array[level0Seqs->size - 1];
    startingOffset -= lastLevel0Seq->size;
    //Reduce by the size of the last level 1 episode
    Vector* lastLevel1Ep = (Vector *)level1Eps->array[level1Eps->size - 1];
    startingOffset -= lastLevel1Ep->size;
    //If there is overlap between the two last level 0 sequence and last level 1
    //episode, adjust them accordingly
    Action *lastAct = (Action *)lastLevel1Ep->array[lastLevel1Ep->size - 1];
    Episode *lastEp = (Episode *)lastAct->epmem->array[lastAct->index];
    if (lastEp->cmd != CMD_SONG)
    {
        startingOffset += 1;
    }
    
    
    //Iterate backwards over the level 0 episodes finding the longest
    //subsequence that matches the most recent episodes
    for(i = startingOffset; i >= MIN_LEVEL0_MATCH_LEN; i--)
    {
        //Count the length of the match at this point
        matchLen = 0;
        while(TRUE)
        {
            //don't fall off the end of the array
            if (i-matchLen < 0) break;
           
            Episode *ep1 = (Episode *)level0Eps->array[(level0Eps->size - 1) - matchLen];
            Episode *ep2 = (Episode *)level0Eps->array[i-matchLen];

            if (compareEpisodes(ep1, ep2, matchLen > 0))
            {
                matchLen++;
            }
            else
            {
                break;          // end of match
            }
        }//while
           
        //Add this match to the neighborhood if it's cool enough
        if (matchLen >= MIN_LEVEL0_MATCH_LEN)
        {
            KN_addNeighbor(hood, level0Eps->array[i+1], matchLen);
        }
            
    }//for


#ifdef DEBUGGING_FINDINTERIMSTART
    printf("\tsearch complete\n");
    fflush(stdout);
#endif
   
    //Check for no acceptable match found
    if (hood->numNeighbors < MIN_NEIGHBORS)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("findInterimStartPartialMatch found no match\n");
        fflush(stdout);
#endif
        return NULL;
    }

    //Extract the best match based upon frequency of appearane in the
    //neighborhood
    int resultIndex = evaluateNeighborhood(hood, TRUE);
    Episode *result = (Episode *)hood->neighbors[resultIndex];
    KN_destroyNeighborhood(hood);
    

    /*======================================================================
     * Step 2: Iterate backwards over all the level 1 episodes to figure out
     *         which one corresponds to the partial match found at level 0.
     * ----------------------------------------------------------------------
     */
    
    //Index into the level 0 episodes
    int zeroIndex = startingOffset - 1;
    //Special case:  if startingOffset is right before a goal, skip it now
    Episode *ep = (Episode *)level0Eps->array[zeroIndex - 1];
    if (ep->cmd == CMD_SONG)
    {
        zeroIndex -= 2;
    }

    int bestMatchIndex = result->now;             // index of level 0 episode that is best match
    
    //indexes into the level 1 episodes
    int currSeqIndex = level1Eps->size - 2;       // index of curr level 1 episode
    Vector *currSeq =
        (Vector *)level1Eps->array[currSeqIndex]; // curr level 1 episode
    int currActIndex = currSeq->size - 1;         // index of current action in
                                                  // level 1 episode

    //Iteration ends when I've re-reached the best match position.  As I proceed
    //there I'm also decrementing the currSeqIndex and currActIndex appropriately.
    while(zeroIndex > bestMatchIndex)
    {
        //decrement the zeroIndex
        zeroIndex--;

        //Special case: 512SO episodes don't end up in sequences so skip
        Episode *ep = (Episode *)level0Eps->array[zeroIndex];
        if (ep->cmd == CMD_SONG)
        {
            zeroIndex--;
        }

        //decrement current action index
        currActIndex--;
       
        //If I fall off the edge of a sequence, proceed to the next sequence
        if (currActIndex < 0)
        {
            //Special case:  best match is the very first action
            if (currSeqIndex == 0)
            {
                currActIndex = 0;
                break;
            }
            
            //Get next sequence
            currSeqIndex--;
            currSeq = (Vector *)level1Eps->array[currSeqIndex];

            //Unless this sequence ended in a goal, currActIndex should point
            //to the second-to-last action in the new sequence because of the
            //overlap between the two sequences
            if (ep->cmd != CMD_SONG)
            {
                currActIndex = currSeq->size-2;
            }
            else
            {
                currActIndex = currSeq->size-1;
            }

                
        }//if
       
    }//while

#ifdef DEBUGGING_FINDINTERIMSTART
    printf("\tPartial match result of length %d corresponds to level 1 sequence %d and and action %d:  ",
           matchLen, currSeqIndex, currActIndex);
    fflush(stdout);
    for(i = 0; i < currSeq->size; i++)
    {
        int bestMatchLen = hood->nValues[bestMatchIndex];
        if (i == currActIndex - bestMatchLen) printf("[*");
        Action *act = (Action *)currSeq->array[i];
        Episode *ep = (Episode *)act->epmem->array[act->index];
        displayEpisodeShort(ep);
        if (i == currActIndex) printf("*]");
        if (i != currSeq->size - 1) printf(",");
    }
    printf("\n");
    fflush(stdout);
#endif

    //Report the result
    *offset = currActIndex;
    return currSeq;
   
}//findInterimStartPartialMatch_KNN

/**
 * findInterimStart_NO_KNN
 *
 * This method locates a past episode that is a best match for the agent's
 * current "location". This is an episode that follows the longest series of
 * episodes that match match the ones most recently created by the agent. This
 * is highly analagous to McCallum's NSM match.  The returned sequence can be
 * used as the "start" sequence for creating a plan.  (See initPlan().)
 *
 * NOTE:  This method does not search level 0 episodes.
 *        See findInterimStartPartialMatch()
 *
 * @return the "start" sequence that was found
 *         or NULL if the most recently completed sequence is unique in every
 *         level.
 */
Vector* findInterimStart_NO_KNN()
{
    int level, i, j;              // loop iterators
    Vector *currLevelEpMem;       // the epmem list for the level being searched
    int lastIndex;                // the index of the last entry in currLevelEpMem
    int bestMatchIndex = 0;       // position and
    int bestMatchLen = 0;         // length of best match found
    int matchLen = 0;             // length of current match

#ifdef DEBUGGING_FINDINTERIMSTART
    printf("Entering findInterimStart()\n");
    fflush(stdout);
#endif
   
    //Iterate over all levels that are not the very top or bottom
    for(level = g_lastUpdateLevel; level >= 1; level--)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
    printf("\tsearching Level %d\n", level);
    fflush(stdout);
#endif
   
        //Set the current episode list and its size for this iteration
        currLevelEpMem = g_epMem->array[level];
        lastIndex = currLevelEpMem->size - 1;

        //starting with the penultimate level iterate backwards looking for a
        //subsequence that matches the current position
        for(i = lastIndex - 1; i >= 0; i--)
        {
            //Count the length of the match at this point
            matchLen = 0;
            while(currLevelEpMem->array[i - matchLen]
                  == currLevelEpMem->array[lastIndex - matchLen])
            {
                matchLen++;

                //don't fall off the edge
                if (i - matchLen < 0) break;
            }

            //See if we've found a new best match
            if (matchLen > bestMatchLen)
            {
                bestMatchLen = matchLen;
                bestMatchIndex = i;
            }
        }//for

        //If any match was found at this level, then stop searching
        if (bestMatchLen > 0) break;
    }//for

    //Check for no match found
    if (bestMatchLen == 0)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("findInterimStart failed: all new sequences are unique\n");
        fflush(stdout);
#endif
        return NULL;
    }

    //***If we reach this point, we've found a match.

#ifdef DEBUGGING_FINDINTERIMSTART
        printf("\tSearch Result of length %d at index %d in level %d:  ",
               bestMatchLen, bestMatchIndex + 1, level);
        fflush(stdout);
        displaySequenceShort(currLevelEpMem->array[bestMatchIndex + 1]);
        printf(" which comes after: ");
        displaySequenceShort(currLevelEpMem->array[bestMatchIndex]);
        printf(" and which matches: ");
        displaySequenceShort(currLevelEpMem->array[currLevelEpMem->size - 1]);
        printf("\n");
        fflush(stdout);
#endif
        return currLevelEpMem->array[bestMatchIndex + 1];
   
}//findInterimStart_NO_KNN


/**
 * findInterimStartPartialMatch_NO_KNN
 *
 * Like findInterimStart(), this method searches episodes for the best match to
 * the present.  However, it only searches level 0 and since the planning
 * routines need a sequences to build plans, it takes its best match and returns
 * it as a level 0 sequence and an offset into that sequence.  The plan is built
 * from the sequence but begins where the match left off
 *
 * @arg offset is the index of the action in the returned sequence that a new
 * plan should start with
 *
 * @return the "start" sequence that was found or NULL if there was no partial
 *         match
 */
Vector *findInterimStartPartialMatch_NO_KNN(int *offset)
{
    int i,j,k;                    // iterators
    Vector *level0Eps = (Vector *)g_epMem->array[0];
    Vector *level1Eps = (Vector *)g_epMem->array[1];
    int lastIndex = level0Eps->size-1; // where the match begins
    int matchLen = 0;             // length of current match
    int bestMatchLen = 0;         // length of the best match
    int bestMatchIndex = -1;    // index of level 1 episode that is best match
    int bestMatchOffset = -1;   // index of first matching action in best match

    //Must be at least two level 1 episodes to do a partial match
    if (level1Eps->size < 2) return NULL;
   
#ifdef DEBUGGING_FINDINTERIMSTART
    printf("Entering findInterimStartPartialMatch()\n");
    fflush(stdout);
#endif

    /*======================================================================
     * Step 1: Find the best match by comparing the level 0 episode sequence to
     *         itself
     *----------------------------------------------------------------------
     */
    
    //We want to start the search at the second-to-last level 1 episode since
    //our return value will be the "next" episode.  Calculate what offset in the
    //level 0 episodes that correponds to.  This means skipping the
    //currently-incomplete last level 0 sequence and the last level 1 sequence.
    int startingOffset = level0Eps->size;
    //Reduce by the size of last level 0 sequence
    Vector* level0Seqs = (Vector *)g_sequences->array[0];
    Vector* lastLevel0Seq = (Vector *)level0Seqs->array[level0Seqs->size - 1];
    startingOffset -= lastLevel0Seq->size;
    //Reduce by the size of the last level 1 episode
    Vector* lastLevel1Ep = (Vector *)level1Eps->array[level1Eps->size - 1];
    startingOffset -= lastLevel1Ep->size;
    //If there is overlap between the two last level 0 sequence and last level 1
    //episode, adjust them accordingly
    Action *lastAct = (Action *)lastLevel1Ep->array[lastLevel1Ep->size - 1];
    Episode *lastEp = (Episode *)lastAct->epmem->array[lastAct->index];
    if (lastEp->cmd != CMD_SONG)
    {
        startingOffset += 1;
    }
    
    
    //Iterate backwards over the level 0 episodes finding the longest
    //subsequence that matches the most recent episodes
    for(i = startingOffset; i >= bestMatchLen; i--)
    {
        //Count the length of the match at this point
        matchLen = 0;
        while(TRUE)
        {
            //don't fall off the end of the array
            if (i-matchLen < 0) break;
           
            Episode *ep1 = (Episode *)level0Eps->array[(level0Eps->size - 1) - matchLen];
            Episode *ep2 = (Episode *)level0Eps->array[i-matchLen];

            if (compareEpisodes(ep1, ep2, matchLen > 0))
            {
                matchLen++;    
            }
            else
            {
                break;          // end of match
            }
        }//while
           
        //See if we've found a new best match
        if (matchLen > bestMatchLen)
        {
            bestMatchLen    = matchLen;
            bestMatchIndex  = i;
               
#ifdef DEBUGGING_FINDINTERIMSTART
            printf("\tBest partial match so far length %d at index %d:  ",
                   bestMatchLen, bestMatchIndex);
            fflush(stdout);
            for(k = matchLen-1; k >= 0; k--)
            {
                Episode *ep = level0Eps->array[i - k];
                displayEpisodeShort(ep);
                if (k != 0) printf(",");
            }
            printf("\n");
            fflush(stdout);
#endif
        }//if
    }//for


#ifdef DEBUGGING_FINDINTERIMSTART
    printf("\tsearch complete\n");
    fflush(stdout);
#endif
   
    //Check for no acceptable match found
    if (bestMatchLen < MIN_LEVEL0_MATCH_LEN)
    {
#ifdef DEBUGGING_FINDINTERIMSTART
        printf("findInterimStartPartialMatch found no match\n");
        fflush(stdout);
#endif
        return NULL;
    }

    /*======================================================================
     * Step 2: Iterate backwards over all the level 1 episodes to figure out
     *         which one corresponds to the partial match found at level 0.
     * ----------------------------------------------------------------------
     */
    
    //Index into the level 0 episodes
    int zeroIndex = startingOffset - 1;
    //Special case:  if startingOffset is right before a goal, skip it now
    Episode *ep = (Episode *)level0Eps->array[zeroIndex - 1];
    if (ep->cmd == CMD_SONG)
    {
        zeroIndex -= 2;
    }

    
    //indexes into the level 1 episodes
    int currSeqIndex = level1Eps->size - 2;       // index of curr level 1 episode
    Vector *currSeq =
        (Vector *)level1Eps->array[currSeqIndex]; // curr level 1 episode
    int currActIndex = currSeq->size - 1;         // index of current action in
                                                  // level 1 episode

    //Iteration ends when I've re-reached the best match position.  As I proceed
    //there I'm also decrementing the currSeqIndex and currActIndex appropriately.
    while(zeroIndex > bestMatchIndex + matchLen)
    {
        //decrement the zeroIndex
        zeroIndex--;

        //Special case: 512SO episodes don't end up in sequences so skip
        Episode *ep = (Episode *)level0Eps->array[zeroIndex];
        if (ep->cmd == CMD_SONG)
        {
            zeroIndex--;
        }

        //decrement current action index
        currActIndex--;
       
        //If I fall off the edge of a sequence, proceed to the next sequence
        if (currActIndex < 0)
        {
            //Special case:  best match is the very first action
            if (currSeqIndex == 0)
            {
                currActIndex = 0;
                break;
            }
            
            //Get next sequence
            currSeqIndex--;
            currSeq = (Vector *)level1Eps->array[currSeqIndex];

            //Unless this sequence ended in a goal, currActIndex should point
            //to the second-to-last action in the new sequence because of the
            //overlap between the two sequences
            if (ep->cmd != CMD_SONG)
            {
                currActIndex = currSeq->size-2;
            }
            else
            {
                currActIndex = currSeq->size-1;
            }

                
        }//if
       
    }//while

#ifdef DEBUGGING_FINDINTERIMSTART
    printf("\tPartial match result of length %d corresponds to level 1 sequence %d and and action %d:  ",
           matchLen, currSeqIndex, currActIndex);
    fflush(stdout);
    for(i = 0; i < currSeq->size; i++)
    {
        if (i == currActIndex - bestMatchLen) printf("[*");
        Action *act = (Action *)currSeq->array[i];
        Episode *ep = (Episode *)act->epmem->array[act->index];
        displayEpisodeShort(ep);
        if (i == currActIndex) printf("*]");
        if (i != currSeq->size - 1) printf(",");
    }
    printf("\n");
    fflush(stdout);
#endif

    //Report the result
    *offset = currActIndex;
    return currSeq;
   
}//findInterimStartPartialMatch_NO_KNN





