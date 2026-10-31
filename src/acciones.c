/* Autor: Tu Nombre */
#include <stdio.h>
#include <string.h>
#include "acciones.h"

/*
 * Descarta un objeto por ID y registra la accion en el historial.
 * Parametros: inv - inventario, h - historial, id - ID del objeto a descartar.
 * Retorna: ACCION_OK si se descarto,
 *          ACCION_ERR_NO_EXISTE si no existe (o inv o h son NULL),
 *          ACCION_ERR_MEMORIA si no hay memoria para registrar la accion.
 * Errores: si no se puede registrar, el objeto vuelve al inventario.
 */
int descartar_objeto_por_id(Inventario *inv, Historial *h, int id) {
    Objeto objeto;
    Accion accion;

    if (inv == NULL || h == NULL) {
        return ACCION_ERR_NO_EXISTE;
    }
    if (inventario_eliminar_por_id(inv, id, &objeto) != 0) {
        return ACCION_ERR_NO_EXISTE;
    }

    memset(&accion, 0, sizeof(accion));
    accion.tipo = ACCION_DESCARTAR_OBJETO;
    snprintf(accion.descripcion, MAX_DESCRIPCION, "Se descarto %s", objeto.nombre);
    accion.objeto_descartado = objeto;

    if (historial_push(h, accion) != 0) {
        inventario_agregar(inv, objeto);        /* revertir: no se pudo registrar */
        return ACCION_ERR_MEMORIA;
    }
    return ACCION_OK;
}

/*
 * Descarta un objeto buscandolo por nombre y registra la accion en el historial.
 * Parametros: inv - inventario, h - historial, nombre - nombre exacto del objeto.
 * Retorna: ACCION_OK si se descarto,
 *          ACCION_ERR_NO_EXISTE si no existe (o inv o nombre son NULL),
 *          ACCION_ERR_MEMORIA si no hay memoria para registrar la accion.
 */

int descartar_objeto_por_nombre(Inventario *inv, Historial *h, const char *nombre) {
    if (inv == NULL || nombre == NULL) {
        return ACCION_ERR_NO_EXISTE;
    }
    NodoInventario *nodo = inventario_buscar_por_nombre(inv, nombre);
    if (nodo == NULL) {
        return ACCION_ERR_NO_EXISTE;
    }
    return descartar_objeto_por_id(inv, h, nodo->objeto.id);
}

/*
 * Deshace la ultima accion del historial, solo si fue descartar un objeto:
 * devuelve el objeto al inventario y lo saca de la pila.
 * Parametros: inv - inventario, h - historial.
 * Retorna: ACCION_OK si se deshizo,
 *          ACCION_ERR_VACIO si no hay acciones (o algun puntero es NULL),
 *          ACCION_ERR_NO_DESHACER si la ultima accion no fue descartar,
 *          ACCION_ERR_ID_REPETIDO si ya existe otro objeto con ese ID,
 *          ACCION_ERR_MEMORIA si no hay memoria para devolver el objeto.
 * Errores: si algo falla, el historial queda sin cambios.
 */
int deshacer_ultima_accion(Inventario *inv, Historial *h) {
    Accion accion;

    if (inv == NULL || h == NULL) {
        return ACCION_ERR_VACIO;
    }
    if (historial_peek(h, &accion) != 0) {
        return ACCION_ERR_VACIO;                /* pila vacia */
    }
    if (accion.tipo != ACCION_DESCARTAR_OBJETO) {
        return ACCION_ERR_NO_DESHACER;
    }
    if (inventario_buscar_por_id(inv, accion.objeto_descartado.id) != NULL) {
        return ACCION_ERR_ID_REPETIDO;
    }
    /* Primero se devuelve el objeto: si falla, el historial queda intacto. */
    if (inventario_agregar(inv, accion.objeto_descartado) != 0) {
        return ACCION_ERR_MEMORIA;
    }
    historial_pop(h, &accion);
    return ACCION_OK;
}