#include <stdio.h>
#include "unitTest.h"

int main(void)
{
	int cmds[6] = {1,1,3,1,4,1};
	char* str;
	int i;
	for(i=0; i < 6; i++)
	{
		str = unitTest2(cmds[i]);

		printf("Received Sensor Data: %s\n", str);
	}

	return 0;
}
