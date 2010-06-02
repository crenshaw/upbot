#include <signal.h>

// The following pair of header files is needed for pid_t, the process ID type.
#include <sys/types.h>
#include <unistd.h>

// Function prototypes for parent-child signaling
static void sig_usr(int signo);
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void WAIT_PARENT(void);
void TELL_CHILD(pid_t pid);
void WAIT_CHILD(void);
