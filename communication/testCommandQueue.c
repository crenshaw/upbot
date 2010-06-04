
// A small throw-away test file for testing the command queue data
// structure.

#include "communication.h"
#include "commandQueue.h"
#include "tell.h"

int main(void)
{

  command_t * myFirstCommand = NULL;
  command_t * mySecondCommand = NULL;
  
  char myCommandCode = 'a';

  int pid;

  caddr_t sharedArea = NULL;

  printf("Running Command Queue Test... \n\n");

  printf("Test 1.  Attempt to print uninitialized command queue: ");

  printCommandQueueHeader(sharedArea);

  printf("\nTest 2.  Creating shared memory area and initialize as command queue");
  printf("\n   create small piece of shared memory, map to /dev/zero...");
  
  // Create a small piece of shared memory.
  if(createSharedMem("/dev/zero", &sharedArea) == -1)
    {
      perror("createSharedMem()");
      return -1;
    }

  printf("shared memory created at address 0x%x...", sharedArea);  

  // Create the command queue to be shared between processes
  printf("done.\n   creating empty command queue...");

  createCommandQueue(sharedArea, 10);

  printf("done.\n   ");
  printCommandQueueHeader(sharedArea);

  printf("\nTest 3.  Make two processes, exchange commands over the command queue.\n");

  // Set up signaling.
  TELL_WAIT();

  // Fork two processes.  One to read and one to write.
  if (( pid = fork()) < 0)
    {
      perror("fork error");
    }

  // ------------------------------------------------------------------------
  // Parent (reader)
  // ------------------------------------------------------------------------
  else if (pid > 0) 
    {
      printf("   parent created, waiting on child....");

      WAIT_CHILD();

      printf("parent done waiting; printing command queue header: \n");      

      printCommandQueueHeader(sharedArea);

      printf("\n   parent should see changes made to command queue by child.\n");

      printCommandQueueEntry(sharedArea, 0);
      printCommandQueueEntry(sharedArea, 1);
      printCommandQueueEntry(sharedArea, 2);

    }

  // ------------------------------------------------------------------------
  // Child (writer)
  // ------------------------------------------------------------------------
  else
    {
      printf("   attempt to print an empty command...");

      printCommand(myFirstCommand);

      printf("   child creating command...");

      constructCommand(&myFirstCommand, &myCommandCode);

      printf("   child writing command to queue...");

      writeCommandToQueue(sharedArea, myFirstCommand);

      myCommandCode++;

      constructCommand(&mySecondCommand, &myCommandCode);

      printf("   child writing command to queue...");

      writeCommandToQueue(sharedArea, mySecondCommand);

      //printCommandQueueHeader(sharedArea);

      TELL_PARENT(getppid());

      printCommand(myFirstCommand);

      free(myFirstCommand);
      free(mySecondCommand);
      free(sharedArea);

    }
  
}
