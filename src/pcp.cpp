#include "pcp.h"
#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
using namespace std;

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
  map<int,bool> hilos; //para almacenar la tabla de hilos disponibles
  
  /*if(argc==3 or argc==5){
    if(string(argv[1]).compare("-i")!=0){
      printUsage();
      return 1;
    }
    if(argc==5 and string(argv[3]).compare("-t")!=0){
      printUsage();
      return 1;
    }
    if(argc==5){
      pcpNumHilos = stoi(string(argv[4]));
    }
    pcpNumId = string(argv[2]);
  }else{
    printUsage();
    return 1;
  }*/
  //Work Work Work Worksave in map c++(
  //inicializo los hilos de este proceso en True, estan disponibles
  //for (int i=1;i<stoi(string(argv[4]));i++)
  //  hilos[i] = true;
  #ifdef DEBUG
    cerr << "I'm PCP " << pcpNumId << " with " << pcpNumHilos << " threads"
    <<", PID: " << getpid() << " PPID " << getppid() << endl;
    cerr.flush();
  #endif
  string a;
  while (cin >> a){
    #ifdef DEBUG
      cerr << "Desde P" << argv[2] << ":" << endl;
      cerr.flush();
    #endif
    cout << a << (char)(generateRand(97,122)) << endl;
    cout.flush();
  }
  return 0;
}
void printUsage(){
  cerr << "Modo de empleo: pcp -i nProceso [-t nHilos]" << endl
  << "\t-i nProceso \t\t Identificador dentro del anillo" << endl
  << "\t-t nHilos \t\t Numero de hilos entre 2 y 10 (default: 3)"
  << endl;
  cerr.flush();
}
int generateRand(int init, int final){
  srand(time(NULL)); //Actualizar el Reloj
  return (rand()%(final-init))+init;
}