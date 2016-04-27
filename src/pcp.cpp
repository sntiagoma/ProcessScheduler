#include "pcp.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv, char** envp){
  if(argc==3 or argc==5){
    //Work Work Work Work
  }else{
    printUsage();
    return 1;
  }

  return 0;
}
void printUsage(){
  cout << "Modo de empleo: pcp -i nProceso [-t nHilos]" << endl
  << "\t-i nProceso \t\t Identificador dentro del anillo" << endl
  << "\t-t nHilos \t\t Numero de hilos entre 2 y 10 (default: 3)"
  << endl;
  cout.flush();
}