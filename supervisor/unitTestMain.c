#include <stdio.h>
#include "unitTest.h"
#include "supervisor.h"

int main(void)
{
    initSupervisor();
    replanTest();
    endSupervisor();
	return 0;
}
