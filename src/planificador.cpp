#include "planificador.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

typedef int Pipe[2];

int main(int argc, char** argv, char** envp){
  if(argc==1){
    cerr << "Modo de empleo: planificador [-l <plpname>] -n N [-t npcp totalHilos ...]" << endl;
    return 1;
  }
  int tempn, tempp, temph;
  extern char *optarg;
  extern int optind, opterr, optopt;
  int option;
  string plpname = string(""); //Nombre del PLP
  int n = 3; //Numero de procesos
  map<int,int> mapHilos; //Mapa con proceso y cuantos hilos tiene
  while ((option = getopt(argc, argv, "l:n:t:")) != -1) {
    switch (option) {
      case 'l':
        plpname = string(optarg);
          #ifdef DEBUG
            cerr <<"plpname: " << plpname << endl;
            cerr.flush();
          #endif
      break;

      case 'n':
        tempn = stoi(string(optarg));
          #ifdef DEBUG
            cerr <<"n: " << tempn << endl;
            cerr.flush();
          #endif
        if(tempn>=2 and tempn<=255){
          n=tempn;
        }else{
          return 3;
        }
      break;

      case 't':
        tempp = stoi(string(optarg));
        if(tempp>=n){
          return 4;
        }
        temph = stoi(string(argv[optind]));
        mapHilos[tempp] = temph;
          #ifdef DEBUG
            cerr << "P:" << tempp << "- H:" << temph << endl;
            cerr.flush();
          #endif
      break;
    }
  }
  int* hilos = new int[n];
  for(int i=0; i<n; i++){ //Inicialice en 3
    hilos[i] = 3;
  }
  for(auto i:mapHilos){ // Cambie los que estan
    hilos[i.first] = i.second;
  }
  #ifdef DEBUG
    for(int i=0; i<n; i++){
      cout << "hilos[" << i << "]" << "=" << hilos[i] << endl;
    }
  #endif  
  //Inicializando Matrix de Tuberias
  int** tuberias = new int*[n];
  //Crea Matrix e Inicializa los Arreglos
  for (int i = 0; i < n; i++){
    tuberias[i] = new int[2];
  }
  //Crear los Pipes
  for (int i = 0; i < n; i++){
    pipe(tuberias[i]);
  }
  #ifdef DEBUG
    cerr << "Planificador PID: " << getpid() << " and PPID: " << getppid() << endl;
  #endif
  //PLP
  if(fork()==0){
    #ifdef DEBUG
      cerr << "I'm 0 process, I'm the PLP PID: " << getpid() << " and my parent is " << getppid() << endl;
      cerr.flush();
    #endif
    //Configurando Pipes
    for(int i=0; i<n; i++){
      if(i==0){
        close(tuberias[i][0]);
        dup2(tuberias[i][1],1); //Donde va a Escribir
        close(tuberias[i][1]); //?
      }else if(i==(n-1)){
        close(tuberias[i][1]);
        dup2(tuberias[i][0],0); //De donde va a Leer
        close(tuberias[i][0]); //?
      }else{ //Cerrar los demas
        close(tuberias[i][0]);
        close(tuberias[i][1]);
      }
    }
    execlp("./plp","./plp", NULL);
  }
  //PCP
  pid_t pid;
  for (int i = 1; i < n; i++) {
    pid = fork();
    //Configurando Pipes
    if (pid == 0) {
      #ifdef DEBUG
          cerr << "I'm " << i << " process, I'm a PCP PID: " << getppid() << " and my parent is " << getppid() << endl;
          cerr.flush();
      #endif
      for (int j = 0; j < n; j++) {
        if (j == i) {
          close(tuberias[j][0]);
          dup2(tuberias[j][1], 1);
          close(tuberias[j][1]);
        } else if (j == (i - 1)) {
          close(tuberias[j][1]);
          dup2(tuberias[j][0], 0);
          close(tuberias[j][0]);
        } else {
          close(tuberias[j][0]);
          close(tuberias[j][1]);
        }
      }
      execlp("./pcp", "./pcp", "-i", to_string(i).c_str(), "-t", to_string(hilos[i]).c_str(), NULL);
    }
  }
  int status;
  for (int i = 0; i < n; i++){
    wait(&status);
  }
  delete[] hilos;
  //Eliminando Matrix de Tuberias
  for (int i = 0; i < n; i++){
    delete[] tuberias[i];
  }
  delete[] tuberias;
  //cout << crearAnillo(n,hilos) << endl;

  return 0;
}
int crearAnillo(int nprocs, int *hilos){
    pid_t childpid;
    pid_t anterior;
    int tuberia[2];
    if (pipe(tuberia) == -1){
        cout <<"Fallo la creacion del pipe" << endl;
        return 0;
    }/*
    if (fork() > 0){
      execl("./plp","./plp",0);
        cout<<"Si se imprime esto, no se pudo crear el programa";
    }*/
    dup2(tuberia[0],0);
    dup2(tuberia[1],1);
    close(tuberia[0]);
    close(tuberia[1]);
    if ((childpid = fork()) == 0){
        execl("./plp","./plp",0);
        cout<<"Si se imprime esto, no se pudo crear el programa";
    }
    for (int i=1;i<nprocs;i++){
        pipe(tuberia);
        if ((anterior = fork()) > 0){
            //execl("./pcp","./pcp","-i",i,"-t",hilos[i],0);
            //execl("./pcp","./pcp",0);
            fprintf(stderr, "Creo el fork padre \n");
            dup2(tuberia[1],1);
        }else{
            dup2(tuberia[0],0); //hijo
            fprintf(stderr, "Creo el fork hijo \n");
        }
        close(tuberia[0]);
        close(tuberia[1]);
        if (anterior)
            break;
    }


}

