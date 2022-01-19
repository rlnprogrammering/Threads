#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

void menu(); // write controls to console
void *userinput(void *arg); // takes user input
void *stopwatch(void *arg); // main function for stopwatch
void clear(); // clears terminal
void reset(int *laps, int *min, int *sec, int *milli); // resets the variables for the stopwatch
int lap(int laps, int min, int sec, int milli); // function for laps
int kbhit(void); // register keypress on keyboard
void printlaps(int laps); // prints laps to terminal 
char c; // global variable for userinput
bool start = false; // condition for starting/stopping stopwatch
int arrmin[25], arrsec[25], arrmilli[25]; // arrays for storing laps

int main(){
  pthread_t thread1, thread2; // variables for our threads

  // initialize threads
  pthread_create(&thread1, NULL, &userinput, NULL);
  pthread_create(&thread2, NULL, &stopwatch, NULL);

  // wait for threads to finish executing
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  return 0;
}

void *stopwatch(void *arg){
  int min = 0, sec = 0, milli = 0, laps = 0; // initialize variables for stopwatch
  clear();
  menu();
  printf("\r%02d : %02d : %02d", min, sec, milli);

  while(!start){ // repeat program
    switch (c){
      case 's': // start stopwatch
        start = true;
        clear();
        menu();
        if(laps > 0){
          printlaps(laps);
        }
        c = '\0'; // reset c variable
        break;
      case 'r': // reset stopwatch
        clear();
        menu();
        reset(&laps, &min, &sec, &milli);
        c = '\0'; // reset c variable
        break;
      case 'e': // terminate thread2
        clear();
        exit(0); 
        break;
    }

      while(start){
        switch (c){
          case 's': // stop stopwatch
            start = false;
            clear();
            menu();
            if(laps > 0){
              printlaps(laps);
            }
            c = '\0';
            break;
          case 'e': // terminate thread2
            clear();
            exit(0); 
            c = '\0';
            break;
          case 'l': // lap
            clear();
            menu();
            laps = lap(laps, min, sec, milli);
            c = '\0';
            break;
        }
            
          // print current time
      printf("\r%02d : %02d : %02d", min, sec, milli);
      milli++;

      if(milli > 99){
        milli = 0;
        sec++;
      }
      if(sec > 59){
        sec = 0;
        min++;
      }
      if(min > 59){
        min = 0;
        sec = 0;
        milli = 0;
      }

      usleep(10000); // Sleep for 10000 microseconds (10 milliseconds)
    }
  }
}

void *userinput(void *arg){
  while(1){
    if(kbhit()){ // if keyboard press is registered
      c = getchar(); // save keyboard press in c
      c = tolower(c); // make sure it is a lower case letter
      if(c == 'e'){ // terminate thread1
        clear();
        exit(0); 
      }
    }
  }
}

// https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
// Linux version of kbhit
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt); // writes the settings of stdin to address of oldt
  newt = oldt; // copy stdin settings and store and newt
  newt.c_lflag &= ~(ICANON | ECHO); // changes flag of ICANON so user doesn't need to press enter and flag of ECHO so characters dont appear twice
  tcsetattr(STDIN_FILENO, TCSANOW, &newt); // set the new settings to STDIN
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0); // get the file access mode and the file status flags for STDIN
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // set the status flags of STDIN to oldf, and prevent it from blocking for too long

  ch = getchar(); // get user input

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // change STDIN settings back to normal
  fcntl(STDIN_FILENO, F_SETFL, oldf); // set the status flags for STDIN to oldf

  if(ch != EOF) // return 1 if keyboard has been pressed
  {
    ungetc(ch, stdin); // undo getchar
    return 1;
  }

  return 0; // return 0 if keyboard hasnt been pressed
}

void menu(){
  printf("Press s to start/stop\n"
          "Press l to lap(max 25 laps)\n"
          "Press r to reset\n"
          "Press e to exit\n\n");
}

void clear(){
  printf("\e[1;1H\e[2J"); // clear terminal
}

int lap(int laps, int min, int sec, int milli){
  laps++;
  // store variables in arrays
  arrmin[laps-1] = min;
  arrsec[laps-1] = sec;
  arrmilli[laps-1] = milli;

  // clear arrays when laps is larger than 25
  if(laps > 25){
  laps = 0;
  for(int i = 0; i < 25; i++){
    arrmin[i] = 0;
    arrsec[i] = 0;
    arrmilli[i] = 0;
    }
  }
  
  printlaps(laps); 
  return laps;
}

void reset(int *laps, int *min, int *sec, int *milli){
  // resets variables
  *milli = 0;
  *sec = 0;
  *min = 0;
  *laps = 0;
  for(int i = 0; i < 25; i++){ // clear arrays
    arrmin[i] = 0;
    arrsec[i] = 0;
    arrmilli[i] = 0;
  }
  printf("\r%02d : %02d : %02d", *min, *sec, *milli);
}

void printlaps(int laps){
  for(int i = 0; i < laps; i++){ // print all the laps
    printf("Lap %d: %02d : %02d : %02d \n", i+1, arrmin[i], arrsec[i], arrmilli[i]);
  }    
}







