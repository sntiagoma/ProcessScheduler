#include "pcp.h"
#include <string>
#include <map>
#include <unistd.h>
#include <typeinfo>
#include "structs.h"
#include <semaphore.h>
#include <pthread.h>

using namespace std;

sem_t queue, mutex, msg;
string ln = string("\n");

void printUsage(){
    string message = string(
        "Modo de empleo: pcp -i nProceso [-t nHilos]\n\
        \t-i nProceso \t Identificador dentro del anillo\n\
        \t-t nHilos \t Numero de hilos entre 2 y 10 (default: 3)"
    );
    write(2, message.c_str(), sizeof(char)*message.size());
}

void print(string message){
    sem_wait(&msg);
        write(2, message.c_str(), sizeof(char)*message.size());
    sem_post(&msg);
}

void* hilo(void* args){
    HiloInfo* hilo_param = (HiloInfo*)args;
    #ifdef DEBUG
        sem_wait(&mutex);
        print(
            string("New Thread, PID:")+to_string(hilo_param->pid)+
            string(", ID:")+to_string(hilo_param->id)+ln
        );
        sem_post(&mutex);
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

    /***************************************************************************
    *   
    *   Semaforos
    *
    ***************************************************************************/

    sem_init(&queue, 0, 0); //Cola de procesos esperando (De sincronización)
    sem_init(&mutex, 0, 1); //Mutex acceso a 
    sem_init(&msg,   0, 1); //Mutex acceso a print()


    #ifdef DEBUG
        print(string("New PCP, id:")+to_string(pcpNumId)+string(" #thread:")+
            to_string(pcpNumHilos)+string(", PID:")+to_string(getpid())+
            string(", PPID:")+to_string(getppid())+ln
        );
    #endif

    /***************************************************************************
    *   
    *   Hilos
    *
    * *************************************************************************/
  
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

    /***************************************************************************
    *   
    *   Recibir Numero de Tareas
    *
    ***************************************************************************/
    int nTareas = 0;
    read(0,&nTareas,sizeof(nTareas)); //Recibir
    #ifdef DEBUG
        print(string("P")+to_string(pcpNumId)+string(" get nTareas=")+
            to_string(nTareas)+ln);
    #endif
    write(1,&nTareas,sizeof(nTareas)); //Pasar al Siguiente PCP

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