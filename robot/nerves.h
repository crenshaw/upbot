#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include "eventresponder.h"
#include "roomba/roomba.h"

#include "events.h"
#include "responders.h"

#include "clock.h"
#include "erQueue.h"
#include "erControl.h"
#include "myEventResponders.h"

#include "netDataProtocol.h"

#include "../communication/connector.h"


//#include "../roomba/roomba.h"

#ifndef _NERVES_H_
#define _NERVES_H_

void getSensorData(char* sensDataFromRobot);

#endif
