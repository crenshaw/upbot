#include "roomba.h"

void song()
{
  char song[13] = {CmdSong, 0, 4, 60, 16, 60, 8, 60, 8, 72, 32, CmdPlay, 0}; 
  
  int i;
  for(i = 0; i < 13; i++)
    {
      byteTx(song[i]);
    }
}
  
