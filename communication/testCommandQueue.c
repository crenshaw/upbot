/*
 * testCommandQueue.c
 *
 * A small test file for testing the command queue data
 * structure.
 *
 * @author Tanya L. Crenshaw
 * @since 3 June 2010
 *
 */

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

  createCommandQueue(sharedArea, 1);

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
      printf("\n   parent created, waiting on child....");

      // Wait for the child to finish working in the critical section.
      WAIT_CHILD();

      printf("parent done waiting; printing command queue header: \n");      

      // Parent prints the command queue, should see changes; that is, there
      // should be changes made to writerPos for as many commands that the
      // child wrote to the queue.
      printCommandQueueHeader(sharedArea);
      printf("\n   parent should see changes made to command queue by child.\n\n");

      // Print the commands that the child wrote to the shared command
      // queue.
      printCommandQueueEntry(sharedArea, 0);
      printCommandQueueEntry(sharedArea, 1);
      printCommandQueueEntry(sharedArea, 2);

      // Use the get() operation to retrieve and remove all of the commands
      // from the queue.
      printf("\n   parent to get all commands from the queue\n");

      printf("\n   parent got command code '%c' from the queue.", getCommandCodeFromQueue(sharedArea));
      printf("\n   parent got command code '%c' from the queue. \n\n", getCommandCodeFromQueue(sharedArea));

      printf("\n   parent attempting to print commands that should have been removed from the queue:\n\n");

      // Attempt to print the entries that should have been removed in
      // the previous get() operation.
      printCommandQueueEntry(sharedArea, 0);
      printCommandQueueEntry(sharedArea, 1);

      // Tell the child process that work on the critical section is
      // complete.
      TELL_CHILD(pid);      

      exit(0);

    }

  // ------------------------------------------------------------------------
  // Child (writer)
  // ------------------------------------------------------------------------
  else
    {

      // Attempt to print an empty command.
      printf("   attempt to print an empty command...");
      printCommand(myFirstCommand);

      // Build a command and write it to the queue.  Print the command
      // queue header to see the writerPos change.
      printf("   child creating command 1...\n");
      constructCommand(&myFirstCommand, &myCommandCode);
      printCommand(myFirstCommand);
      printf("   child writing command 1 to queue.\n");
      writeCommandToQueue(sharedArea, myFirstCommand);

      // Alter the command code so that the second command is
      // different.
      myCommandCode++;

      // Build a command and write it to the queue.  Print the command
      // queue header to see the writerPos change.
      printf("   child creating command 2...\n");
      constructCommand(&mySecondCommand, &myCommandCode);
      printCommand(mySecondCommand);
      printf("   child writing command 2 to queue...");
      writeCommandToQueue(sharedArea, mySecondCommand);
      printf("\n   child printing command queue header: \n");
      printCommandQueueHeader(sharedArea);

      // Let the parent process know that the child is done working
      // in the critical section.
      TELL_PARENT(getppid());

      // Wait for the parent to complete its critical section.
      WAIT_PARENT();

      printf("\n   child cleaning up all memory: \n\n");

      free(myFirstCommand);
      free(mySecondCommand);
      free(sharedArea);

      printf("--- Complete ---");

      fflush(stdout);

      exit(0);

    }

  
}
