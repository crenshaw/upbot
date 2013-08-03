//led.c
//Description:
//		Provides functions which carry out Roomba LED commands
//Author:
//		Steven M. Beyer
//Date Created:
//		18 February 2010

//setLED
//Description:
//		sets LEDs according to parameters
//Parameters:
//		powerSetting: if 0 power is off; if 1 power is Red; if 2 power is Green
//		playSetting: if 0 play is off; if 1 play is on
//		advanceSetting: if 0 advance is off; if 1 advance is on
void setLED(int powerSetting, int playSetting, int advanceSetting)
{
  byteTx(CmdLeds);
  if(playSetting == PLAY_ON && advanceSetting == ADVANCE_ON)
    byteTx(SET_ALL);
  else if(playSetting == PLAY_OFF && advanceSetting == ADVANCE_ON)
    byteTx(SET_ADVANCE);
  else if(playSetting == PLAY_ON && advanceSetting == ADVANCE_OFF)
    byteTx(SET_PLAY);
  else
    byteTx(OFF);
  if (powerSetting == RED)	
    byteTx(PWR_RED);
  else
    byteTx(PWR_GREEN);
  if (powerSetting == OFF)
    byteTx(OFF);
  else
    byteTx(FULL_INTENSITY);
}

void blinkLED()
{
  int i = 0;

  for(i = 0; i < 2; i++)
    {

      byteTx(CmdLeds);
      byteTx(OFF);
      byteTx(PWR_GREEN);
      byteTx(OFF);

      usleep(HALF_SECOND);

      byteTx(CmdLeds);
      byteTx(SET_ALL);
      byteTx(PWR_RED);
      byteTx(FULL_INTENSITY);

      usleep(HALF_SECOND);
      
    }
  byteTx(CmdLeds);
  byteTx(OFF);
  byteTx(PWR_GREEN);
  byteTx(OFF);
}

