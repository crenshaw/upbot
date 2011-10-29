/* Filename: saccFilt.c
 * Authors:	Michael Liedtke, Tim Yandl, Andrew Nuxoll
 * Created: 20 Sep 2011
 * Last Modified: 28 Sep 2011
 *
 * TODO 
 *
 **/
 

#include "saccFilt.h"
#include "../communication/communication.h"
#include "supervisor.h"

#include <jni.h>
#include <stdlib.h>
#include <stdio.h>


//note: we are aware that we do not destroy the JVM at the end of a successful run.
// our expiriments indicate that the JVM is automatically freed.
JNIEnv *env = NULL; //the environmental pointer
JavaVM *jvm = NULL; //JVM pointer

jclass clid_Filter = NULL; //the calss ID for SaccFilter
jobject obid_FilterObj = NULL; //the object ID for SaccFilter
jmethodID mid_initFilter = NULL; //The ID for the constructor
jmethodID mid_runFilter = NULL; //The ID for the runFilter method
jmethodID mid_filterCommand = NULL; //The ID for the filterCommand method
jmethodID mid_getSensors = NULL; //The ID for the getSensorArray method

/**
*This method was taken from the example at http://www.codeproject.com/KB/cpp/CJniJava.aspx
*Basically it sets up the JVM and returns a pointer to the java environment
*and sets a pointer to the JVM as a side-effect.
*/
JNIEnv* create_vm(JavaVM ** jvm)
{
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options;
    options.optionString = "-Djava.class.path=../supervisor"; //Path to the java source code assuming it is in the same directory...
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;

    //if the JVM could not be created, exit now... the program is hopeless.
    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if(ret < 0)
    {
        //If we failed to set up the JVM, abondonship the program is hopeless.
        printf("--Unable to Launch JVM--\n");
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
        printf("--failed to load class--\n");
        (*jvm)->DestroyJavaVM(jvm);
        return EXIT_FAILURE;
    }
    //consructor ID for SaccFilter
    mid_initFilter = (*env)->GetMethodID(env, clid_Filter, "<init>", "()V");
    //runFilter ID
    mid_runFilter = (*env)->GetMethodID(env, clid_Filter, "runFilter", "([C)[C");
    //filterCommand
    mid_filterCommand = (*env)->GetMethodID(env, clid_Filter, "filterCommand", "(I)I");
    //getSensorArray ID
    mid_getSensors = (*env)->GetMethodID(env, clid_Filter, "getSensorArray", "()[C");



    /************************************************************************/
    /*      Now we will call the functions using the their method IDs       */
    /************************************************************************/
    if(mid_runFilter == NULL || mid_initFilter == NULL
        || mid_getSensors == NULL)
    {
        //if the methods were not found, there is no point in continuing,
        //exit with a fauilure code.
        printf("--failed to load method(s): ");
        if(mid_runFilter == NULL){printf(" runFilter");}
        if(mid_initFilter == NULL){printf(" initFilter");}
        if(mid_filterCommand == NULL){printf(" filterCommand");}
        if(mid_getSensors == NULL){printf(" getSensors");}
        printf("--\n");

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
    //Setup the JVM if it isn't already running.
    if (env == NULL)
    {
        env = create_vm(&jvm);
        init_ids();
    }
    
    jchar* temp = (jchar*)malloc(SENSOR_LENGTH * sizeof(jchar));
    int i;
    for(i=0; i < SENSOR_LENGTH; i++)
    {
        temp[i] = input[i];
    }
    
    //create a jcharArray from the jchar[].
    jcharArray jca; //the new jcharArray
    //requests that java make an array of length SENSOR_LENGTH to fit our array
    jca = (*env)->NewCharArray(env, SENSOR_LENGTH);
    //copies our array into the jcharArray.
    (*env)->SetCharArrayRegion(env, jca, 0, SENSOR_LENGTH, temp);
    
    
    free(temp);
    temp = NULL;
    
    
    //call the method in Java to run the saccfilter and return a thinned array.
    //the array returned will need to be converted back into a C-friendly form
    //before we can pass it back out to the calling program.
    jcharArray out = (jcharArray)(*env)->CallObjectMethod(env, obid_FilterObj, mid_runFilter, jca);
    temp = (*env)->GetCharArrayElements(env, out, NULL);
    
    //check that the extraction was successful
    if (temp == NULL) {
        return; /* exception occurred */
    }
    
    char* ret = (char*)malloc(SENSOR_LENGTH * sizeof(char));
    
    //modify the sensor array to reflect the result
    for(i=0; i<SENSOR_LENGTH; i++)
    {
        ret[i] = (char)temp[i];
    }
    
    //prevent memory leak by telling java that we are done with this array.
    (*env)->ReleaseCharArrayElements(env, out, temp, 0);
    return ret;
}


/**
 *saccReceiveAction() takes an integer command and forwards it to the environment
 *
 *@param int command code.
 */
int saccReceiveAction(int command)
{
    //Setup the JVM if it isn't already running.
    if (env == NULL)
    {
        env = create_vm(&jvm);
        init_ids();
    }
    
    int ret = (*env)->CallIntMethod(env, obid_FilterObj, mid_filterCommand, (jint)command);
    //send command to the environment
    return ret;
}


/**
 *saccGetCurrSensing() takes a sensing array buffer and fills it with
 * the current sensing.
 *
 * @param buf    the 10 bit sense
 */
void saccGetCurrSensing(char *buf)
{
    //Setup the JVM if it isn't already running.
    if (env == NULL)
    {
        env = create_vm(&jvm);
        init_ids();
    }
    
    //gets sensor array by calling the method in java. then must extract the array
    //from the jcharArray. we now have an array of jchars which needs to be cast
    //element by element to chars just in-case other parts of the program don't appreciate jchars...
    jcharArray out = (jcharArray)(*env)->CallObjectMethod(env, obid_FilterObj, mid_getSensors, NULL);
    int js = (int)(*env)->GetArrayLength(env, out);//testing the actual length of the array (as java sees it)
    jchar * tempArray = (*env)->GetCharArrayElements(env, out, NULL);
    
    //casting each element to char and adding them to *buf.
    int i;
    for(i=0; i<SENSOR_LENGTH; i++)
    {
        buf[i] = (char)tempArray[i];
    }
    buf[SENSOR_LENGTH] = '\0';
    
    //prevent memory leak by telling java that we are done with this array.
    (*env)->ReleaseCharArrayElements(env, out, tempArray, 0);
    return;
}





