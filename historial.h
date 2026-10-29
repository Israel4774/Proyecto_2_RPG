#ifndef HISTORIAL_H
#define HISTORIAL_H

#include "objeto.h"

#define MAX_DESCRIPCION 100

typedef enum {
    ACCION_ENCONTRAR_OBJETO,
    ACCION_DERROTAR_ENEMIGO,
    ACCION_USAR_OBJETO,
    ACCION_DESCARTAR_OBJETO,
    ACCION_OTRA
} TipoAccion;

typedef struct {
    TipoAccion tipo;
    char descripcion[MAX_DESCRIPCION];
    Objeto objeto_descartado;
} Accion;

typedef struct NodoHistorial {
    Accion accion;
    struct NodoHistorial *siguiente;
} NodoHistorial;

typedef struct {
    NodoHistorial *top;
    int cantidad;
} Historial;

/* Inicializa la pila vacía. */
void historial_iniciar(Historial *h);

/* Agrega una acción al tope: 0 si funciona, -1 si falla. */
int historial_push(Historial *h, Accion accion);

/* Retira la acción superior: 0 si funciona, -1 si está vacía o hay error. */
int historial_pop(Historial *h, Accion *accion);

/* Copia la acción superior sin retirarla: 0 si funciona, -1 si falla. */
int historial_peek(const Historial *h, Accion *accion);

/* Devuelve 1 si está vacía, 0 si no; -1 si h es NULL. */
int historial_esta_vacio(const Historial *h);

/* Muestra las acciones de la más reciente a la más antigua. */
void historial_mostrar(const Historial *h);

/* Libera todos los nodos de la pila. */
void historial_liberar(Historial *h);

#endif
