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
    
    
    
    //called 0x
    printf("Calling saccReceiveState with 0101101100\n");
    printf("Expected result is: \n0000011101\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    //called 1x
    saccReceiveAction(0x7);
    printf("Saccades called once\n");
    temp2 = saccReceiveState(temp);
    printf("Calling saccReceiveState with 0101101100\n");
    printf("Expected result is: \n0000010101\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    
    printf("set goalbit\n");
    (*temp) = '1';
    
    //called 2x
    saccReceiveAction(0x7);
    printf("Saccades called twice\n");
    temp2 = saccReceiveState(temp);
    printf("Calling saccReceiveState with 1101101100\n");
    printf("Expected result is: \n1000001100\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    //called 3x
    saccReceiveAction(0x7);
    printf("Saccades called thrice\n");
    temp2 = saccReceiveState(temp);
    printf("Calling saccReceiveState with 1101101100\n");
    printf("Expected result is: \n1000011101\nResult is:\n");
    for(i=0; i < 10; i++)
    {
        printf("%c",temp2[i]);
    }
    printf("\n");
    
    return 0;
}
