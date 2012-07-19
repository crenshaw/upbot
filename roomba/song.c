/**
 * song.c
 *
 * A collection of functions that use the SONG the PLAY commands on
 * the iRobot Create.
 *
 * @author Steven M. Beyer, Tanya L. Crenshaw
 * @since  24 February 2010
 * 
 *
 */
#include "roomba.h"

#define SONG_NUMBER 0
#define SONG_LENGTH 4

/**
 * song()
 * 
 * Use the SONG command to specify a 4-note "song #0" and use the PLAY
 * command to immediately play it.  It is only necessary to specify
 * the set of notes once.  Subsequent PLAY commands to play "song #0"
 * will play the same song. 
 * 
 * The song is "dum da da dum!", the song played in the 1980s
 * Softspread Imperial Margine commercials at the moment the crown
 * suddenly appears after the actor tastes the vile product.
 * 
 * @return void
 * 
 */
void song()
{
  /* Create an array of commands and data bytes.  Data bytes 3, 5, 7
   * and 9 indicate note pitch while data bytes 4, 6, 8, and 10
   * indicate note length.
   */
  char song[13] = {RCMD_SONG, SONG_NUMBER, SONG_LENGTH, 60, 16, 60, 8, 60, 8, 72, 32, RCMD_PLAY, SONG_NUMBER}; 
  
  int i;
  for(i = 0; i < 13; i++)
    {
      byteTx(song[i]);
    }
}

/**
 * play()
 * 
 * Issue the PLAY command to the iRobot to play the song number
 * specified in the parameter.  
 * 
 * From the Open Interface specification, "The PLAY command does not
 * work if a song is already playing.  Note that the 'song playing'
 * sensor packet can be used to check whether the iRobot Create is
 * ready to accept this command."
 *
 * @param An single byte, 0-15, to indicate which song should be played.
 * @return 0 if a songNumber within 0-15 was passed into the function,
 * -1 otherwise.
 *
 */
int play(char songNumber)
{

  if (songNumber >= 0 && songNumber <= 15)
    {
      byteTx(RCMD_PLAY);
      byteTx(songNumber);
      return 0;
    }

  return -1;
}  
