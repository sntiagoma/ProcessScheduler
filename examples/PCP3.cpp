#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
using namespace std;

vector<bool>vbTareas;
vector<string>vsTareas;


sem_t mutex, semHilos;
int numProc;
string tarea; 

void *HiloTareas(void *){
  char *dir;
  dir = getenv("PLN_DIR_TAREAS");
  while(true){
    sem_wait(&semHilos);
    sem_wait(&mutex);
    cout << "fork" << endl << flush;
    string task = vsTareas.back(); 
    vsTareas.pop_back();
    sem_post(&mutex);

    if(fork()==0){
      string exec = dir + task;
      execl(exec.c_str(), task.c_str(), NULL);    	
    }
    int status;
    wait(&status);
    sem_wait(&mutex);
    //write(2,&msg,sizeof(msg));
    cerr << "Tarea finalizada con: " << WEXITSTATUS(status) << endl << flush;
    numProc++;
    sem_post(&mutex);
  }
}


int
main(int argc, char ** argv) {
    //Leer argumentos
    //ID proceso (Numero dentro del anillo)
    int idProc = stoi(argv[2]);

    string tarea = "Task";


    /*
    //Borrar case
    int numTarea = stoi(argv[5]);;
    
    switch(numTarea){
        case 1: tarea = "tarea01";
                break;
        case 2: tarea = "tarea02";
                break;
        case 3: tarea = "tarea03";
                break;
        case 4: tarea = "tarea04";
                break;
        case 5: tarea = "tarea05";
                break;
        case 6: tarea = "tarea06";
                break;
        case 10: tarea = "Task"; //Solo para testing, borrar <<<<<<<<<<<<<<<<<<<<<<<<
                 break;
        default:
                cerr << "Tarea invalida" << endl << flush;
                return -1;
                break;
    }//Borrar hasta aca
    */

    int nTareas = 20; //Representa el numero de tareas que se quieren completan, solo para testing, BORRAR <<<
    

    //n = numero de hilos - MIN 3 - MAX 10
    int n = stoi(argv[4]);
    cerr << "N" << n << endl << flush;
    

    //numProc se usa para los semaforos
    numProc = n;
    sem_init(&semHilos,0,0);
    sem_init(&mutex,0,1);
    pthread_t hilos[n];
    //Creacion de los hilos
    for(int i=0;i<n;i++){
        pthread_create(&hilos[i], NULL, HiloTareas, NULL); 
    }
    while(true){
        sem_wait(&mutex);
        if(numProc==0){
          //Si numProc == 0, todos los hilos estan ocupados
          sem_post(&mutex);
        }else{
          if(nTareas==0){
    	numProc = 0;
          }else{
            int na = numProc;
    	for(int i=0;(i<na)&&(nTareas>0);i++){
    	  vsTareas.push_back(tarea);
    	  numProc--;
    	  nTareas--;
          sem_post(&semHilos);
    	  cout << "numProc: " << numProc << endl << flush;
    	  cout << "nTareas: " << nTareas << endl << flush;
    	}
          }
          sem_post(&mutex);
        }
    }

    return 0;
}

  //int status;
  //wait(&status);
  //WEXITSTATUS(status);
  //printf("Una tarea termino con estado: %d, WEXITSTATUS(status));

