#include "pcp.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv, char** envp){
  if(argc==3 or argc==5){
    if(string(argv[1]).compare("-i")!=0){
      printUsage();
      return 1;
    }
    if(argc==5 and string(argv[3]).compare("-t")!=0){
      printUsage();
      return 1;
    }

  }else{
    printUsage();
    return 1;
  }
  //Work Work Work Work
  return 0;
}
void printUsage(){
  cout << "Modo de empleo: pcp -i nProceso [-t nHilos]" << endl
  << "\t-i nProceso \t\t Identificador dentro del anillo" << endl
  << "\t-t nHilos \t\t Numero de hilos entre 2 y 10 (default: 3)"
  << endl;
  cout.flush();
}
