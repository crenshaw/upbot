Daemon Demo
===========

Quickstart
----------
1. `make all`
2. `sudo ./tbd.out 9191`
3. `./tc.out localhost 9191`

Overview
--------
### become\_daemon.h
A simple header file to define a few constants that we will need.

### become\_daemon.c
This is a stripped down version of the _becomeDaemon()_ function as defined in Kerrisk's Linux Programming Interface.
The code code contained within this function is the bare minimum required to daemonize a process. We begin by
backgrounding the current process and removing ourselves from the current session group. We will then fork again
to ensure that we are not a session leader. Finally we will ensure that our daemon has the proper permissions and
that it is operating in a stable location in the filesystem (i.e. not in a /tmp folder). Finally, we will close all file
descriptors and ensure that any previous file descriptors are now pointing to /dev/null (if we need to open any FDs we can
do so after we have daemonized).

### test\_become\_daemon.c
We begin by checking arguments and daemonizing. Next we define a struct that handles TCP/IP information and create a socket.
Next we bind the our struct and configuration options together[1]. If this step fails we will exit, otherwise we will attempt
to listen on this bound socket (and exit if failing). Finally, we will await connections on this socket from any port. Once we
receive an connection request we will pass the connection function into POSIX thread and await a new connection.

Our connection function is in charge of logging messages to syslog. It will accept messages from the client and record them
faithfully in the system logger using the syslog(3) library call. Depending on your system and configuration, the messages will
appear in /var/log/syslog.

[1] Note that if this step fails, we will log to the system logger (syslog). The first function variable `LOG_DAEMON | LOG_DEBUG`
the means by which syslog will categorize the message. `LOG_DAEMON` is known as a _facility_ (determines what type
of application is attempting to log something) which is then ORed with `LOG_DEBUG`, which is known as a _level_ (determines the
severity of the request). A complete list of facilities and levels can be found in the syslog(3) manpage.

Usage
-----
1. First begin by compiling the sources into executable binaries. This can be done with a simple `make`.
2. Then, as a priveleged user, run the server with the desired port: `sudo ./tbd.out 9191`.
    * The server should now be listening for connections on port 9191
3. Start the client with the port defined above: `./tc.out localhost 9191`
    * You should now see a prompt ("Send>"), at this point you may begin sending messages to the server.
4. When finished, simply send a Ctrl-C to the process to terminate`
QUESTION::::5. What should one expect to see in /var/log/system.log?


Next Steps
----------
1. This sample daemon currently does not read configuration options from the command line or a file
2. This sample daemon currently does not handle the SIGHUP signal to reload its configuration and environment
