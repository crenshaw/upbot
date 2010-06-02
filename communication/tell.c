/**
 * tell.c
 *
 * This set of routines allows a parent and child to synchronize.
 * Based on "Advanced Programming in the Unix Environment" by
 * Stevens, program 10.17. 
 */


#include "tell.h"

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

/** 
 * sig_usr(int signo)
 *
 */
static void sig_usr(int signo)
{
  sigflag = 1;
  return;
}

/** 
 * TELL_WAIT(void)
 *
 */
void TELL_WAIT(void)
{
  if(signal(SIGUSR1, sig_usr) == SIG_ERR)
    perror("signal(SIGUSR1) error");
  if(signal(SIGUSR2, sig_usr) == SIG_ERR)
    perror("signal(SIGUSR2) error");
  
  sigemptyset(&zeromask);

  sigemptyset(&newmask);

  sigaddset(&newmask, SIGUSR1);
  sigaddset(&newmask, SIGUSR2);

  // Block SIGUSR1 and SIGUSR2 and save current signal mask 
  if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    perror("SIG_BLOCK error");
}

/**
 * TELL_PARENT(pid_t pid)
 *
 */
void TELL_PARENT(pid_t pid)
{
  // Tell the parent we're done.
  kill(pid, SIGUSR2);
}

/**
 * WAIT_PARENT(void)
 *
 */
void WAIT_PARENT(void)
{
  while(sigflag == 0)
    {
      // Wait for parent
      sigsuspend(&zeromask);
    }

  sigflag = 0;

  // Reset signal mask to original value
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    perror("SIG_SETMASK error");

}

/**
 * TELL_CHILD(pid_t pid)
 *
 */
void TELL_CHILD(pid_t pid)
{
  // Tell the child we're done
  kill(pid, SIGUSR1);
}

/**
 * WAIT_CHILD(void)
 * 
 */
void WAIT_CHILD(void)
{
  while(sigflag == 0)
    {
      // Wait for child
      sigsuspend(&zeromask); 
    }
  sigflag = 0;
  
  if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    perror("SIG_SETMASK error");

}
