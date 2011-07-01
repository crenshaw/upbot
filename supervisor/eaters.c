/**
* eaters.c
*
* This runs a virtual environment called Eaters
* where a Supervisor attempts to amass the greatest score
* possible by 'eating' items in a small world.
*
* Author: Zachary Paul Faltersack
* Last Edit: June 1, 2011
*
*/

#include "eaters.h"

#define DEBUGGING 0

// map dimensions
int g_map_width;
int g_map_height;
int g_initX;
int g_initY;

int g_X;
int g_Y;
char* g_color;
int** g_world;

// Keep track of some state variables
int g_numMoves;
int g_score;
int g_reward;

/**
 * initWorld
 *
 * This function initializes the Eaters environment in a 'random'
 * layout of walls and food.
 *
 * @param firstInit A boolean to indicate if memory needs to be allocate for the map
 */
void initWorld(int firstInit)
{
    int i,j;

	//--Allocate memory if this is the first time initWorld is called
	if(firstInit)
    {
        g_map_width 	= MAP_WIDTH;
        g_map_height 	= MAP_HEIGHT;
#if DEBUGGING
        printf("Allocating memory1\n");
#endif
        srand(time(NULL)); 	
#if DEBUGGING
        printf("Allocating memory2\n");
#endif
        g_world = (int**)malloc(g_map_width * sizeof(int*));
        for(i = 0; i < g_map_width; i++) g_world[i] = (int*)malloc(g_map_height * sizeof(int));

#if DEBUGGING
        printf("Allocating memory3\n");
#endif
        g_color = (char*)malloc(sizeof(char) * 10);
        strcpy(g_color, "red");
    }//if

#if DEBUGGING
    printf("Setting globals\n");
#endif
    //--Set up globals------------
    g_X 			= (rand() % (g_map_width - 2)) + 1;;
    g_Y 			= (rand() % (g_map_height - 2)) + 1;;
    g_numMoves 		= 0;
    g_score 		= 0;
    g_reward        = 0;
    g_statsMode 	= FALSE;

    // Save the initial coords for resetting the environment
    g_initX         = g_X;
    g_initY         = g_Y;
    //----------------------------

#if DEBUGGING
    printf("Setting environment\n");
#endif
    //--Set up the environment with bounding walls and food supplies
    for(i = 0; i < g_map_width; i++)
    {
        for(j = 0; j < g_map_height; j++)
        {
            if(i == 0 || i == g_map_width - 1 || 
                    j == 0 || j == g_map_height - 1) g_world[i][j] = V_E_WALL;
            else if(i == SFOODC1 || i == SFOODC2 || 
                    i == SFOODC3 || i == SFOODC4 || 
                    i == SFOODC5) g_world[i][j] = V_E_FOOD2;
            else g_world[i][j] = V_E_FOOD1;
        }//for
    }//for

#if DEBUGGING
    printf("Adding internal walls\n");
#endif
    //--Set up internal walls in random layout
    int numWalls = 0, percentWalls = MAP_PERCENT_WALLS;
    while(numWalls++ < (g_map_height - 2) * (g_map_width - 2) * (percentWalls) / 100)
    {
        i = (rand() % (g_map_width - 2)) + 1;
        j = (rand() % (g_map_height - 2)) + 1;

        g_world[i][j] = V_E_WALL;
    }//while

#if DEBUGGING
    printf("Inserting agent\n");
#endif
    //--Insert our agent
    g_world[g_X][g_Y] = V_E_AGENT;
}//initWorld

/**
 * resetWorld
 *
 * This function refills the environment with food, without
 * changing the location of the existing internal walls.
 */
void resetWorld()
{
    int i,j;

#if DEBUGGING
    printf("Resetting globals\n");
#endif
    //--Set up globals------------
    g_X 			= g_initX;
    g_Y 			= g_initY;
    g_numMoves 		= -1; // Account for the increment associated
                          // with the RESET command
    g_score 		= 0;
    g_reward        = 0;
    g_statsMode 	= FALSE;
    //----------------------------

#if DEBUGGING
    printf("Resetting environment\n");
#endif
    //--Set up the environment with bounding walls and food supplies
    for(i = 1; i < g_map_width - 1; i++)
    {
        for(j = 1; j < g_map_height - 1; j++)
        {
            // If this is an internal wall, skip it
            if(g_world[i][j] == V_E_WALL) continue;

            // Otherwise set to the appropriate food value
            else if(i == SFOODC1 || i == SFOODC2 || 
                    i == SFOODC3 || i == SFOODC4 || 
                    i == SFOODC5) g_world[i][j] = V_E_FOOD2;
            else g_world[i][j] = V_E_FOOD1;
        }//for
    }//for

#if DEBUGGING
    printf("Resetting agent location\n");
#endif
    //--Insert our agent
    g_world[g_initX][g_initY] = V_E_AGENT;
}//resetWorld

/**
 * freeWorld
 *
 * This function frees the map of the environment used by the unit test
 */
void freeWorld()
{
#if DEBUGGING
    printf("Freeing memory\n");
#endif
    int i;
    for(i = 0; i < g_map_width; i++)
    {
        free(g_world[i]);
    }
    free(g_world);
}//freeWorld

/**
 * displayWorld
 *
 * Print the current view of the world
 */
