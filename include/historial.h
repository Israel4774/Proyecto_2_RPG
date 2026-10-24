/* Autor: <tu nombre> */
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
    Objeto objeto_descartado;   /* solo vale si tipo == ACCION_DESCARTAR_OBJETO */
} Accion;

typedef struct NodoHistorial {
    Accion accion;
    struct NodoHistorial *siguiente;   /* el que está debajo */
} NodoHistorial;

typedef struct {
    NodoHistorial *top;
    int cantidad;
} Historial;



#endif

