#include "plp.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <typeinfo>
#include <vector>
#include <signal.h>
#include "structs.h"
using namespace std;

string ln = string("\n");
int terminar = false;
vector<Tarea*> tareasaux;
vector<Tarea*> tareas;
vector<Estadistica*> estadisticas;
int generateRand(int init, int final){
    return (rand()%(final-init))+init;
}

void print(string message){
    write(2, message.c_str(), sizeof(char)*message.size());
}

int main(int argc, char** argv, char** envp){
    srand(time(NULL)); //Reloj
    #ifdef DEBUG
        print(string("New PLP, PID:")+to_string(getpid())+
            string(", PPID:")+to_string(getppid())+ln
        );
    #endif
    /***************************************************************************
    *   
    *   Generar Numero de Tareas
    *
    ***************************************************************************/
    int nTareas = generateRand(3,255);
    //int nTareas = generateRand(20,100);
    int nTareasR = 0; //Numero de tareas Recibido
    #ifdef DEBUG
        print(string("i> PLP sent #tareas:")+to_string(nTareas)+ln);
    #endif
    write(1,&nTareas,sizeof(nTareas)); //Enviando #Tareas
    read(0,&nTareasR,sizeof(int)); //Recibiendo Numero de Tareas
    #ifdef DEBUG
        print(string("i> PLP received #tareas:")+to_string(nTareasR)+ln);
    #endif

    if(nTareas!=nTareasR){
        print("Problem, # of works sent doen't match with received #"+ln);
        exit(4);
    }

    /***************************************************************************
    *   
    *   Mensaje Inicial
    *
    ***************************************************************************/
    Mensaje* mensaje = new Mensaje;
    mensaje->nTareas = nTareas;
    mensaje->nEstadisticas = 0;
    mensaje->tareas = new Tarea* [nTareas];
    mensaje->estadisticas = new Estadistica* [0];
    
    /***************************************************************************
    *   
    *   Crear Tareas
    *
    ***************************************************************************/
    for(int i=0; i<(mensaje->nTareas); i++){
        mensaje->tareas[i] = new Tarea;
        mensaje->tareas[i]->asignado = false;
        strcpy(mensaje->tareas[i]->tareaAEjecutar,tareasArray[generateRand(0,6)]);
        mensaje->tareas[i]->procesoId = 0;
        mensaje->tareas[i]->hiloId = 0;
    }

    /***************************************************************************
    *   
    *   Crear Estadisticas
    *
    ***************************************************************************/
    for(int i=0; i<(mensaje->nEstadisticas); i++){
        mensaje->estadisticas[i] = new Estadistica;
        mensaje->estadisticas[i]->procesoId = 0;
        mensaje->estadisticas[i]->hiloId = 0;
    }
    #ifdef DEBUG
        print(string("Mensaje:\n\t->nTareas:")+to_string(mensaje->nTareas)
            +string("\n\t->nEstadisticas:")+to_string(mensaje->nEstadisticas));
        for(int i=0; i<mensaje->nTareas; i++){
            print(
                string("\n\t->[")+to_string(i)+string("] | "+
                    string(mensaje->tareas[i]->tareaAEjecutar)
                )
            );
        }
        print(ln);
    #endif
    /***************************************************************************
    *   
    *   Enviar Mensaje Primera Vez
    *
    ***************************************************************************/
    write(1,mensaje,sizeof(Mensaje));
    //Enviar Tareas
    for(int i=0; i<mensaje->nTareas; i++){
        write(1,mensaje->tareas[i],sizeof(Tarea));
    }
    //Enviar Estadisticas
    for(int i=0; i<mensaje->nEstadisticas; i++){
        write(1,mensaje->estadisticas[i],sizeof(Estadistica));
    }

    /***************************************************************************
    *   
    *   Recibir, Controlar y Reenviar
    *
    ***************************************************************************/
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
        if(mensaje->nEstadisticas>0){
            #ifdef DEBUG
                print(string("iplp> PLP recibio :")
                    +to_string(mensaje->nEstadisticas)+string(" estadisticas")
                    +ln);
            #endif
            //Pasar estadisticas al Vector
            for(int i=0; i<mensaje->nEstadisticas; i++){
                estadisticas.push_back(mensaje->estadisticas[i]);
                #ifdef DEBUG
                    print(string("iplp> Agregada Estadistica P")+
                        to_string(mensaje->estadisticas[i]->procesoId)+
                        string(":H")
                        +to_string(mensaje->estadisticas[i]->hiloId)+
                        string(":")
                        +string(mensaje->estadisticas[i]->tareaAEjecutar)+ln
                    );
                #endif
            }
            delete[] mensaje->estadisticas;
            mensaje->estadisticas = new Estadistica* [0];
            mensaje->nEstadisticas = 0;
        }

        #ifdef DEBUG
            print(string("eplp> PLP, nTareas:")+to_string(mensaje->nTareas)+
                string(", nEstadisticas:")+to_string(mensaje->nEstadisticas)
                +string(", size <estadisticas>:")+to_string(estadisticas.size())
                +ln);
        #endif

        if(mensaje->nTareas==estadisticas.size()){
            terminar = true;
            mensaje->nTareas=0;
            mensaje->nEstadisticas=0;
        }

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
        if(terminar){
            print(string("ipcp> PLP")+string(": Cerrado")+ln);
            for(int i=0; i<estadisticas.size(); i++){
                print(
                    string("ppcp> E: P")
                    +to_string(estadisticas[i]->procesoId)+
                    string(":H")
                    +to_string(estadisticas[i]->hiloId)+
                    string(":")+
                    string(estadisticas[i]->tareaAEjecutar)+ln
                );
            }
            break;
        }
    }
    delete mensaje;
    return 0;
}


