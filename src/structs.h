#pragma once

char tareasArray[6][64] = {
    "tarea01",
    "tarea02",
    "tarea03",
    "tarea04",
    "tarea05",
    "tarea06"
};
const int MAX_TAREAS =  256;
const int MAX_TEXT_TAREA = 64;

struct Tarea{
    bool asignado;
    char tareaAEjecutar[MAX_TEXT_TAREA];
    unsigned short procesoId;
    unsigned short hiloId;
};

struct Estadistica {
    char tareaAEjecutar[MAX_TEXT_TAREA];
    unsigned short procesoId;
    unsigned short hiloId;
};

struct Mensaje{
    unsigned nTareas;
    unsigned nEstadisticas;
    Tarea** tareas;
    Estadistica** estadisticas;
};

struct HiloInfo{
    unsigned id;
    unsigned pid;
};