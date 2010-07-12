/** 
 * commandQueue.c
 *
 * Source file for the command queue, a data structure used for
 * managing high-level commands received from a remote machine.
 * The command queue is implemented as a small circular queue
 * where each entry in the queue is of type command_t.  The command_t
 * type comprises a single character command and timestamp.
 *
 * @author Tanya L. Crenshaw
 * @since 3 June 2010
 *
 */

#include "communication.h"
#include "commandQueue.h"

/**
 * constructCommand()
 *
 * Given a single character command, generate a timestamp, and 
 * construct a well-formed command_t command.  This function
 * allocates the necessary memory 
 *
 * @arg cmd the address of the pointer to the command about to
 * be constructed.
 *
 * @arg code the single-character command code.
 *
 * @return 0 if successful, -1 otherwise
 * 
 */
int constructCommand(command_t ** cmd, char * code)
{

  // Check all pointers before progressing.
  if(cmd != NULL && *cmd == NULL && code != NULL)
    {

      *cmd = (command_t *)malloc(sizeof(command_t));
      
      if (*cmd == NULL)
	{
	  // Memory allocation failed.  Do not proceed.
	  return -1;
	}
      
      // Initialize command_t struct
      (*cmd)->command = *code;
      time(&((*cmd)->timestamp)); 
    }

  // The pointers passed in are not well-formed, cannot construct the
  // command.
  return -1;

}

/**
 * printCommand()
 *
 * print a command_t command.
 * 
 * @arg cmd a pointer to a command_t command
 *
 * @return void
 *
 */
void printCommand(command_t * cmd)
{
  if(cmd != NULL)
    {
      printf("   print command: %c received at %d seconds\n", cmd->command, cmd->timestamp);
      return;
    }

  printf("   empty command.\n");
  return;
}


/**
 * createCommandQueue()
 *
 * Given a pointer to already allocated shared memory and a maximum size,
 * initialize a command queue data structure and store it in the shared
 * memory.
 * 
 * @arg ptr a pointer to already allocated shared memory.
 * @arg size the maximum number of commands to be stored by the queue.
 * 
 * @return 0 if successful, -1 otherwise.
 *
 */
int createCommandQueue(caddr_t ptr, int size)
{

  // Declare counter variable
  int i;

  // Check all pointers before progressing. 
  if ( ptr != NULL )
    {
      // Initialize command queue struct
      *((int *)(ptr + CQ_SIZE_OFFSET)) = size;
      *((int *)(ptr + CQ_RPOS_OFFSET)) = 0;
      *((int *)(ptr + CQ_WPOS_OFFSET)) = 0;
     
      // Calculate address of first command and timestamp
      char * command = (char *)(ptr + CQ_QUEUE_OFFSET);
      time_t * timestamp = (time_t *)(command + CQ_COMMAND_TIMESTAMP_OFFSET);

      // Initialize the queue memory area with canary values.
      for(i = 0; i < size; i++)
	{
	  *command = CQ_COMMAND_CANARY_VALUE;
	  *timestamp = 0;

	  command += CQ_COMMAND_SIZE;
	  timestamp += CQ_COMMAND_SIZE;
	}

      // Allocation and initialization successful.
      return 0;
    }

  // The ptr passed in is a NULL pointer, cannot create the queue.
  return -1;
}

/**
 * writeCommandToQueue()
 *
 * Write a new command to the queue.
 * 
 * @arg q the commandQueue to write to.
 * @arg cmd the command_t type command to write.
 *
 * @return 0 if successful, -1 otherwise
 */
int writeCommandToQueue(caddr_t q, command_t * cmd)
{
  fprintf(stdout, "%s : cmd: %d", __FILE__, cmd->command);
  // Get the writer's position and queue size
  int writerPos =   *((int *)(q + CQ_WPOS_OFFSET));
  int qSize = *((int *)(q + CQ_SIZE_OFFSET));

  // Check that the writer's position doesn't exceed the size
  if(writerPos >= qSize)
    {
      // Reset writerPos back to 0 in this circular command queue.
      writerPos = 0;
    }


  // Calculate address of command queried
  char * command = (char *)(q + CQ_QUEUE_OFFSET);
  command += CQ_COMMAND_SIZE * writerPos;
  time_t * timestamp = (time_t *)(command + CQ_COMMAND_TIMESTAMP_OFFSET);

#ifdef DEBUG
  printf("\nSource %s, Line %d:  command address = 0x%x\n", __FILE__, __LINE__, command);
  printf("\nSource %s, Line %d:  timestamp address = 0x%x\n", __FILE__, __LINE__, timestamp);
#endif

  // Write the single-character command to the queue.
  *command = cmd->command;

  // Write the timestamp to the queue.
  *timestamp = cmd->timestamp;  
  
  // Increment the writer's position.
  writerPos++;
  *((int *)(q + CQ_WPOS_OFFSET)) = writerPos;

   return 0;
}

/**
 * getCommandFromQueue()
 *
 * Get a command from the queue.  The command is returned and removed
 * from the queue.
 *
 * @arg q a pointer to the queue.
 *
 * @return if successful, the get() function returns a
 * single-character command, and return CQ_COMMAND_CANARY_VALUE
 * otherwise.
 */
