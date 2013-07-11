/**
 * respondStop
 * 
 * Default responder function.  Stop the robot.
 */ 
void respondStop(void)
{
  // TODO: Add code to stop the robot.
  printf("Reminder: add code to actually stop the robot\n");

  return;
}

void respondDriveLow(void) {
   printf("Drive low\n");
   driveStraightUntil(1,LOW);
}

void respondDriveMed(void) {
   printf("Drive Med\n");
   driveStraightUntil(1,MED);
}

void respondDriveHigh(void) { 
      driveStraightUntil(1,HIGH);
}

/**
 * respondOne
 *
 * Example responder function, as written by an application developer.
 */
void respondOne(void)
{
  printf("      Got a 1!\n");

  return;
}

/**
 * respondTwo
 *
 * Example responder function, as written by an application developer. 
 */
void respondTwo(void)
{
  printf("      Got a 2!\n");

  return;
}

/**
 * respondTurn
 *
 * Turns the robot a random amount
 */
void respondTurn(void) {
    //turnClockwise(90);
    turnRandom(300000,1800000);
}

