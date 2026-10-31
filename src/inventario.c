#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventario.h"

/* Convierte el tipo de objeto a texto. 
 * Parametros: tipo - tipo de objeto a convertir.
 * Retorna: cadena de texto correspondiente al tipo de objeto.
 */
static const char *tipo_a_texto(TipoObjeto tipo) {
    switch (tipo) {
        case ARMA:     return "arma";
        case ARMADURA: return "armadura";
        case POCION:   return "pocion";
        default:       return "otro";
    }
}

/* Inicializa el inventario vacío.
 * Parametros: inv - inventario a inicializar.
 * Retorna: nada.
 */
void inventario_iniciar(Inventario *inv) {
    inv->cabeza = NULL;
    inv->cantidad = 0;
}

/* Funcion para agregar un objeto al inventario.
 * Parametros: inv - inventario donde agregar el objeto, obj - objeto a agregar.
 * Retorna: 0 si se agrego correctamente, -1 si hubo un error.
 */
int inventario_agregar(Inventario *inv, Objeto obj) {
    NodoInventario *nuevo = malloc(sizeof(NodoInventario));
    if (nuevo == NULL) {
        return -1;                  /* memoria insuficiente */
    }
    nuevo->objeto = obj;
    nuevo->siguiente = NULL;

    if (inv->cabeza == NULL) {      /* caso: lista vacía */
        inv->cabeza = nuevo;
    } else {                        /* caso: hay elementos, buscar el último */
        NodoInventario *actual = inv->cabeza;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
    inv->cantidad++;
    return 0;
}

/* Muestra el inventario completo.
 * Parametros: inv - inventario a mostrar.
 * Retorna: nada.
 */
void inventario_listar(const Inventario *inv) {
    if (inv->cabeza == NULL) {
        printf("El inventario esta vacio.\n");
        return;
    }

    const NodoInventario *actual = inv->cabeza;
    printf("Inventario (%d objetos):\n", inv->cantidad);
    while (actual != NULL) {
        printf(" [%d] %s (%s, valor %d)\n",
               actual->objeto.id,
               actual->objeto.nombre,
               tipo_a_texto(actual->objeto.tipo),
               actual->objeto.valor);
        actual = actual->siguiente;
    }
}


/* Busca un objeto por nombre en el inventario.
 * Parametros: inv - inventario donde buscar, nombre - nombre del objeto a buscar.
 * Retorna: puntero al nodo si se encuentra, NULL si no.
 */
NodoInventario *inventario_buscar_por_nombre(const Inventario *inv, const char *nombre) {
    NodoInventario *actual = inv->cabeza;
    while (actual != NULL) {
        if (strcmp(actual->objeto.nombre, nombre) == 0) {
            return actual;          /* encontrado */
        }
        actual = actual->siguiente;
    }
    return NULL;                    /* no encontrado */
}

/* Funcion para eliminar un objeto por id del inventario.
 * Parametros: inv - inventario donde eliminar el objeto, id - id del objeto a eliminar, eliminado - objeto eliminado (puede ser NULL).
 * Retorna: 0 si se elimino correctamente, -1 si hubo un error.
 */
int inventario_eliminar_por_id(Inventario *inv, int id, Objeto *eliminado) {
    if (inv->cabeza == NULL) {      /* caso: lista vacía */
        return -1;
    }

    NodoInventario *anterior = NULL;
    NodoInventario *actual = inv->cabeza;

    while (actual != NULL && actual->objeto.id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == NULL) {           /* recorrió todo y no estaba */
        return -1;
    }

    if (anterior == NULL) {         /* caso: es la cabeza (o único elemento) */
        inv->cabeza = actual->siguiente;
    } else {                        /* caso: nodo intermedio o último */
        anterior->siguiente = actual->siguiente;
    }

    if (eliminado != NULL) {
        *eliminado = actual->objeto;
    }
    free(actual);
    inv->cantidad--;
    return 0;
}

/* Libera toda la memoria del inventario.
 * Parametros: inv - inventario a liberar.
 * Retorna: nada.
 */
void inventario_liberar(Inventario *inv) {
    NodoInventario *actual = inv->cabeza;
    while (actual != NULL) {
        NodoInventario *siguiente = actual->siguiente;  /* guardar antes del free */
        free(actual);
        actual = siguiente;
    }
    inv->cabeza = NULL;
    inv->cantidad = 0;
}

/* Busca un objeto por ID en el inventario.
 * Parametros: inv - inventario donde buscar, id - ID del objeto a buscar.
 * Retorna: puntero al nodo si se encuentra, NULL si no.
 */
NodoInventario *inventario_buscar_por_id(const Inventario *inv, int id) {
    NodoInventario *actual = inv->cabeza;
    while (actual != NULL) {
        if (actual->objeto.id == id) {
            return actual;          /* encontrado */
        }
        actual = actual->siguiente;
    }
    return NULL;                    /* no encontrado */
}

/* Usa un objeto del inventario.
 * Parametros: inv - inventario donde buscar el objeto, id - ID del objeto a usar, p - personaje que lo usa.
 * Retorna: 0 si se uso correctamente, -1 si hubo un error.
 */
int inventario_usar_objeto(Inventario *inv, int id, Personaje *p) {
    NodoInventario *nodo = inventario_buscar_por_id(inv, id);
    if (nodo == NULL) {
        return -1;  /* objeto no encontrado */
    }

    Objeto obj = nodo->objeto;  /* copia: el nodo se libera si es pocion */

    switch (obj.tipo) {
        case POCION: {
            int recuperados = personaje_curar(p, obj.valor);
            printf("%s usa %s y recupera %d PV. Vida: %d/%d.\n", 
                p->nombre, obj.nombre, recuperados, p->vida, p->vida_max);
            inventario_eliminar_por_id(inv, id, NULL);
            printf("%s se elimina del inventario.\n", obj.nombre);
            break;
        }
        case ARMA:
            p->ataque += obj.valor;
            printf("Equipas %s. Ataque: %d.\n", obj.nombre, p->ataque);
            break;
        case ARMADURA:
            p->defensa += obj.valor;
            printf("Equipas %s. Defensa: %d.\n", obj.nombre, p->defensa);
            break;
        default:
            printf("%s no tiene efecto.\n", obj.nombre);
            break;
    }
    return 0;
}