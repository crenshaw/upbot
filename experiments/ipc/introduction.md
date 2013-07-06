Interprocess Communication Overview
===================================

A Taxonomy of IPC Facilities
----------------------------
* _Communication:_ concerned with exchanging data between processes
    * data transfer
        * byte stream
            * pipe
            * FIFO
            * stream socket
        * pseudoterminal
        * message
            * System V message queue
            * POSIX message queue
            * datagram socket
    * shared memory
        * System V shared memory
        * POSIX shared memory
        * memory mapping
            * anonymous mapping
            * mapped file
* _Synchronization:_ concerned with synchronizing the actions of processes or threads
    * semaphore
        * System V semaphore
        * POSIX semaphore
            * named
            * unnamed
        * file lock
            * "record" lock (_fcntl()_)
            * file lock (_flock()_)
        * mutex (threads)
        * condition variables (threads)
* _Signals:_ can be used as a synchronization technique or communication technique (using realtime signals)
    * standard signal
    * realtime signal

* IPC facilities are often similar
    * Similar facilities evolved on different UNIX variants
    * New facilities developed to address design deficiencies

Communication Facilities
------------------------
* Can be broken into two categories
    1. _Data-transfer facilities:_ Notion of writing and reading
        * One process writing data to IPC facility, one process reading data
        * Require two data transfers between user memory and kernel memory
    2. _Shared memory:_ Allows processes to exchange information

### Data Transfer
* Can brake up data-transfer facilities into subcategories
    * _Byte stream:_ data exchanged via pipes, FIFOs, and datagram sockets
        * Follows UNIX "file as a sequence of bytes" model"
    * Message: data exchanged via System V message queues, POSIX message queues, and datagram sockets
        * Each read operation reads as an entire message
        * Not possible to read multiple messages in single read operation
    * _Pseudoterminals:_ communication facility intended ofr use in special situations

#### Diferences from shared memory
* Data-transfer reads are destructive
    * Read operation consumes data, which is then not consumable by another process
        * MSG_PEEK flag used to perform a nondestructive read from a socket
        * UDP allows for multicast to multiple recipients
* Synchronization between reader and writer is automatic
    * By nature will automatically block

### Shared Memory
* Three main flavors of shared memory
    1. System V shared memory
    2. POSIX shared memory
    3. Memory mappings
* Provides fast communication by requires copious amounts of synchronization
    * Synchronization usually achieved through semaphores
* Data placed in shared memory is visible to all of the processes that share that memory

Synchronization Facilities
--------------------------
* Synchronization allows processes to avoid updating share memory at the same time
* UNIX systems provide the follow facilities
    * _Semaphores:_ kernel maintained int whos value is never permitted to fall below 0
        * Process can increase/decrease value of semaphore
        * Kernel blocks operation until semaphore value is appropriate (developer defined)
            * Alternatively, can be defined to behave in non-blocking manner and report error instead
        * Meaning is determined by application
    * _File locks:_ explicitly designed to coordinate actions of multiple processes operating on the same file
        * Can also be used to coordinate access to other shared resources
        * Two flavors:
            1. read (shared) locks
                * Can be held by any number of processes
            2. write (exclusive) locks
                * Only one process has access to read/write to file
        * _fnctl()_ system call most commonly used
    * _Mutexes and condition variables:_ normally used with POSIX threads
* Choice of facility depends on functional requirements
* Linux (as of 2.6.22) allows an additional, nonstandard synchronization mechanism via _eventfd()_ system call
    * Creates _eventfd_ object that has associated 8-byte unsigned integer maintained by the kernel
    * syscall returns a file descriptor that refers to object
    * Writing an int to this FD adds that integer to the object's value
    * _read()_ blocks if object's value is 0
    * _poll()_, _select()_, or _epoll_ can test if object has nonzero value
    * @see _eventfd(2)_

### TODO
[ ] Comparing IPC Facilities
