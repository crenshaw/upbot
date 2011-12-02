#include <stdlib.h>
#include <stdio.h>
#include "saccFilt.h"


int main(int argc, char* argv[])
{
    char * sensorData = (char*)malloc(11 * sizeof(char));
    sensorData[0]='x';
    sensorData[1]='A';
    sensorData[2]='B';
    sensorData[3]='C';
    sensorData[4]='D';
    sensorData[5]='E';
    sensorData[6]='F';
    sensorData[7]='G';
    sensorData[8]='H';
    sensorData[9]='I';
    sensorData[10]='\0';
    int i;
    
    
    
    //called 0x
    printf("Called Saccades 0x\n");
    
    printf("Calling saccReceiveState with: %s\n", sensorData);
    char* temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 1x
    printf("Called Saccades 1x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    printf("set goalbit\n");
    sensorData[0] = 'X';
    
    printf("\n");
    
    //called 2x
    printf("Called Saccades 2x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 3x
    printf("Called Saccades 3x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 4x
    printf("Called Saccades 4x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 5x
    printf("Called Saccades 5x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 6x
    printf("Called Saccades 6x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 7x
    printf("Called Saccades 7x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 8x
    printf("Called Saccades 8x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 9x
    printf("Called Saccades 9x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 10x
    printf("Called Saccades 10x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    printf("\n");
    
    //called 11x
    printf("Called Saccades 11x\n");
    saccReceiveAction(0x7);
    printf("Calling saccReceiveState with: %s\n", sensorData);
    temp = saccReceiveState(sensorData);
    printf("Result is: %s\n", temp);
    
    return 0;
}
