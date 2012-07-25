/**
 * compass.c
 *
 * Device driver created for the Devantech Electronic Compass.  This
 * driver allows one to initialize the compass hardware using the
 * initializeCompass() function and then subsequently obtain the
 * compass heading using getHeading(). The initialization function
 * need only be invoked once while getHeading() may be invoked as
 * often as allowed by the I2C bus.  When compass use is complete,
 * the device may be closed using closeCompass().
 *
 * @author Cole Mercer and Tanya L. Crenshaw
 * @since  21 July 2010
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


/* Intended to allow the reading of I2C packets to arbitrary addresses */

int file;
char FILENAME[20] = "/dev/i2c-0";

/**
 * initializeCompass()
 * 
 * Initialize the Devantech Electronic Compass by opening its associated
 * I2C port on the gumstix motherboard.
 *
 * @return int indicating if the operation was successful or not.
 * 0 is success, -1 failure.
 *
 * TODO: REMOVE "exit(1)" from this function!!
 * TODO: REMOVE debug printf's from this function.
 *
 */
int initializeCompass(void)
{
  printf("I2CRead system\n");

  if ((file = open(FILENAME,O_RDWR)) < 0) {
    printf("ERRNO: %s\n",strerror(errno));
    exit(1);
  } else {
    printf("Opened %s\n", FILENAME);
  }

  printf("FILE = %d\n", file);
}

/**
 * getHeading()
 *
 * Get heading information from the Devantech Electronic Compass.
 *
 * @return int representing a one-byte value indicating heading
 * information.  The value of 0 is due North.
 *
 * TODO: REMOVE debug printf's from this function.
 */
int getHeading(void)
{
  printf("FILE = %d\n", file);
  printf("function getHeading entered\n");
  //Create int i for the 'for' loop
  int i;
  //Set the variables that must be set
  //Only one byte is read for short int
  int read_bytes = 1;
  //writeBuffer only needs to be 1 char in length because it only needs to hold a 1
  unsigned char writeBuffer[1] = {1};
  //Buffer to be read to.
  char buf[256];

  //bearing of the compass (from 0-359)
  int heading;

  if (ioctl(file,I2C_SLAVE,0x60) < 0) {
    printf("ictl ERR: %s\n",strerror(errno));
    return -1;
  }


  if (write(file,writeBuffer,1) < 1) {
    printf("in getHeading(): ERRNO: %s\n",strerror(errno));
    return -1;
    //exit(1);
  }

  if (read(file,buf,read_bytes) != read_bytes) {
    printf("in getHeading(): I2C Send %s Failed\n",FILENAME);
  }

  printf("in getHeading(): Addr: %s Data: ",FILENAME);
  for(i=0; i<read_bytes; i++)
  {
    //1.40784314 = conversion factor from 0-255 to 0-359
    //multiply heading by conversion, then add 0.5 for proper rounding and cast result to int
    //heading = (int)(0.5+1.40784314*((int)buf[i]));
    heading = (int)buf[i];
    printf("%d ",heading);
    printf("function getHeading cleared\n");
    return heading;
  }
  printf("\n");
}


/**
 * closeCompass()
 * 
 * Close the gumstix I2C port associated with the Devantech Electronic
 * Compass.
 *
 * @return int indicating if the operation was successful or not.
 * 0 is success, -1 failure.
 * 
 * TODO: REMOVE "exit(1)" from this function!!
 * TODO: REMOVE debug printf's from this function. * 
 * 
 */
int closeCompass(void)
{
  if(file >= 0){
      close(file);
      return 0;
  }else{
    printf("Couldn't close the compass. Sorry about that.\n");
    exit(1);
  }
}
