#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventario.h"

// Funcion auxiliar para convertir el tipo de objeto a texto.
static const char *tipo_a_texto(TipoObjeto tipo) {
    switch (tipo) {
        case ARMA:     return "arma";
        case ARMADURA: return "armadura";
        case POCION:   return "pocion";
        default:       return "otro";
    }
}

// Funciones del inventario
void inventario_iniciar(Inventario *inv) {
    inv->cabeza = NULL;
    inv->cantidad = 0;
}

// Funcion para agregar un objeto al inventario.
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

//Funcion para listar todos los objetos del inventario.
void inventario_listar(const Inventario *inv) {
    if (inv->cabeza == NULL) {
        printf("El inventario está vacío.\n");
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


//Funcion para buscar un objeto por nombre en el inventario.
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

//Funcion para eliminar un objeto por id del inventario.
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

//Funcion para liberar toda la memoria del inventario.
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