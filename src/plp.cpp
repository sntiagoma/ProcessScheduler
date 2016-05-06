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
  string a;
  cout << "Example" << endl;
  cout.flush();
  while (cin >> a){
    #ifdef DEBUG
      cerr << "DESDE PLP" << endl;
      cerr.flush();
    #endif
    cout << a << endl;
    cout.flush();
  }
  return 0;
}

int generateRand(int init, int final){
  srand(time(NULL)); //Actualizar el Reloj
  return (rand()%(final-init))+init;
}


