//move.c
//Description:
//		Provides functions which carry out Roomba move commands
//Authors:
//		Steven M. Beyer, Tanya L. Crenshaw
//Date Created:
//		17 February 2010

#include "roomba.h"


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
  sleep(1);

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
  sleep(sec);
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
  byteTx(128);
  byteTx(0);
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
  sleep(degrees/90);
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
  sleep(degrees/90);
}

//drive backwards for given seconds
//speed given by LOW, MED, HIGH
void driveBackwardsUntil(int sec, int speed)
{
  driveBackwards(speed);
  sleep(sec);
}

void driveBackwards(int speed)
{
  byteTx(CmdDrive);
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
    }

  byteTx(128);
  byteTx(0);
}

void stop()
{
  byteTx(CmdDrive);
  byteTx(0);
  byteTx(0);
  byteTx(0);
  byteTx(0);
}
