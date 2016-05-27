#include "pcp.h"
#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <typeinfo>
#include "structs.h"
#include <semaphore.h>
#include <pthread.h>

using namespace std;

sem_t queue, mutex;

void printUsage(){
    cerr << "Modo de empleo: pcp -i nProceso [-t nHilos]" << endl
    << "\t-i nProceso \t\t Identificador dentro del anillo" << endl
    << "\t-t nHilos \t\t Numero de hilos entre 2 y 10 (default: 3)"
    << endl;
    cerr.flush();
}

void* hilo(void* args){
    HiloInfo* hilo_param = (HiloInfo*)args;
    #ifdef DEBUG
        cerr << "\t\tDesde el Hilo_" << hilo_param->id << " del Proceso_" << hilo_param->pid
            << endl << flush;
    #endif
}

int main(int argc, char** argv, char** envp){
    if(argc==1){
        printUsage();
        exit(1);
    }
    int pcpNumId = stoi(string(argv[2]));
    int pcpNumHilos = 3;
    if(argc==5){
        pcpNumHilos = stoi(string(argv[4]));
    }
    #ifdef DEBUG
        cerr << "I'm PCP " << pcpNumId << " with " << pcpNumHilos << " threads"
            <<", PID: " << getpid() << " PPID " << getppid() << endl;
        cerr.flush();
    #endif

    /****************************************************************************
    *   Semaforos
    *
    *
    *
    ****************************************************************************/

    sem_init(&queue, 0, 0); //Cola de procesos esperando (De sincronización)
    sem_init(&mutex, 0, 1); //Mutex acceso a 

    /****************************************************************************
    *   Hilos
    *
    *
    * **************************************************************************/
  
    pthread_t** hilos = new pthread_t* [pcpNumHilos]; //Arreglo de Hilos
    for (int i = 0; i < pcpNumHilos; i++){ //Inicializando Espacios
        hilos[i] = new pthread_t;
    }
    for (int i = 0; i < pcpNumHilos; i++){
        HiloInfo* hilo_param = new HiloInfo;
        hilo_param->id = i;
        hilo_param->pid = pcpNumId;
        pthread_create(hilos[i], NULL, &hilo, (void*)hilo_param); //Creando proceso
    }

    Mensaje* mensaje = new Mensaje;
    while (read(0,mensaje,sizeof(Mensaje))){
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
        //++mensaje->tareas[0]->procesoId;
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
    //pthread_exit(NULL);
    return 0;
}