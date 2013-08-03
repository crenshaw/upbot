
void initalizeWanderER();
void initalizeStopER();
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

void selectNextER(char * erName) {

  if (strcmp(erName,"go")==0) {
   initalizeWanderER();
  }
  
}

/*
 * This function will free all dynamic memory within
 * myER and zero everything else
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


void initalizeWanderER() {
  
  myER.curState = 0;
  myER.stateCount = 2;
  myER.states = malloc(sizeof(state)*2);
  
    myER.states[0].count = 3;  
    myER.states[0].clockTime = 5; 
    myER.states[0].transitions = malloc(sizeof(transition)*3); 
  
      myER.states[0].transitions[0].e = eventAlarm; 
      myER.states[0].transitions[0].r = respondDriveMed;
      myER.states[0].transitions[0].n = 1;
  
      myER.states[0].transitions[1].e = eventBump; 
      myER.states[0].transitions[1].r = respondTurn;
      myER.states[0].transitions[1].n = 0;
  
      myER.states[0].transitions[2].e = eventTrue; 
      myER.states[0].transitions[2].r = respondDriveLow;
      myER.states[0].transitions[2].n = 0;
  
    myER.states[1].count = 2;
    myER.states[1].clockTime = 0;
    myER.states[1].transitions = malloc(sizeof(transition)*3); 
     
      myER.states[1].transitions[0].e = eventBump; 
      myER.states[1].transitions[0].r = respondTurn;
      myER.states[1].transitions[0].n = 0;
  
      myER.states[1].transitions[1].e = eventTrue; 
      myER.states[1].transitions[1].r = respondDriveMed;
      myER.states[1].transitions[1].n = 1;
 
}

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
