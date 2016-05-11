#include "pcp.h"
#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include "structs.h"
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
  #ifdef DEBUG
    cerr << "I'm PCP " << pcpNumId << " with " << pcpNumHilos << " threads"
    <<", PID: " << getpid() << " PPID " << getppid() << endl;
    cerr.flush();
  #endif

  int num;
  //while (cin >> a){
  while (read(0,&num,sizeof(int))){
    #ifdef DEBUG
      cerr << "Desde P" << argv[2] << ":" << endl;
      cerr.flush();
    #endif
    ++num;
    write(1,&num,sizeof(int));
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