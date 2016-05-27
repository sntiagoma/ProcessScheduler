#include "plp.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include <typeinfo>
#include "structs.h"
using namespace std;

int main(int argc, char** argv, char** envp){
  srand(time(NULL)); //Reloj
  #ifdef DEBUG
    cerr << "I'm PLP, PID: " << getpid() << " PPID: " << getppid() << endl;
    cerr.flush();
  #endif
  //Tareas
  int nTareas = generateRand(3,255);
  #ifdef DEBUG
    cerr << "nTareas: " << nTareas << endl << flush;
  #endif
  // Mensaje Inicial
  Mensaje* mensaje = new Mensaje;
  mensaje->nTareas = nTareas;
  mensaje->nEstadisticas = 0;
  mensaje->tareas = new Tarea* [nTareas];
  mensaje->estadisticas = new Estadistica* [0];
  //Crear tareas
  for(int i=0; i<(mensaje->nTareas); i++){
    mensaje->tareas[i] = new Tarea;
    mensaje->tareas[i]->asignado = false;
    #ifdef DEBUG
      //cerr << "Tipo: " << typeid(mensaje->tareas[i]->tareaAEjecutar).name() << endl << flush;
      //cerr << "OTipo: "<< typeid(tareasArray[0]).name() << endl << flush; 
    #endif
    strcpy(mensaje->tareas[i]->tareaAEjecutar,tareasArray[generateRand(0,6)]);
    mensaje->tareas[i]->procesoId = 0;
    mensaje->tareas[i]->hiloId = 0;
  }
  //Crear Estadisticas
  for(int i=0; i<(mensaje->nEstadisticas); i++){
    mensaje->estadisticas[i] = new Estadistica;
    mensaje->estadisticas[i]->procesoId = 0;
    mensaje->estadisticas[i]->hiloId = 0;
  }
  #ifdef DEBUG
    cerr << "Mensaje->nTareas: " << mensaje->nTareas
    << ", sizeof(mensaje):" << sizeof(*mensaje) << endl 
    << "\tsizeof(tareas):" << sizeof(*(mensaje->tareas)) << endl
    << "\tsizeof(estadisticas):" << sizeof(*(mensaje->estadisticas)) << endl
    << flush;
  #endif
  //Enviar Mensaje
  write(1,mensaje,sizeof(Mensaje));
  //Enviar Tareas
  for(int i=0; i<mensaje->nTareas; i++){
    write(1,mensaje->tareas[i],sizeof(Tarea));
  }
  //Enviar Estadisticas
  for(int i=0; i<mensaje->nEstadisticas; i++){
    write(1,mensaje->estadisticas[i],sizeof(Estadistica));
  }
  while(read(0,mensaje,sizeof(Mensaje))){
    //Reseterar Tareas y Estadistica
    mensaje->tareas = new Tarea* [mensaje->nTareas];
    mensaje->estadisticas = new Estadistica* [mensaje->nEstadisticas];
    //Reservar Espacio c/Tarea y c/Estadistica
    for(int i=0; i<(mensaje->nTareas); i++){
      mensaje->tareas[i] = new Tarea;
    }
    for(int i=0; i<(mensaje->nEstadisticas); i++){
      mensaje->estadisticas[i] = new Estadistica;
    }
    //Leer Tareas
    for(int i=0; i<mensaje->nTareas; i++){
      read(0,mensaje->tareas[i],sizeof(Tarea));
    }
    //Leer Estadisticas
    for(int i=0; i<mensaje->nEstadisticas; i++){
      read(0,mensaje->estadisticas[i],sizeof(Estadistica));
    }

    /**
     * Do Something
     */
    #ifdef DEBUG
      int rDn = generateRand(0,nTareas-1);
      cerr << "PLP T" << rDn << " PID: " << mensaje->tareas[rDn]->tareaAEjecutar << endl << flush;
    #endif

    //Enviar Mensaje
    write(1,mensaje,sizeof(Mensaje));
    //Enviar Tareas
    for(int i=0; i<mensaje->nTareas; i++){
      write(1,mensaje->tareas[i],sizeof(Tarea));
    }
    //Enviar Estadisticas
    for(int i=0; i<mensaje->nEstadisticas; i++){
      write(1,mensaje->estadisticas[i],sizeof(Estadistica));
    }
    // Deletes
    for(int i=0; i<mensaje->nTareas; i++){
      delete mensaje->tareas[i];
    }
    delete[] mensaje->tareas;
    for(int i=0; i<mensaje->nEstadisticas; i++){
      delete mensaje->estadisticas[i];
    }
    delete[] mensaje->estadisticas;
  }
  delete mensaje;
  return 0;
}

// >= init  && < final
int generateRand(int init, int final){
  return (rand()%(final-init))+init;
}

