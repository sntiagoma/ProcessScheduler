#include "pcp.h"
#include <string>
#include <map>
#include <unistd.h>
#include <typeinfo>
#include "structs.h"
#include <semaphore.h>
#include <pthread.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>

using namespace std;

sem_t queue;
sem_t mutex; //Para agrega al vector de estadisticas
sem_t msg; //Para la funcion print
sem_t** semaforos; //Para que el Hilo comienze a Trabajar
sem_t** mutexSem; //Para Modificar las tareas/estadisticas/libres
Tarea** tareas; //Para pasarle la tarea a hacer al hilo
bool* libres; //Saber cual hilo no esta ocupado
vector<Estadistica*> estadisticas;
bool terminar = false;

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
    HiloInfo* hilo_param = static_cast<HiloInfo*>(args);
    #ifdef DEBUG
        print(
            string("New Thread, PID:")+to_string(hilo_param->pid)+
            string(", ID:")+to_string(hilo_param->id)+ln
        );
    #endif
    while(true){
        sem_wait(semaforos[hilo_param->id]); //Sync Inicia
        int ret = -1000;
        pid_t p = fork();
        if(p==0){
            //semaforo?
            string workdir = string(getenv("PLN_DIR_TAREAS")+
                string("/")+string(tareas[hilo_param->id]->tareaAEjecutar)
            );
            #ifdef DEBUG
                print(
                    string("ih> P")+to_string(hilo_param->pid)+
                    string("h")+to_string(hilo_param->id)+
                    string("'ll exec:")+workdir+ln
                );
            #endif
            ret = execl(workdir.c_str(),
                tareas[hilo_param->id]->tareaAEjecutar, NULL);
        }else{}

        //lo de mmap para compatir el valor de ret
        int ret_fork = 0;
        waitpid(p, &ret_fork, 0); 
        print(
            string("hsalida> P")+to_string(hilo_param->pid)+
            string(":H")+to_string(hilo_param->id)
            +string(" ejecuto ")
            +string(tareas[hilo_param->id]->tareaAEjecutar)+ln
        );

        #ifdef DEBUG
            // print(string("ih> P")+to_string(hilo_param->pid)+
            //         string("h")+to_string(hilo_param->id)
            //         +string(" salio del exec")+ln);
        #endif
        /***********************************************************************
        *   
        *   Liberando Hilo y Dejando Estadistica
        *
        ***********************************************************************/
        sem_wait(mutexSem[hilo_param->id]); //Memoria Critica
            #ifdef DEBUG
                // print(string("ih> P")+to_string(hilo_param->pid)+
                //     string("h")+to_string(hilo_param->id)
                //     +string(" dentro de la zona critica")+ln);
            #endif
            Estadistica* newStat = new Estadistica; //Nueva Estadistica
            strcpy(newStat->tareaAEjecutar,             
                tareas[hilo_param->id]->tareaAEjecutar);
            newStat->procesoId = hilo_param->pid;    
            newStat->hiloId = hilo_param->id;
            sem_wait(&mutex); //Dejarla en el Vector de Estadisticas
                #ifdef DEBUG
                    // print(string("ih> P")+to_string(hilo_param->pid)+
                    //     string("h")+to_string(hilo_param->id)
                    //     +string(" dentro de la zona critica > el mutex")+ln);
                #endif
                estadisticas.push_back(newStat);
            sem_post(&mutex);
            #ifdef DEBUG
                // print(string("ih> P")+to_string(hilo_param->pid)+
                //     string("h")+to_string(hilo_param->id)
                //     +string(" fuera del mutex de la zona critica")+ln);
            #endif
            libres[hilo_param->id] = true;
            delete tareas[hilo_param->id]; //Eliminar Tarea que se envio
        sem_post(mutexSem[hilo_param->id]); //Libera la Memoria Critica
        #ifdef DEBUG
            // print(string("ih> P")+to_string(hilo_param->pid)+
            //     string("h")+to_string(hilo_param->id)
            //     +string(" fuera de la zona critica")+ln);
        #endif
    }
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
    semaforos = new sem_t* [pcpNumHilos];
    mutexSem = new sem_t* [pcpNumHilos];
    for(int i=0; i<pcpNumHilos; i++){
        semaforos[i] = new sem_t;
        sem_init(semaforos[i], 0, 0);
        mutexSem[i] = new sem_t;
        sem_init(mutexSem[i], 0, 1);
    }

    tareas = new Tarea* [pcpNumHilos];
    //estadisticas = new Estadistica* [pcpNumHilos];
    libres = new bool [pcpNumHilos];
    for(int i=0; i<pcpNumHilos; i++){
        libres[i] = true;
    }
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
        pthread_create(hilos[i], NULL, &hilo, (void*)hilo_param); //Creando proc
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

    /***************************************************************************
    *   
    *   Crea Mensaje y Reservar Espacio en MD
    *
    ***************************************************************************/
    Mensaje* mensaje = new Mensaje;

    /***************************************************************************
    *   
    *   Recibir, Asignar, Pasar
    *
    ***************************************************************************/
    while (read(0,mensaje,sizeof(Mensaje))){
        /***********************************************************************
        *   
        *   Leer Mensaje
        *
        ***********************************************************************/
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

        /***********************************************************************
        *   
        *   Asignar Tareas a Hilos Desocupados
        *
        ***********************************************************************/
        for(int i=0; i<mensaje->nTareas; i++){
            if(mensaje->tareas[i]->asignado==false){
                for(int j=0; j<pcpNumHilos; j++){
                    sem_wait(mutexSem[j]); //Zona Critica en Proceso e Hilo
                        if(libres[j]){
                            mensaje->tareas[i]->asignado = true;
                            tareas[j] = new Tarea;
                            memcpy(tareas[j],mensaje->tareas[i],sizeof(Tarea));
                            libres[j] = false;
                            sem_post(semaforos[j]); //WORK WORK WORK
                            sem_post(mutexSem[j]);
                            break;
                        }
                    sem_post(mutexSem[j]); //Fin Zona Critica
                }
            }
        }

        /***********************************************************************
        *   
        *   Ver quien ha terminado
        *
        ***********************************************************************/
        sem_wait(&mutex);
            #ifdef DEBUG
                // print(string("ipcp> P")+to_string(pcpNumId)
                //     +string(" dentro del mutex")+ln);
            #endif

            if(estadisticas.empty()){
                #ifdef DEBUG
                    // print(string("ipcp> P")+to_string(pcpNumId)
                    //     +string(" No hay Estatadisticas")+ln);
                #endif
            }else{
                //Copiando Estadisticas Previas
                for(int i=0; i<mensaje->nEstadisticas; i++){
                    estadisticas.push_back(mensaje->estadisticas[i]);
                }
                //Redefiniendo las estadisticas
                delete[] mensaje->estadisticas;
                mensaje->estadisticas = new Estadistica* [estadisticas.size()];
                for(int i=0; i<estadisticas.size(); i++){
                    mensaje->estadisticas[i] = estadisticas[i];
                }
                mensaje->nEstadisticas = estadisticas.size();
                estadisticas.clear();
                #ifdef DEBUG
                    // print(string("ipcp> P")+to_string(pcpNumId)
                    //     +string(" Estadisticas Redimensionadas a :")+
                    //     to_string(mensaje->nEstadisticas)+ln);
                #endif
            }
        sem_post(&mutex);

        /***********************************************************************
        *   
        *   Enviar Mensaje
        *
        ***********************************************************************/
        write(1,mensaje,sizeof(Mensaje));
        //Enviar Tareas
        for(int i=0; i<mensaje->nTareas; i++){
            write(1,mensaje->tareas[i],sizeof(Tarea));
        }
        //Enviar Estadisticas
        for(int i=0; i<mensaje->nEstadisticas; i++){
            write(1,mensaje->estadisticas[i],sizeof(Estadistica));
        }
        if(mensaje->nTareas==0){
            terminar = true;
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
        if(terminar){
            print(string("ipcp> PCP")+to_string(pcpNumId)+string(": Cerrado")+ln);
            break;
        }
    }
    delete mensaje;
    return 0;
}