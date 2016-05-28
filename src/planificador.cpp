#include "planificador.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "structs.h"

using namespace std;

string ln = string("\n");

void print(string message){
    write(2, message.c_str(), sizeof(char)*message.size());
}

int main(int argc, char** argv, char** envp){
    char *dirPLP = getenv("PLN_DIR_PLP");
    char *dirPCP = getenv("PLN_DIR_PCP");
    if(argc==1){
        print(string(dirPLP)+ln
            +string("Modo de empleo: planificador [-l <plpname>]\
            -n N [-t #pcp totalHilos ...]")+ln
        );
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
                    print(string("plpname:")+plpname+ln);
                #endif
            break;
    
            case 'n':
                tempn = stoi(string(optarg));
                #ifdef DEBUG
                    print(string("#procesos:")+to_string(tempn)+ln);
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
                    print(
                        string("Process:")+to_string(tempp)+
                        string(" 'll have ")+to_string(temph)
                        +string(" threads")+ln
                    );
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
        for(int i=1; i<n; i++){
            print(string("P")+to_string(i)+string("=")
                +to_string(procesos[i])+ln);
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
    string PLPDirectory = string(dirPLP)+string("/")+plpname;
    string PCPDirectory = string(dirPCP)+string("/")+string("pcp");
    #ifdef DEBUG
        print(string("PLP path:")+PLPDirectory+ln);
        print(string("PCP path:")+PCPDirectory+ln);
        print(
            string("Planificador, PID:")+to_string(getpid())
            +string(", PPID:")+to_string(getppid())+ln
        );
    #endif
    /***************************************************************************
    *   
    *   PLP
    *
    ***************************************************************************/
    if(fork()==0){ //Creacion del PLP
        #ifdef DEBUG
            print(
                string("Fork P0-PLP, PID:")+to_string(getpid())+
                string(", PPID:")+to_string(getppid())+ln
            );
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
                print(
                    string("Fork P")+to_string(i)+string("-PCP, PID:")+to_string(getpid())+
                    string(", PPID:")+to_string(getppid())+ln
                );
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
