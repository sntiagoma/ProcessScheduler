#include "planificador.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"

using namespace std;

int main(int argc, char** argv, char** envp){
    char *dirPLP = getenv("PLN_DIR_PLP");
    char *dirPCP = getenv("PLN_DIR_PCP");
    if(argc==1){
        cerr << dirPLP << endl;
        cerr << "Modo de empleo: planificador [-l <plpname>] -n N [-t #pcp totalHilos ...]" << endl;
        exit(1);
    }

    /***************************************************************************
    *   
    *   Parametros
    *
    ***************************************************************************/
    int tempn, tempp, temph;
    extern char *optarg;
    extern int optind, opterr, optopt;
    int option;
    string plpname = string("plp"); //Nombre del PLP
    int n = 3; //Numero de procesos
    map<int,int> mapHilos; //Mapa con proceso y cuantos procesos tiene
    while ((option = getopt(argc, argv, "l:n:t:")) != -1) {
        switch (option) {
            case 'l':
                plpname = string(optarg);
                #ifdef DEBUG
                    cerr <<"plpname: " << plpname << endl;
                    cerr.flush();
                #endif
            break;
    
            case 'n':
                tempn = stoi(string(optarg));
                #ifdef DEBUG
                    cerr <<"n: " << tempn << endl;
                    cerr.flush();
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
                    cerr << "P:" << tempp << "- H:" << temph << endl;
                    cerr.flush();
                #endif
            break;
        }
    }

    /***************************************************************************
    *   
    *   Crear Procesos
    *
    ***************************************************************************/

    int* procesos = new int[n];
    for(int i=0; i<n; i++){ //Inicializamos todos en 3
        procesos[i] = 3;
    }
    for(auto i:mapHilos){ // Cambiamos los que por parametros tienen != de 3
        procesos[i.first] = i.second;
    }
    #ifdef DEBUG
        for(int i=0; i<n; i++){
            cerr << "procesos[" << i << "]" << "=" << procesos[i] << endl;
            cerr.flush();
        }
    #endif  

    /***************************************************************************
    *   
    *   Pipes y Procesos
    *
    ***************************************************************************/

    int** tuberias = new int*[n]; //Matrix de Pipes
    for (int i = 0; i < n; i++){   //Crea Matrix e Inicializa los Arreglos
        tuberias[i] = new int[2];
    }
    for (int i = 0; i < n; i++){ //Crear los Pipes
        pipe(tuberias[i]);
    }
    #ifdef DEBUG
        cerr << "Planificador PID: " << getpid() << " and PPID: " << getppid() << endl << flush;
    #endif

    /***************************************************************************
    *   
    *   PLP
    *
    ***************************************************************************/
    if(fork()==0){ //Creacion del PLP
        #ifdef DEBUG
            cerr << "I'm 0 process, I'm the PLP PID: " << getpid() << " and my parent is " << getppid() << endl;
            cerr.flush();
        #endif
        for(int i=0; i<n; i++){ //Configurando Pipes para el PLP
            if(i==0){
                close(tuberias[i][0]);
                dup2(tuberias[i][1],1); //Donde va a Escribir
                close(tuberias[i][1]);
            }else if(i==(n-1)){
                close(tuberias[i][1]);
                dup2(tuberias[i][0],0); //De donde va a Leer
                close(tuberias[i][0]);
            }else{ //Cerrar los demas
                close(tuberias[i][0]);
                close(tuberias[i][1]);
            }
        }
        string PLPDirectory = string(dirPLP)+string("/")+plpname;
        #ifdef DEBUG
            cerr << "PLP path:" << PLPDirectory.c_str() << endl << flush;
        #endif
        execl(PLPDirectory.c_str(),plpname.c_str(), NULL);
    }

    /***************************************************************************
    *   
    *   PCPs
    *
    ***************************************************************************/
    pid_t pid;
    for (int i = 1; i < n; i++) { //Creacion de (n-1) PCPs
        pid = fork();
        if (pid == 0) { //Creacion de PCP
            #ifdef DEBUG
                cerr << "I'm " << i << " process, I'm a PCP PID: " << getppid() << " and my parent is " << getppid() << endl;
                cerr.flush();
            #endif
            for (int j = 0; j < n; j++) { //Configurando Pipes para los PCP
                if (j == i) {
                    close(tuberias[j][0]);
                    dup2(tuberias[j][1], 1);
                    close(tuberias[j][1]);
                } else if (j == (i - 1)) {
                    close(tuberias[j][1]);
                    dup2(tuberias[j][0], 0);
                    close(tuberias[j][0]);
                } else {
                    close(tuberias[j][0]);
                    close(tuberias[j][1]);
                }
            }
            string PCPDirectory = string(dirPCP)+string("/")+string("pcp");
            #ifdef DEBUG
                cerr << "PCP path:" << PCPDirectory.c_str() << endl << flush;
            #endif
            execl(PCPDirectory.c_str(), "pcp", "-i", to_string(i).c_str(), "-t", to_string(procesos[i]).c_str(), NULL);
        }
    }

    /***************************************************************************
    *   
    *   Waits
    *
    ***************************************************************************/
    int status;
    for (int i = 0; i < n; i++){
        wait(&status);
    }

    /***************************************************************************
    *   
    *   Deletes
    *
    ***************************************************************************/
    delete[] procesos;
    //Eliminando Matrix de Tuberias
    for (int i = 0; i < n; i++){
        delete[] tuberias[i];
    }
    delete[] tuberias;
    return 0;
}
