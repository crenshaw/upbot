#include <stdio.h>
#include "unitTest.h"
#include "supervisor.h"

int main(void)
{
    initSupervisor();
	simpleTest();
    endSupervisor();
	return 0;
}
