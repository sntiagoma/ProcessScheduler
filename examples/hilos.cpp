#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <typeinfo>

using namespace std;

void* hilo(void *);

sem_t queue, mutex;

int main(int argc, char const *argv[]){
	sem_init(&queue, 0, 0);
	sem_init(&mutex, 0, 1);
	cout << "¿Cuantos hilos?: ";
	int nHilos = 0;
	cin >> nHilos;
	pthread_t** hilos = new pthread_t* [nHilos];
	for (int i = 0; i < nHilos; i++)
	{
		hilos[i] = new pthread_t;
	}
	for (int i = 0; i < nHilos; i++){
		int* hiloId = new int;
		*hiloId = i;
		pthread_create(hilos[i], NULL, &hilo, (void*)hiloId);
	}
	int h;
	while(cin>>h){
		if(h>=nHilos){
			cerr << "You're Fucking Crazy" << endl;
			continue;
		}else{
			sem_post(&queue);
		}
	}
	sem_destroy(&queue);
	sem_destroy(&mutex);
	return 0;
}

void* hilo(void* args){
	int h = *((int*)args);
	while(true){
		sem_wait(&queue);
		cerr << "Respondio Hilo_" << h << endl << flush;
	}
}