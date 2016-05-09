#include "plp.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;
const int MAX_TAREAS = 255;
const int MAX_TEXT_TAREA = 64;
/*
struct Tarea{
    bool asignado;
    char tareaAEjecutar[MAX_TEXT_TAREA];
    unsigned short procesoId;
    unsigned short hiloId;
};
struct Estadistica {
    char tareaAEjecutar[MAX_TEXT_TAREA];
    unsigned short procesoId;
    unsigned short hiloId;
};

struct Mensaje {
    unsigned nTareas;
    unsigned nEstadisticas;
    Tarea tareas[nTareas];
    Estadistica estadisticas[nEstadisticas];
};
*/
int main(int argc, char** argv, char** envp){
  #ifdef DEBUG
    cerr << "I'm PLP, PID: " << getpid() << " PPID: " << getppid() << endl;
    cerr.flush();
  #endif
  //cout << generateRand(3,255) << endl;
  //string a;
  int num = 0;
  //cout << "Example" << endl;
  //cout.flush();
  write(1,&num,sizeof(int));
  //while (cin >> a){
  while(read(0,&num,sizeof(int))){ 
    #ifdef DEBUG
      cerr << "DESDE PLP- num:" << num << endl;
      cerr.flush();
    #endif
    write(1,&num,sizeof(int));
    //cout << a << endl;
    //cout.flush();
  }
  /*char* algo = "Algo";
  write(1,&x,sizeof(x));
  int y = 0;
  read(0,&y,sizeof(y));
  cerr << "Es mierda llego con :" << to_string(y) << endl;
  cerr.flush();*/
  return 0;
}

int generateRand(int init, int final){
  srand(time(NULL)); //Actualizar el Reloj
  return (rand()%(final-init))+init;
}


