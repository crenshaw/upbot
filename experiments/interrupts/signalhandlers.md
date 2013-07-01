Signal Handlers
===============
*   Continues description of signals
    *   How to design a signal handler
        *   Discuss topics of reentrancy and async-signal-safe functions
    *   Alternatives to performin a normal return from a signal handler
        *   Use of nonlocal goto
    *   Handling of signals on an alternate stack
    *   Use of _sigaction()_ SA\_SIGINFO flag to allow a signal handler to obtain detailed informaiton about the signal that caused its invocation
    *   How a blocking system call may be interrupted by a signal handler and how the call be restarted if desired

Designing Signal Handlers
-------------------------
*   Preferable to write simple signal handlers
    *   Reduce risk of creating race conditions
*   Two common designs
    1.  Signal handler sets a global flag and exits
        *   "main" periodically checks this flag and, if set, takes appropriate action
        *   Also possible to have sig handler write a single byte to a dedicated pipe whose read end is included among checked file descriptors
    2.  Signal handler performs cleanup and either terminates process or uses nonlocal goto to unwind the stack

### Signals Are Not Queued
*   Delivery of a signal is blocked during the execution of its handler (unless we specify SA\_NODEFER flag to _sigaction()_)
*   Signals are not queued, if it is generated more than once then it is still marked as pending
*   No way to reliably count the number of times a signal is generated

### Reentrant and Async-Signal-Safe Functions
#### Reentrant and nonreentrant functions
*   Must first distinguish between single-threaded and multithreaded programs
*   Classical UNIX programs have a single _thread of execution_
*   Because a signal handler may asynchronously interrupt execution, main and signal handler in effect form two threads (not concurrent)
*   Function is said to be _reentrant_ if it can safely be simultaneously executed by multiple threads of execution in the same process
    *   "Safe" means function achieved expected result, regardless of state of execution of any other thread of execution
*   Function is said to be _nonreentrant_ if it updates global or static data structures
    *   Can also be nonreentrant if they use static data structures for their internal bookkeeping
        *   Examples: functions from stdio

