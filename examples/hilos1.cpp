#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <typeinfo>

using namespace std;

void* hilo(void *);

sem_t sync1;
sem_t sync2;

int main(int argc, char const *argv[]){
	pthread_t hilo1, hilo2;
	sem_init(&sync1, 0, 0);
	sem_init(&sync2, 0, 0);
	int h = 0;
	int h1 = 1;
	int h2 = 2;
	pthread_create(&hilo1,NULL, &hilo, (void*)&h1);
	pthread_create(&hilo2,NULL, &hilo, (void*)&h2);

	
	while(cin>>h){
		if(h==1){
			sem_post(&sync1);
		}else if(h==2){
			sem_post(&sync2);
		}else{
			cerr << "You're Fucking Crazy" << endl;
		}
	}

	pthread_join(hilo1, NULL);
	pthread_join(hilo2, NULL);

	sem_destroy(&sync1);
	sem_destroy(&sync2);
	return 0;
}

void* hilo(void* args){
	int h = *((int*)args);
	while(true){
		if(h==1){
			sem_wait(&sync1);
		}else if(h==2){
			sem_wait(&sync2);
		}
		cout << "Hilo" << h << ":" << " working" << endl;
	}
}