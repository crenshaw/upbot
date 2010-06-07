//move.c
//Description:
//		Provides functions which carry out Roomba move commands
//Authors:
//		Steven M. Beyer, Tanya L. Crenshaw
//Date Created:
//		17 February 2010

#include "roomba.h"
#define SIZE 40

// driveStraightWithFeedback()
//   This function issues a drive command to the
//   iRobot.  It then queries the iRobot for the 
//   "velocity most recently requested with a Drive
//   command (see packet ID 39)."  If the issued
//   velocity and the queried velocity match, the function
//   returns 0.  Otherwise, it returns -1.
//
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

// driveStraightUntil()
//   issue a drive forward command to the iRobot 
//   for given number of seconds.  Velocity given by LOW, 
//   MED, or HIGH
void driveStraightUntil(int sec, int speed)
{
  driveStraight(speed);
  usleep(sec);
}

// driveStraight()
//  issue a drive straight to the iRobot for 
//  the given velocity. Velocity given by LOW, 
//  MED, or HIGH
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
  byteTx(CmdDrive);

  // Send the high byte, then the low byte of
  // the desired velocity value.
  byteTx(velocityWord >> 8);
  byteTx(velocityWord & 0x00FF);

  // Send the high byte, then the low byte of
  // the desired turn-radius value.
  byteTx(STRAIGHT >> 8);
  byteTx(STRAIGHT & 0x00FF);
}

//turn counter clockwise in increments of 
//90 degrees (i.e. 90, 180, 270...)
void turnCounterClockwise(int degrees)
{
  byteTx(CmdDrive);
  byteTx(0);
  byteTx(200);
  byteTx(0);
  byteTx(1);
  usleep(degrees);
  stop();
}

//turn clockwise in increments of 
//90 degrees (i.e. 90, 180, 270...)
void turnClockwise(int degrees)
{
  byteTx(CmdDrive);
  byteTx(0);
  byteTx(200);
  byteTx(255);
  byteTx(255);
  usleep(degrees);
  stop();
}

//drive backwards for given seconds
//speed given by LOW, MED, HIGH
void driveBackwardsUntil(int sec, int speed)
{
  driveBackwards(speed);
  usleep(sec);
  stop();
}

void driveBackwards(int velocity)
{
  unsigned int velocityWord = 0;
  byteTx(CmdDrive);
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
  /*
    if (speed == HIGH)
    {
    byteTx(0xFE);
    byteTx(0x0C);
    }
    else if (speed == MED)
    {
    
    byteTx(254);
    byteTx(212);
    }
    else if (speed == LOW)
    {
    byteTx(255);
    byteTx(156);
    }*/
  byteTx(velocityWord >> 8);
  byteTx(velocityWord & 0x00FF);

  byteTx(STRAIGHT >> 8);
  byteTx(STRAIGHT & 0x00FF);
}

void stop()
{
  byteTx(CmdDrive);
  byteTx(0xFF);
  byteTx(0xFF);
  byteTx(0xFF);
  byteTx(0xFF);
}

/** driveDistance()
 *
 * Tries to drive a given distance while checking sensor data and
 * distance data.
 *
 * @arg distanceRequested distance to drive
 *
 * @return 0 if it has driven the given distance and -1 if it did not.
 */ 
int driveDistance()
{
  printf("%s %d\n", __FILE__, __LINE__);
  char distanceTraveled[2] = {'\0'};
  char sensDataFromRobot[SIZE] = {'\0'};
  int sumDistanceTraveled = 0;

  //packet 19 checks distance traveled since last time requested
  //so zero out register
  receiveSensorData(19, distanceTraveled, 2, 1);
  driveStraight(MED);


  //while there is nothing read from sensor and has not traveled given distance
  while(!checkSensorData(sensDataFromRobot) && 
	(sumDistanceTraveled <= TILE_DISTANCE))
    {
      //receive bump, wheeldrop, and cliff data
      receiveGroupOneSensorData(sensDataFromRobot);

      // One of the sensors may have been activated.  Check if so, and
      // react by stopping.  Do this now instead of waiting until after
      // the distance traveled has been polled for better reaction time.
      checkSensorData(sensDataFromRobot);
 
      //receive distance traveled data
      receiveSensorData(19, distanceTraveled, 1, 2);
      //convert 2 byte char to int and store into sumDistanceTraveled
      sumDistanceTraveled += ((distanceTraveled[0]<<8) | distanceTraveled[1]);
    }

  //if there was a bump, wheeldrop, or cliff before distance then return
  if(sumDistanceTraveled < TILE_DISTANCE)
    {
      return -1;
    }
  //else successfully completed command within given distance
  stop();
  return 0;

}
