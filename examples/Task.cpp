#include <iostream>
#include <unistd.h>
#include <stdio.h>
using namespace std;

int main(void){
  cout << "Start" << endl << flush;
  usleep(3000000);
  cout << "End" << endl << flush;
  return -1;
}
