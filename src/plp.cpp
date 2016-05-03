#include "plp.h"
#include <cstdlib>
#include <iostream>
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
  cout << generateRand(3,255) << endl;
  return 0;
}

int generateRand(int init, int final){
  srand(time(NULL)); //Actualizar el Reloj
  return (rand()%(final-init))+init;
}


