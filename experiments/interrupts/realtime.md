Realtime Signals
================
*   Realtime signals defined in POSIX.1b to rememby limitations of standard signals
*   Advantages over standard signals
    *   Provide increased range of signals
    *   Queued. If multiple instances of a realtime signal are sent to a process, then the signal is delivered mlutiple times
    *   Possible to specify data (int or ptr) that accompanies the signal
        * Can be handled by signal handler
    *   Order of delivery of different realtime signals is guaranteed
*   There are limits to queues, since they take up kernel memory space
    * Determine queue size in Linux: `cat /proc/sys/kernel/rtsig-max`
    * Default is 1024
    * Determine current queue size in Linux: `cat /proc/sys/kernel/rtsig-nr`
    * Changed after 2.6.8 and /proc references removed
        * Now contained in RLIMIT_SIGPENDING soft resource

Using Realtime Signals
----------------------
*   Requirements
    *   Sending process sends the signal plus its accompanyin data using _sigqueue()_ system call
        *   Can also be sent using _kill()_ and _raise()_
            * Not very portable outside of linux since SUSv3 leaves queueing through these calls to be implementation specific
    *   Receiving process establishes a handler for the signal using a call to _sigaction()_ that specifies SA\_SIGINFO flag
        * Allows for data to be passed through to the handler

Sending Realtime Signals
------------------------
*   _sigqueue()_ system call sends the realtime signal specified by _sig_ to the process specified by _pid_

```C
    #include <signal.h>

    int sigqueue(pid_t pid, int sig, const union sigval value);

    union sigval {
        int sival_int;   // int value for accompanying data
        void *sival_ptr; // ptr value for accompanying data
    }
```

*   The same permissons are required for _sigqueue()_ as _kill()_
