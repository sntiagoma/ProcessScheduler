#include "plp.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>
#include "structs.h"
using namespace std;
int main(int argc, char** argv, char** envp){
  #ifdef DEBUG
    cerr << "I'm PLP, PID: " << getpid() << " PPID: " << getppid() << endl;
    cerr.flush();
  #endif
  int num = 0;
  /**
   * TAREAS
   */
  int nTareas = generateRand(3,255);
  cerr << "nTareas: " << nTareas << endl << flush;
  /**
   * IO
   */
  write(1,&num,sizeof(int));
  while(read(0,&num,sizeof(int))){ 
    #ifdef DEBUG
      cerr << "DESDE PLP- num:" << num << endl;
      cerr.flush();
    #endif
    write(1,&num,sizeof(int));
  }
  return 0;
}

int generateRand(int init, int final){
  srand(time(NULL)); //Actualizar el Reloj
  return (rand()%(final-init))+init;
}