void displayWorld()
{
    int i,j;
    // Iterate down columns
    for(i = 0; i < g_map_height; i++)
    {
        // Iterate across rows
        for(j = 0; j < g_map_width; j++)
        {
            // Switch on item under pointer
            switch(g_world[j][i])
            {
                case V_E_EMPTY:
                    if(!g_statsMode) printf(" ");
                    break;
                case V_E_WALL:
                    if(!g_statsMode) printf("W");
                    break;
                case V_E_FOOD1:
                    if(!g_statsMode) printf("-");
                    break;
                case V_E_FOOD2:
                    if(!g_statsMode) printf("+");
                    break;
                case V_E_AGENT:
                    if(!g_statsMode) printf("O");
                    break;
            }//switch
        }//for
        if(!g_statsMode) printf("\n");
    }//for
    if(!g_statsMode) printf("\n");
}//displayWorld

/**
 * unitTest
 *
 * This subroutine emulates a Roomba in the grid world defined by world
 * It receives an action from the supervisor and updates the world map with its
 * location. This allows us to determine the next set of sensor data to return
 * to the supervisor.
 *
 * @arg command This is a command from the supervisor
 * @arg needCleanup Use as Boolean, if TRUE then test is over and need to free memory
 *
 * @return char* a string containing fake sensor data
 */
char* unitTest(int command, int needCleanup)
{
    // Check if we've completed the unit test and need to clean up
    if(needCleanup)
    {
        freeWorld();
        return NULL;
    }//if

    return doMove(command);
}//unitTest

/**
 * doMove
 *
 * This function applies the command received from the Supervisor to the
 * world and returns the resulting sensor string
 *
 * @arg command An integer representing the command to complete
 * @return A pointer to char buffer containing the sensor string to return
 */
char* doMove(int command)
{
    /*
       If the spot is empty, then it will contain a 0, else
       it will contain one of the two rewards. If a wall is
       there we won't move into it.
       So first we check for a wall in our destination.
       If free, we first gather the reward in the location
       (0,5,10)
       Then we add that to the running score, free our current
       location, and move into the destination square.
     */
    g_reward = 0;   // Reset here just in case a wall is in our way
    // Switch on the command that was received to update world appropriatelY
    switch(command)
    {
        // Left and Right just turn the Roomba
        case CMD_MOVE_N:
            if(!g_statsMode) printf("Move north...\n");
            if(g_world[g_X][g_Y - 1] != V_E_WALL)
            {
                g_reward = g_world[g_X][g_Y - 1];
                g_score += g_reward;
                g_world[g_X][g_Y] = V_E_EMPTY;
                g_Y--;
                g_world[g_X][g_Y] = V_E_AGENT;
            }//if
            else if(!g_statsMode) printf("Cannot complete command.\n");
            break;
        case CMD_MOVE_S:
            if(!g_statsMode) printf("Move south...\n");
            if(g_world[g_X][g_Y + 1] != V_E_WALL)
            {
                g_reward = g_world[g_X][g_Y + 1];
                g_score += g_reward;
                g_world[g_X][g_Y] = V_E_EMPTY;
                g_Y++;
                g_world[g_X][g_Y] = V_E_AGENT;
            }//if
            else if(!g_statsMode) printf("Cannot complete command.\n");
            break;
        case CMD_MOVE_E:
            if(!g_statsMode) printf("Move east...\n");
            if(g_world[g_X + 1][g_Y] != V_E_WALL)
            {
                g_reward = g_world[g_X + 1][g_Y];
                g_score += g_reward;
                g_world[g_X][g_Y] = V_E_EMPTY;
                g_X++;
                g_world[g_X][g_Y] = V_E_AGENT;
            }//if
            else if(!g_statsMode) printf("Cannot complete command.\n");
            break;
        case CMD_MOVE_W:
            if(!g_statsMode) printf("Move west...\n");
            if(g_world[g_X - 1][g_Y] != V_E_WALL)
            {
                g_reward = g_world[g_X - 1][g_Y];
                g_score += g_reward;
                g_world[g_X][g_Y] = V_E_EMPTY;
                g_X--;
                g_world[g_X][g_Y] = V_E_AGENT;
            }//if
            else if(!g_statsMode) printf("Cannot complete command.\n");
            break;
        case CMD_NO_OP:
            if(!g_statsMode) printf("No operation...\n");
            break;
        case CMD_EATERS_RESET:
            if(!g_statsMode) printf("Reset Eaters environment...\n");
            resetWorld();
            break;
        default:
            if(!g_statsMode) printf("Invalid command: %i\n", command);
            break;
    }//switch

    g_numMoves++;

    if(!g_statsMode) displayWorld();

    return setSenseString(command);
}//doMove

/**
 * setSenseString
 *
 * This function puts together the sensor string to be sent to the agent.
 *
 * @return char*	The sensor data to be sent to the Supervisor
 */
char* setSenseString(int command)
{
    // Memory for data string to return to Supervisor
    char* str = (char*) malloc(sizeof(char) * 150); 

    // Fill out sensor string
    if(command == CMD_EATERS_RESET)
    {
        sprintf(str, "Reset: Success");
    }
    else
    {
        sprintf(str, ":UL,i,%d:UM,i,%d:UR,i,%d:LT,i,%d:RT,i,%d:LL,i,%d:LM,i,%d:LR,i,%d:score,i,%d:steps,i,%d:color,s,%s:reward,i,%d:", 
                g_world[g_X - 1][g_Y - 1], g_world[g_X][g_Y - 1], g_world[g_X + 1][g_Y - 1], 
                g_world[g_X - 1][g_Y], g_world[g_X + 1][g_Y], 
                g_world[g_X - 1][g_Y + 1], g_world[g_X][g_Y + 1], g_world[g_X + 1][g_Y + 1],
                g_score, g_numMoves, g_color, g_reward);
    }

    return str;
}//setSenseString

