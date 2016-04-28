#include "plp.h"
#include <cstdlib>
#include <iostream>
using namespace std;
int main(int argc, char** argv, char** envp){
  cout << generateRand(3,255) << endl;
  return 0;
}

int generateRand(int init, int final){
  srand(time(NULL));
  return (rand()%(final-init))+init;
}