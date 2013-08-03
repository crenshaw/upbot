//file includes the following functions
void setEventResponder(char * erName);
void cleanupER();
void initalizeStopER();


//selectNextER is not defined in this file but it is expected to be placer here
void selectNextER(char * erName);

/*
typedef struct erLLTag {
  char * name;
  void * setupFunc;
  struct erLLTag * next;
} erLL;


//a linked list of our event responders
erLL* ers;

void setupERS() {
  ers = malloc(sizeof(erLL));
  ers->name = "stop";
  ers->setupFunc = initalizeStopER;
  ers->next = NULL;
}
setupERS();

//yet another spot where I miss lua...
*/



/*
 * Sets the event responde based on the name provided
 */
void setEventResponder(char * erName) {
  cleanupER();

  selectNextER(erName);
  
  //now set the clock for the next event responder if needed
  if (myER.states[myER.curState].clockTime > 0) {
    setClock(myER.states[myER.curState].clockTime,0);
  }
}


/*
 * This function will free all dynamic memory within
 * myER and zero everything on the stack
 */
void cleanupER() {
  
  int i,j;
  for (i=0;i<myER.stateCount;++i) {
    for (j=0;j<myER.states[i].count;++j) {
      free(&(myER.states[i].transitions[j]));
    }
    free(&(myER.states[i]));
  }
  myER.curState = 0;
  myER.stateCount = 0;

}

/**
 * stop responder is always present. Thatway we can
 * initalize the robot in a responder which won't have it running
 * away
 */
void initalizeStopER() {
  myER.curState = 0;
  myER.stateCount = 1;
  myER.states = malloc(sizeof(state));
  
    myER.states[0].count = 1;  
    myER.states[0].clockTime = 0; 
    myER.states[0].transitions = malloc(sizeof(transition)); 
  
      myER.states[0].transitions[0].e = eventTrue; 
      myER.states[0].transitions[0].r = respondStop;
      myER.states[0].transitions[0].n = 0;
  
}