char getCommandCodeFromQueue(caddr_t q)
{

  // Get the reader's position and queue size
  int readerPos =   *((int *)(q + CQ_RPOS_OFFSET));
  int qSize = *((int *)(q + CQ_SIZE_OFFSET));

  // Calculate address of single-character command code
  char * command = (char *)(q + CQ_QUEUE_OFFSET);
  command += (CQ_COMMAND_SIZE * readerPos);

#ifdef DEBUG
  printf("\nSource %s, Line %d:  readerPos = %d\n", __FILE__, __LINE__, readerPos);
  printf("\nSource %s, Line %d:  command address = 0x%x\n", __FILE__, __LINE__, command);
#endif

  // Obtain the command 
  char returnValue = *command;

  // If the single-character command code read from the queue is a
  // canary value then no command was actually read and the reader's
  // position shouldn't be advanced.  If it is not a canary value, then
  // advance the reader's position.
  if(returnValue != CQ_COMMAND_CANARY_VALUE)
    {
      readerPos++;

      // Check that the writer's position doesn't exceed the size
      if(readerPos >= qSize)
	{
	  // Reset writerPos back to 0 in this circular command queue.
	  readerPos = 0;
	}

      // Update reader's position in the queue
      *((int *)(q + CQ_RPOS_OFFSET)) = readerPos;

      // Clean out the command entry
      *command = CQ_COMMAND_CANARY_VALUE;
      time_t * timestamp = (time_t *)(command + CQ_COMMAND_TIMESTAMP_OFFSET);
      *timestamp = 0;

      // Return the single-character command code read from the queue.
      return returnValue;
    }
  
  // A canary value was read from the queue, return it to indicate no
  // command was available for getting.
  return CQ_COMMAND_CANARY_VALUE;
}

/**
 * peakCommandFromQueue()
 *
 * Get a command from the queue.  The command is returned and not removed
 * from the queue
 *
 * @arg q a pointer to the queue.
 *
 * @return if successful, the get() function returns a
 * single-character command, and return CQ_COMMAND_CANARY_VALUE
 * otherwise.
 */
char peakCommandCodeFromQueue(caddr_t q)
{

  // Get the reader's position and queue size
  int readerPos =   *((int *)(q + CQ_RPOS_OFFSET));
  int qSize = *((int *)(q + CQ_SIZE_OFFSET));

  // Calculate address of single-character command code
  char * command = (char *)(q + CQ_QUEUE_OFFSET);
  command += (CQ_COMMAND_SIZE * readerPos);

#ifdef DEBUG
  printf("\nSource %s, Line %d:  readerPos = %d\n", __FILE__, __LINE__, readerPos);
  printf("\nSource %s, Line %d:  command address = 0x%x\n", __FILE__, __LINE__, command);
#endif

  // Obtain the command 
  char returnValue = *command;

  // If the single-character command code read from the queue is a
  // canary value then no command was actually read and the reader's
  // position shouldn't be advanced.  If it is not a canary value, then
  // advance the reader's position.
  if(returnValue != CQ_COMMAND_CANARY_VALUE)
    {
      return returnValue;
    }
  
  // A canary value was read from the queue, return it to indicate no
  // command was available for getting.
  return CQ_COMMAND_CANARY_VALUE;
}

/**
 * printCommandQueueEntry()
 *
 * Given an entry in the command queue, the command at that entry is
 * printed; No check is made to determine if the entry contains a
 * well-formed command.  If the entry given excceeds the queue size,
 * then this is indicated.
 * 
 * @arg entry an integer indicating which entry of the command queue
 * to print.
 * 
 * @return void
 * 
 */
void printCommandQueueEntry(caddr_t q, int entry)
{

  // Get the queue size
  int qSize = *((int *)(q + CQ_SIZE_OFFSET));

  if(entry >= qSize)
    {
      printf("Queried entry exceeds queue size!\n");
      return;
    }

  // Calculate address of command queried
  char * command = (char *)(q + CQ_QUEUE_OFFSET);
  command += (CQ_COMMAND_SIZE * entry);
  time_t * timestamp = (time_t *)(command + CQ_COMMAND_TIMESTAMP_OFFSET);

  if( *command == CQ_COMMAND_CANARY_VALUE)
    {
      printf("   command queue entry %d == empty entry \n", entry);
      return;
    }

#ifdef DEBUG
  printf("\nSource %s, Line %d:  command address = 0x%x\n", __FILE__, __LINE__, command);
#endif

  printf("   command queue entry %d == %c at %d seconds\n", entry, *command, *timestamp);

  return;

}


/**
 * printCommandQueueHeader()
 *
 * Print the header information for the command queue, including
 * maximum size, reader position, writer position, and pointer
 * to the memory storing the actual commands
 */
void printCommandQueueHeader(caddr_t q)
{

  printf("\n   command queue header: ");

  // The pointer is not NULL, assume for the best, and print the contents
  // of the command queue header.
  if( q != NULL)
    {
       	  printf("\n    Size: %d. \n    Reader Pos: %d. \n    Writer Pos: %d \n    pointer: 0x%x \n",
		 *((int *)(q + CQ_SIZE_OFFSET)),
		 *((int *)(q + CQ_RPOS_OFFSET)),
		 *((int *)(q + CQ_WPOS_OFFSET)),
		 (int *)(q + CQ_QUEUE_OFFSET));
	  
	  return;
    }
   
  // The pointer is NULL, indicate the queue is empty or not
  // well-formed.
  printf(" uninitialized.\n");
  return;
}


