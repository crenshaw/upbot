/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

#include "become_daemon.h"

int becomeDaemon() {
    int maxfd, fd;

    switch (fork()) {                       /* Become background process */
        case -1: return -1;
        case 0:  break;                     /* Child falls through... */
        default: exit(EXIT_SUCCESS);       /* while parent terminates */
    }

    if (setsid() == -1)                 /* Become leader of new session */
        return -1;

    switch (fork()) {                   /* Ensure we are not session leader */
        case -1: return -1;
        case 0:  break;
        default: exit(EXIT_SUCCESS);
    }

    umask(0);                       /* Clear file mode creation mask */
    chdir("/");                     /* Change to root directory */

    /* Ask the system for the maximum number of files that a process
     * can have open at any time.
     */
    maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)                /* Limit is indeterminate... */
        maxfd = BD_MAX_CLOSE;       /* so take a guess */
    for (fd = 0; fd < maxfd; fd++)
        close(fd);

    close(STDIN_FILENO);            /* Reopen standard fd's to /dev/null */

    fd = open("/dev/null", O_RDWR);

    if (fd != STDIN_FILENO)         /* 'fd' should be 0 */
        return -1;
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        return -1;

    return 0;
}
