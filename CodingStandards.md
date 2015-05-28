# UPBOT Coding Standards #

The issues below are listed in alphabetical order according to topic.  Coding standards related to **Bracket Style** appear before those related to **Indentation**.

## Bracket Style ##
  * Opening brackets should be placed on the same line as the function that is being called.

## Comments ##
  * Issues to address should be annotated with TODO:
  * Comment headers for files should be done in the style of Doxygen.  They should:
    * begin with /`*``*`.
    * have the file's name at the top.
    * provide a description of the file's purpose.
    * specify the original author using @author.
    * specify the original creation date using @since.

Example taken from compass.c
```
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
```

  * Comment headers for functions should be done in the style of Doxygen.  They should:
    * begin with /`*``*`.
    * include the function's name, with ().
    * specify each parameter using @param.
    * specify the return value using @return.

```
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
```

## Conditionals ##
  * Conditionals that test assignment should be placed inside of the if statement

Pretty good usage (C):
```
    if ((check = checkArgName(argc, argv, addressess)) == -1) {
        //do something
    }
```

## Constants ##
  * Pre-processor constants must always be capitalized.
  * Use pre-processor constants that accurately describe the the condition bein\
g tested:

Unclear usage that does not convey programmer intention (C):
```
  if(( pid = fork()) < 0)
    {
 perror("fork error");
    }
```

Pretty good usage (C):
```
  // The fork() system call returns -1 if it cannot fork a process.  There are four
  // reasons why a fork call might fail: see the value of errno after the failed call.
  #define CANNOT_FORK (-1)


  if(( pid = fork()) == CANNOT_FORK)
    {
      perror("fork error");
    }
```

## Header Files ##
  * All `*`.c files that contain functions used by other `*`.c files must be accompanied by a header that contains the constants used and the function prototypes for the functions.
  * As with all files, all header files should have a comment header describing their purpose.
  * To avoid compilation errors, header files should:
    1. Include anything that is needed for the contents of the header file only.
    1. Use the #ifndef/#define construct to avoid redefinition errors.

Pretty good usage (C):
```
/**                                                                                
 * acceptor.h                                                                      
 *                                                                                 
 * Provides the interfaces necessary to connect to remote services in              
 * the UPBOT robotics system.  Entities that use the acceptor                      
 * establish a passive-mode endpoint and await connection from other               
 * entities.                                                                       
 *                                                                                 
 * @author Tanya L. Crenshaw                                                       
 * @since July 2013                                                                
 */

#include "services.h"

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

/**                                                                                
 *  CONSTANT DEFINITIONS.  All constants in this file should begin                 
 *  with 'ACC' to indicate their membership in eventresponder.h                    
 */

#define ACC_SUCCESS 0

// ** The multitude of ways that accCreateConnection can fail. **                                 
#define ACC_SOCK_OPT_FAILURE (-1)
#define ACC_SOCK_BIND_FAILURE (-2)
#define ACC_BAD_PORT (-3)
#define ACC_NULL_SH (-4)
#define ACC_SOCK_LISTEN_FAILURE (-5)
#define ACC_SIGACTION_FAILURE (-6)
#define ACC_SOCK_ACCEPT_FAILURE (-7)

// The number of backlog requests accepted by any acceptor.                        
#define ACC_BACKLOG 10

/**                                                                                
 * Function prototypes.  See acceptor.c for details on                             
 * this/these functions.                                                           
 */
int accCreateConnection(char * port, serviceType type, serviceHandler * sh);
int accAcceptConnection(serviceHandler * sh);
int accBroadcastService(serviceHandler * sh);
int accCompleteConnection(serviceHandler * sh);

#endif
```

## Including Files ##
  * When including `*`.h files, include standard library headers first, followed by custom header files.

```
   #include <stdlib.h>
   #include <stddef.h>

   #include "acceptor.h"
   #include "services.h"
```

  * Never #include a `*`.c file.  No exceptions.

## Indentation ##
  * Indented blocks should use four spaces

## Makefiles ##
  * Except for single-file, throw-away experiments, all programs must be accompanied by a makefile.
  * To be consistent across all code, makefiles should be entitled, 'makefile', that is, with a little m.
  * Makefiles should have comment headers that describe their purpose, author, and creation date.  For example,

```
   # Filename: makefile (for daemon demo)  
   # Author: Fernando Freire
   # Date created: 7/7/2013 
```


  * Makefiles should generate executable files with the .out extension to indicate their binary status and to insure that git automatically ignores them.  For example

```
daemon: daemon_objects
        @gcc -pthread -o tbd.out become_daemon.o test_become_daemon.c
```

## Variables ##
  * Variable names must always convey programmer intention.
  * Whenever possible, variables should be initialized with some default value at the time of their declaration.
  * Pointers should be declared with a space between the type and the star.


Unclear usage that does not convey programmer intention or see initialization of variables (C):
```
int var1, var2;
char * p;
```

Pretty good usage (C):
```
int count = 0;
char * robotName = "Webby";
```