/** 
 * commandQueue.h
 *
 * Header file for the command queue, a data structure used for
 * managing high-level commands received from a remote machine.  The
 * command queue is used by an asynchronous writer and reader in
 * separate processes.  The command queue is implemented as a small
 * circular queue where each entry in the queue is of type command_t.
 * The command_t type comprises a single character command and
 * timestamp.  processes.
 *
 * @author Tanya L. Crenshaw
 * @since 3 June 2010
 *
 */

// The command type comprises a single-character command and 
// a timestamp indicating the time that the single-character
// command was received.
typedef struct commandTag {

  char command;           // The command received

  time_t timestamp;       // A timestamp indicating when the command
                          // was received.
} command_t;


// The commandQueue is a data structure used by an asynchronous writer
// and reader.  It is possible that the writer and reader may produce
// and consume at different rates.  Thus, to maintain this structure,
// one must keep track of size, the position of the reader, the position
// of the writer, and the pointer to memory.
// 
// If the commandQueue were defined as a struct, it would be like so:


/*
  typedef struct commandQueueTag {

  int size;               // The size of the command queue, the maximum
                          // number of commands it can store.

  int readerPos;          // The last position read from.
  int writerPos;          // The last position written to.
  caddr_t queue;          // A pointer to the queue.

} commandQueue;

*/

// However, because the commandQueue will be located in a piece of
// shared memory mapped to a device (the fastest way I know to
// implement IPC in linux), all of these fields will be calculated
// manually.  The manual offset definitions follow.

#define CQ_SIZE_OFFSET 0x00
#define CQ_RPOS_OFFSET 0x04
#define CQ_WPOS_OFFSET 0x08
#define CQ_QUEUE_OFFSET 0x0C
#define CQ_COMMAND_SIZE (sizeof(command_t))
#define CQ_COMMAND_TIMESTAMP_OFFSET (sizeof(char))

#define CQ_COMMAND_CANARY_VALUE ('(')

// Possible operations on the queue, implemented in commandQueue.c
int constructCommand(command_t ** cmd, char * code);
int createCommandQueue(caddr_t ptr, int size);
int writeCommandToQueue(caddr_t q, command_t * cmd);
char getCommandCodeFromQueue(commandQueue * q);
void printCommand(command_t * cmd);
void printCommandQueueHeader(caddr_t q);
void printCommandQueueEntry(caddr_t q, int entry);

