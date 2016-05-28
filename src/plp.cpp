#include "plp.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <typeinfo>
#include "structs.h"
using namespace std;

string ln = string("\n");

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

        
        //Do Something
        
        #ifdef DEBUG
            int rDn = generateRand(0,nTareas-1);
            print(string("PLP T")+to_string(rDn)+string(", PID:")+string(mensaje->tareas[rDn]->tareaAEjecutar)+ln);
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


