/* Autor: Tu Nombre */
#ifndef COLA_COMBATE_H
#define COLA_COMBATE_H

#include "enemigo.h"

/* Estructura que representa un nodo en la cola de combate. */
typedef struct NodoCola {
    Enemigo enemigo;
    struct NodoCola *siguiente;
} NodoCola;

/* Estructura que representa la cola de combate. */
typedef struct {
    NodoCola *frente;   /* por aqui sale el siguiente enemigo */
    NodoCola *final;    /* por aqui entra el nuevo enemigo */
    int cantidad;
} ColaCombate;


/* Inicializa una cola de combate. */
void cola_iniciar(ColaCombate *c);

/* Encola un enemigo en la cola de combate.*/
int  cola_encolar(ColaCombate *c, Enemigo e); //* 0 ok, -1 si vacia 

/* Desencola un enemigo de la cola de combate.*/
int  cola_desencolar(ColaCombate *c, Enemigo *e);      //* 0 ok, -1 si vacia 

/* Obtiene el enemigo del frente de la cola.*/
int  cola_frente(const ColaCombate *c, Enemigo *e);    //* 0 ok, -1 si vacia 

/* Verifica si la cola de combate está vacia.*/
int  cola_esta_vacia(const ColaCombate *c);

/* Libera la memoria de la cola de combate.*/
void cola_liberar(ColaCombate *c);

#endif