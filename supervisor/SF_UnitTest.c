#include <stdlib.h>
#include <stdio.h>
#include "saccFilt.h"
int main(int argc, char* argv[])
{
    char * temp = (char*)malloc(10 * sizeof(char));
    temp[0]='0';
    temp[1]='1';
    temp[2]='0';
    temp[3]='1';
    temp[4]='1';
    temp[5]='0';
    temp[6]='1';
    temp[7]='1';
    temp[8]='0';
    temp[9]='0';
    char * temp2 = (char*)malloc(10 * sizeof(char));
    temp2 = saccReceiveState(temp);
    int i;
    printf("Calling saccReceiveState with 0101101100\n");
    printf("Expected result is: \n0000000010\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    saccReceiveAction(0x9);
    printf("Saccades called once\n");
    
    temp2 = saccReceiveState(temp);
    printf("Calling saccReceiveState with 0101101100\n");
    printf("Expected result is: \n0000001110\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    saccReceiveAction(0x9);
    saccReceiveAction(0x9);
    printf("Saccades called three times\n");
    temp2 = saccReceiveState(temp);
    printf("Calling saccReceiveState with 0101101100\n");
    printf("Expected result is: \n0000011000\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    saccReceiveAction(0x9);
    
    
    
    
    
    free(temp);
    free(temp2);
    
}
