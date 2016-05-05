#include "planificador.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
int main(int argc, char** argv, char** envp){
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
            cout <<"plpname: " << plpname << endl;
          #endif
      break;

      case 'n':
        tempn = stoi(string(optarg));
          #ifdef DEBUG
            cout <<"n: " << tempn << endl;
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
            cout << "P:" << tempp << "- H:" << temph << endl;
            cout << "Mapa: " << mapHilos.size() << endl;
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
  cout << crearAnillo(n,hilos) << endl;

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

