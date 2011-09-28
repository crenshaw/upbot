/*
* By Timothy Yandl
* Note: when used in the upbot project, remove testing code
* also, be aware that the JVM should only be created once and
* only destroyed when the program quits... otherwise we will have
* a major memory leak.
*/

#include <jni.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define SENSOR_LENGTH 26

/**
*This method was taken from the example at http://www.codeproject.com/KB/cpp/CJniJava.aspx
*Basically it sets up the JVM and returns a pointer to the java environment.
*/
JNIEnv* create_vm(JavaVM ** jvm)
{
   JNIEnv *env;
   JavaVMInitArgs vm_args;
   JavaVMOption options;
   options.optionString = "-Djava.class.path=."; //Path to the java source code
   vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
   vm_args.nOptions = 1;
   vm_args.options = &options;
   vm_args.ignoreUnrecognized = 0;

   //if the JVM could not be created, exit now... the program is hopeless.
   int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
   if(ret < 0)
   {
      printf("\nUnable to Launch JVM\n");
      return;
   }
   return env;
}

/**
*The main method, gets a Java environment pointer.
*gets class and method IDs for the java class(es) and methods that will be used
*then makes an array, turns it into a jintArray
*creates an instance of the java class and passes the jchararray into the
*runFilter method which returns a jcharArray.
*the chars are extracted from the jcharArray and printed.
*end of program, release the JVM.
*/
int main(int argc, char* argv[])
{
   JNIEnv *env; //the environmental pointer
   JavaVM * jvm; //JVM pointer
   int x;
   env = create_vm(&jvm); //create the JVM.
   if (env == NULL)
   {
      //No JRE, just exit, it's hopeless.
      return EXIT_FAILURE;
   }
   
   jclass clid_Filter = NULL; //the calss ID for SaccFilter
   jobject obid_FilterObj = NULL; //the object ID for SaccFilter
   jmethodID mid_initFilter = NULL; //The ID for the constructor
   jmethodID mid_runFilter = NULL; //The ID for the runFilter method
   
   
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
   
   
   

   //create an array of length SENSOR_LENGTH
   jchar* tempArray = (jchar*)malloc(SENSOR_LENGTH * sizeof(jchar)); 
   
   /**************************************************************************
   /*                           Delete this code                             *
   /*************************************************************************/
   
   //fill the new array with chars A-J. and print them to the console
   int i;
   for(i=0;i<SENSOR_LENGTH;i++)
   {
      tempArray[i] = (jchar)i+65;
      printf("%c",tempArray[i]);
   }
   printf("\n");
   
   /**************************************************************************
   /*                             End Delete                                 *
   /*************************************************************************/
   
   //create a jcharArray from the jchar[].
   jcharArray jca; //the new jcharArray
   //requests that java make an array of length SENSOR_LENGTH to fit our array
   jca = (*env)->NewCharArray(env, SENSOR_LENGTH);
   //copies our array into the jcharArray.
   (*env)->SetCharArrayRegion(env, jca, 0, SENSOR_LENGTH, tempArray);
   
   //release extra resources
   free(tempArray);
   tempArray = NULL;
   
   //Calling runFilter method int the object 'FilterObj' of class SaccFilter.
   //The method returns an jcharArray in the form of a jobject which needs a cast to jcharArray.
   jcharArray out = (jcharArray)(*env)->CallObjectMethod(env, obid_FilterObj, mid_runFilter, jca);
   
   //extracting the underlying chars.
   jchar * tempArray2 = (*env)->GetCharArrayElements(env, out, NULL);
   
   //check that the extraction was successful
   if (tempArray2 == NULL) {
      return; /* exception occurred */
   }
   
   /**************************************************************************
   /*                           Delete this code                             *
   /*************************************************************************/
   //print each element to the console
   for(i=0;i<SENSOR_LENGTH;i++)
   {
      printf("%c",tempArray2[i]);
   }
   printf("\n");
   
   /**************************************************************************
   /*                             End Delete                                 *
   /*************************************************************************/
   
   //Release resources.
   getchar();
   (*jvm)->DestroyJavaVM(jvm);
   return EXIT_SUCCESS;
}
