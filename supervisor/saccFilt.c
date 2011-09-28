/* Filename: saccFilt.c
 * Authors:	Michael Liedtke, Tim Yandl, Andrew Nuxoll
 * Created: 20 Sep 2011
 * Last Modified: -
 *
 * TODO 
 *
 **/
 

#include "saccFilt.h"
#include "../communication/communication.h"
#include "supervisor.h"

#include <jni.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#define SENSOR_LENGTH 10

JNIEnv *env = NULL; //the environmental pointer
JavaVM *jvm = NULL; //JVM pointer

jclass clid_Filter = NULL; //the calss ID for SaccFilter
jobject obid_FilterObj = NULL; //the object ID for SaccFilter
jmethodID mid_initFilter = NULL; //The ID for the constructor
jmethodID mid_runFilter = NULL; //The ID for the runFilter method

/**
*This method was taken from the example at http://www.codeproject.com/KB/cpp/CJniJava.aspx
*Basically it sets up the JVM and returns a pointer to the java environment.
*/
JNIEnv* create_vm(JavaVM ** jvm)
{
   JNIEnv *env;
   JavaVMInitArgs vm_args;
   JavaVMOption options;
   options.optionString = "-Djava.class.path=."; //Path to the java source code assuming it is in the same directory...
   vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
   vm_args.nOptions = 1;
   vm_args.options = &options;
   vm_args.ignoreUnrecognized = 0;

   //if the JVM could not be created, exit now... the program is hopeless.
   int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
   if(ret < 0)
   {
      printf("\nUnable to Launch JVM\n");
      exit(EXIT_FAILURE);
   }
   return env;
}

int init_ids()
{
    //Obtaining Classes
   clid_Filter = (*env)->FindClass(env, "SaccFilter");
   
   //Obtaining Method IDs method signatures can be obtained by the
   //linux command javap -s -p <java_classname>
   if (clid_Filter == NULL)
   {
      //if the class was not found, there is no point in continuing,
      //exit with a fauilure code.
      printf("failed to load class\n");
      (*jvm)->DestroyJavaVM(jvm);
      return EXIT_FAILURE;
   }
   //consructor ID for SaccFilter
   mid_initFilter = (*env)->GetMethodID(env, clid_Filter, "<init>", "()V");
   //runFilter ID
   mid_runFilter = (*env)->GetMethodID(env, clid_Filter, "runFilter", "([C)[C");

   
   
   /************************************************************************/
   /*      Now we will call the functions using the their method IDs       */
   /************************************************************************/
   if(mid_runFilter == NULL || mid_initFilter == NULL)
   {
      //if the methods were not found, there is no point in continuing,
      //exit with a fauilure code.
      printf("failed to load methods:\n");
      (mid_runFilter == NULL) ? printf("runFilter\n") : 0;
      (mid_initFilter == NULL) ? printf("initFilter\n") : 0;
      
      (*jvm)->DestroyJavaVM(jvm);
      return EXIT_FAILURE;
   }
   //requests that java make an instance of a SaccFilt object, which we will call runFilter.
   //the first parameter is the java environment pointer, the second is the class ID we obtained for SaccFilter
   //the third is the constructor method ID and the last parameter(s) are the arguements for the method
   //in this case NULL because there are none.
   obid_FilterObj = (jobject)(*env)->NewObject(env, clid_Filter, mid_initFilter, NULL);
}

/* 
 *saccReceiveState() takes in a 10 bit sense and returns a thinned senses array.
 *
 * @param the 10 bit sense
 */
char * saccReceiveState(char * input)
{
    if (env == NULL)
    {
        env = create_vm(&jvm);
        init_ids();
    }
    
    
    
    return input;
}


/**
 *saccReceiveAction() takes an integer command and forwards it to the environment
 *
 *@param int command code.
 */
int saccReceiveAction(int command)
{
    //send command to the environment
    return command;
}


/**
 *saccGetCurrSensing() takes a sensing array buffer and fills it with
 * the current sensing.
 *
 * @param buf    the 10 bit sense
 */
void saccGetCurrSensing(char *buf)
{

}
