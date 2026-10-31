/* Autor: Tu Nombre */
#include <stdlib.h>
#include "cola_combate.h"

/* Deja la cola vacia. Parametros: c - cola (no NULL). */
void cola_iniciar(ColaCombate *c) {
    c->frente = NULL;
    c->final = NULL;
    c->cantidad = 0;
}

/* Retorna 1 si la cola no tiene enemigos, 0 si tiene. */
int cola_esta_vacia(const ColaCombate *c) {
    return c->frente == NULL;
}

/*
 * Agrega un enemigo al final de la cola.
 * Parametros: c - cola, e - enemigo a copiar en un nodo nuevo.
 * Retorna: 0 si salio bien, -1 si no hay memoria.
 */
int cola_encolar(ColaCombate *c, Enemigo e) {
    NodoCola *nuevo = malloc(sizeof(NodoCola));
    if (nuevo == NULL) {
        return -1;                      /* memoria insuficiente */
    }
    nuevo->enemigo = e;
    nuevo->siguiente = NULL;

    if (cola_esta_vacia(c)) {           /* caso: cola vacia */
        c->frente = nuevo;
        c->final = nuevo;
    } else {                            /* caso: hay elementos */
        c->final->siguiente = nuevo;
        c->final = nuevo;
    }
    c->cantidad++;
    return 0;
}

/*
 * Saca el enemigo del frente y lo copia en *e (puede ser NULL).
 * Retorna: 0 si salio bien, -1 si la cola esta vacia.
 */
int cola_desencolar(ColaCombate *c, Enemigo *e) {
    if (cola_esta_vacia(c)) {
        return -1;
    }
    NodoCola *nodo = c->frente;
    c->frente = nodo->siguiente;
    if (c->frente == NULL) {            /* era el unico: final tambien en NULL */
        c->final = NULL;
    }
    if (e != NULL) {
        *e = nodo->enemigo;
    }
    free(nodo);
    c->cantidad--;
    return 0;
}

/*
 * Copia el enemigo del frente en *e sin sacarlo.
 * Retorna: 0 si salio bien, -1 si esta vacia o e es NULL.
 */
int cola_frente(const ColaCombate *c, Enemigo *e) {
    if (cola_esta_vacia(c) || e == NULL) {
        return -1;
    }
    *e = c->frente->enemigo;
    return 0;
}

/* Libera todos los nodos y deja la cola vacia. */
void cola_liberar(ColaCombate *c) {
    while (!cola_esta_vacia(c)) {
        cola_desencolar(c, NULL);
    }
}