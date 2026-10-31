/* Autor: Tu Nombre */
#ifndef ACCIONES_H
#define ACCIONES_H

#include "inventario.h"
#include "historial.h"

#define ACCION_OK              0
#define ACCION_ERR_NO_EXISTE  -1   /* el objeto no esta en el inventario */
#define ACCION_ERR_MEMORIA    -2   /* no hubo memoria */
#define ACCION_ERR_VACIO      -3   /* no hay nada que deshacer */
#define ACCION_ERR_NO_DESHACER -4  /* la ultima accion no es descartar */
#define ACCION_ERR_ID_REPETIDO -5  /* ya existe un objeto con ese ID */

/*
 * Descarta un objeto por ID y registra la accion en el historial (push)
 * guardando una copia del objeto para poder deshacer.
 * Retorna: ACCION_OK, ACCION_ERR_NO_EXISTE o ACCION_ERR_MEMORIA.
 * Si falla el registro, el objeto vuelve al inventario.
 */
int descartar_objeto_por_id(Inventario *inv, Historial *h, int id);

/* Igual que la anterior, pero buscando el objeto por nombre exacto. */
int descartar_objeto_por_nombre(Inventario *inv, Historial *h, const char *nombre);

/*
 * Deshace la ultima accion SOLO si fue descartar un objeto: lo devuelve al
 * inventario y lo saca de la pila (peek y luego pop).
 * Retorna: ACCION_OK o un codigo ACCION_ERR_*.
 */
int deshacer_ultima_accion(Inventario *inv, Historial *h);

#endif