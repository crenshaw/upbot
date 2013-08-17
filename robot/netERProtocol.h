#include <stdlib.h>
#include <string.h>

#include "eventresponder.h"
#include "myEventResponders.h"

#ifndef _NET_ER_PROTOCOL_H_
#define _NET_ER_PROTOCOL_H_

#define INT_SIZE (4)
#define ERPRO_PACKAGE_IDENTIFIER 33

int packageEventResponder(eventResponder* er, char** package);
void unpackageEventResponder(int size,eventResponder* er, char* package);

#endif
