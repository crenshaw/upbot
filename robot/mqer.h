#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

void getSensorData(char* sensDataFromRobot);
