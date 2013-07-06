Daemons
=======
Overview
--------
A _daemon_ has the following characteristics:
* Long lived, often created at startup and runs until system shutdown
* Runs in the background with no controlling terminal (immune to job control signals)
* Written to carry out specific tasks
    * _cron_: task scheduler
    * _sshd_: "Secure Shell Daemon", used for managing remote connections to the host
    * _httpd_: "HTTP Server Daemon", aka Apache Web Server


* Daemons often run as a privileged process
    * Proper care should be taken to ensure the security of the daemon

Creating a Daemon
-----------------
1. Perform a _fork()_ (parent exits and child continues)
    * Allows child to continue in the background
    * Ensures that child is not a process group leader
2. Child process calls _setsid()_ to start a new session
3. If the daemon might later open a terminal we must ensure that it does not become a controlling terminal
    * Specify 0\_NOCTTY flag on any _open()_ that may apply to a terminal device
    * Or, simply perform a second _fork()_ after _setsid()_ has been called
4. Clear the process umask to ensure proper permissions on created files and directories
5. Change the process' cwd, typically to /
    * We simply want to make sure that the daemon's cwd is not in a tmp directoy
6. Close all open file descriptors (FDs) that are inherited from the parent
7. Generally, open /dev/null and use _dup2()_ to make FDs closed in previous step point to this device
    * Ensures that library function calls do not fail in unexpected ways (expecting a non-existant FD)

Using `SIGHUP` to Reinitialize a Daemon
---------------------------------------
* Typically a daemon reads operational params from a conf file on startup
    * Sometimes we want to change this on the fly
* Some daemons produce log files
    * If log file is never closed, then it may grow endlessley
* Solution to these problems is to have the daemon establish a handler for SIGHUP
    * Since daemons have no controlling terminal, SIGHUP may be used safely for reinitialization
* _logrotate_ sends SIGHUP to daemons in order to rotate log files and prevent endless growth

Logging Messages and Errors Using _syslog_
------------------------------------------
### Overview
* Syslog provides a central space for logging
    * Two key components: _syslogd_ and _syslog_
* _syslogd_ accepts log messages from /dev/log (UNIX socket), and TCP streams
* Each message processed by _syslogd_ has a number of attributes
    * _facility_: specifies the type of program generating the message
    * _level_: specifies the severity of the message
* syslog parses and sends this message to the appropriate destination based upon a conf file
* _syslog_ library functions can be used to log a message
* _syslog_ consists of three main functions
    * _openlog()_: establishes default settings that apply to subsequent calls [OPTIONAL]
    * _syslog()_: logs a message
    * _closelog()_: called after finished logging messages
* None of the above three functions return a value


