#include "planificador.h"
#include <iostream>
#include <map>
#include <unistd.h>
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
  delete[] hilos;
  return 0;
}