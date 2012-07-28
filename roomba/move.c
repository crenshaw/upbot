//move.c
//Description:
//		Provides functions which carry out Roomba move commands
//Authors:
//		Steven M. Beyer, Tanya L. Crenshaw
//Date Created:
//		17 February 2010

#include "roomba.h"
#define SIZE 40

/**
 *  driveStraightWithFeedback()
 *
 *   This function issues a drive command to the
 *   iRobot.  It then queries the iRobot for the 
 *   "velocity most recently requested with a Drive
 *   command (see packet ID 39)."  If the issued
 *   velocity and the queried velocity match, the function
 *   returns 0.  Otherwise, it returns -1.
 */
int driveStraightWithFeedback(int velocity)
{

  char x[10] ={0};
  unsigned int velocityWord = 0;

  driveStraight(velocity);

  // Get the packet that contains the last issued velocity value.
  receiveSensorData(SP_REQUESTED_VELOCITY, x, 2, 1);
  //sleep(1);

  // First, determine what the expected velocity is; velocity
  // is described by a high byte, then low byte.  Get the word.
  if (velocity == HIGH)
    {
      velocityWord = HIGH_SPEED;
    }

  else if (velocity == MED)
    {
      velocityWord = MED_SPEED;
    }
  else if (velocity == LOW)
    {
      velocityWord = LOW_SPEED;
    }


  // Confirm that the expected high byte and low byte match
  // those reported by the iRobot.
  if ((x[HIGH_BYTE] == (velocityWord >> 8)) && 
      (x[LOW_BYTE] == (velocityWord & 0x00FF)))
    {      
      return 0;      
    }

  return -1;
  
}

/**
 *  driveStraightUntil()
 *
 *  Issue a drive forward command to the iRobot for given number of
 *  microseconds.  Velocity given by LOW, MED, or HIGH
 */
void driveStraightUntil(int usec, int speed)
{
  driveStraight(speed);
  usleep(usec);
}

/**
 * driveStraight()
 * 
 *  Issue a drive straight command to the iRobot for the given
 *  velocity. Velocity given by LOW, MED, or HIGH.
 *
 */ 
void driveStraight(int velocity)
{
  
  unsigned int velocityWord = 0;

  // Based on the input parameter, get the two-byte value
  // that describes the intended velocity for the iRobot.
  if (velocity == HIGH)
    {
      velocityWord = HIGH_SPEED;
    }
  else if (velocity == MED)
    {
      velocityWord = MED_SPEED;
    }
  else if (velocity == LOW)
    {
      velocityWord = LOW_SPEED;
    }

  // Send the 5-byte drive command to the iRobot:

  // Begin by issuing a drive command to the iRobot.
  byteTx(RCMD_DRIVE);

  // Send the high byte, then the low byte of
  // the desired velocity value.
  byteTx(velocityWord >> 8);
  byteTx(velocityWord & 0x00FF);

  // Send the high byte, then the low byte of
  // the desired turn-radius value.
  byteTx(STRAIGHT >> 8);
  byteTx(STRAIGHT & 0x00FF);
}

/**
 * turnCounterClockwise()
 *
 * Turn counter clockwise in increments of 90 degrees (i.e. 90, 180,
 * 270...)
 *
 */
void turnCounterClockwise(int degrees)
{
  byteTx(RCMD_DRIVE);
  byteTx(0);
  byteTx(200);
  byteTx(0);
  byteTx(1);
  usleep(degrees);
  stop();
}

/**
 * turnClockwise()
 * 
 * Turn clockwise in increments of 90 degrees (i.e. 90, 180, 270...)
 */
void turnClockwise(int degrees)
{
  byteTx(RCMD_DRIVE);
  byteTx(0);
  byteTx(200);
  byteTx(255);
  byteTx(255);
  usleep(degrees);
  stop();
}

/**
 * turn()
 * 
 * A comprehensive function which turns the iRobot left or right
 * in-place according to the the number of degress specified.  The
 * on-board Devantech Electronic compass is used to get the number of
 * degrees travelled.  The compass must be available for this function
 * to be usable; otherwise, use the turnClockwise() and
 * turnCounterClockwise() functions.
 *
 * @arg direction 0 for left, 1 for right.
 * @arg degrees number of degrees to turn.
 *
 * @returns void
 *
 */
void turn(int direction, int degrees)
{


}


/**
 * driveBackwardsUntil()
 *
 * Drive backwards for given microseconds speed given by LOW, MED, HIGH
 
 */
void driveBackwardsUntil(int usec, int speed)
{
  driveBackwards(speed);
  usleep(usec);
  stop();
}

/**
 * driveBackwards()
 *
 * Issue the drive backwards command to the iRobot.
 *
 */
void driveBackwards(int velocity)
{
  unsigned int velocityWord = 0;
  byteTx(RCMD_DRIVE);
  if (velocity == HIGH)
    {
      velocityWord = HIGH_SPEED_BACK;
    }
  
  else if (velocity == MED)
    {
      velocityWord = MED_SPEED_BACK;
    }
  else if (velocity == LOW)
    {
      velocityWord = LOW_SPEED_BACK;
    }

  byteTx(velocityWord >> 8);
  byteTx(velocityWord & 0x00FF);

  byteTx(STRAIGHT >> 8);
  byteTx(STRAIGHT & 0x00FF);
}

void stop()
{
  byteTx(RCMD_DRIVE);
  byteTx(0x00);
  byteTx(0x00);
  byteTx(STRAIGHT & 0x00FF);
  byteTx(STRAIGHT & 0x0FF);
}

/** 
 * driveDistance()
 *
 * Tries to drive a given distance while checking sensor data and
 * distance data.
 *
 * @arg distanceRequested distance to drive
 *
 * @return 0 if it has driven the given distance and -1 if it did not.
 *
 */ 
int driveDistance()
{
  
  int distanceTraveled = 0;
  char sensDataFromRobot[SIZE] = {'\0'};
  int sumDistanceTraveled = 0;

  driveStraight(MED);


  //while there is nothing read from sensor and has not traveled given distance
  while(!checkSensorData(sensDataFromRobot) && 
	(distanceTraveled <= TILE_DISTANCE))
    {
      //receive bump, wheeldrop, and cliff data
      receiveGroupOneSensorData(sensDataFromRobot);

      


      // One of the sensors may have been activated.  Check if so, and
      // react by stopping.  Do this now instead of waiting until after
      // the distance traveled has been polled for better reaction time.
      checkSensorData(sensDataFromRobot);

      usleep(15000);

      //update distance traveled data
      distanceTraveled ++;
    }

  //if there was a bump, wheeldrop, or cliff before distance then return
  if(distanceTraveled < TILE_DISTANCE)
    {
      return -1;
    }
  //else successfully completed command within given distance
  STOP_MACRO;
  
  return 0;

}

void adjustRIGHT()
{
  turnClockwise(ADJUST_AMOUNT);
}

void adjustLEFT()
{
  turnCounterClockwise(ADJUST_AMOUNT);
}
