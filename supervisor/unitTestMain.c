#include <stdio.h>
#include "unitTest.h"
#include "supervisor.h"

int main(void)
{
    initSupervisor();
	printf("First run\n");
	simpleTest();
	printf("Second run\n");
	simpleTest();
	printf("Third run\n");
	simpleTest();
    endSupervisor();
	return 0;
}
