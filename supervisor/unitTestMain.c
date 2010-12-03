#include <stdio.h>
#include "unitTest.h"
#include "supervisor.h"

int main(void)
{
    initSupervisor();
    planTest();
    endSupervisor();
	return 0;
}
