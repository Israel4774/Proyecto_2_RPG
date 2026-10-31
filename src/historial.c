/* Autor: Tu Nombre */
/*
 * Archivo: historial.c
 * Descripcion: pila enlazada para registrar acciones del RPG.
 */
#include <stdio.h>
#include <stdlib.h>
#include "historial.h"

/* Inicializa la pila vacia. Si h es NULL, no hace nada.
 * Parametros: h - historial a inicializar.
 * Retorna: nada.
 */
void historial_iniciar(Historial *h)
{
    if (h == NULL) {
        return;
    }
    h->top = NULL;
    h->cantidad = 0;
}

/* Agrega una accion al tope. Retorna 0 si funciona, -1 si falla. 
 * Parametros: h - historial donde agregar la accion, accion - accion a agregar.
 * Retorna: 0 si se agrego correctamente, -1 si hubo un error.
 */
int historial_push(Historial *h, Accion accion)
{
    NodoHistorial *nuevo;

    if (h == NULL) {
        return -1;
    }

    nuevo = malloc(sizeof(*nuevo));
    if (nuevo == NULL) {
        return -1;
    }

    nuevo->accion = accion;
    nuevo->siguiente = h->top;
    h->top = nuevo;
    h->cantidad++;
    return 0;
}

/* Retira la accion superior y libera su nodo. Retorna 0 o -1.
 * Parametros: h - historial donde retirar la accion, accion - accion donde guardar la accion retirada.
 * Retorna: 0 si se retiro correctamente, -1 si hubo un error.
 */
int historial_pop(Historial *h, Accion *accion)
{
    NodoHistorial *eliminado;

    if (h == NULL || accion == NULL || h->top == NULL) {
        return -1;
    }

    eliminado = h->top;
    *accion = eliminado->accion;
    h->top = eliminado->siguiente;
    free(eliminado);
    h->cantidad--;
    return 0;
}

/* Copia la accion superior sin modificar la pila. Retorna 0 o -1.
 * Parametros: h - historial donde copiar la accion, accion - accion donde guardar la copia.
 * Retorna: 0 si se copio correctamente, -1 si hubo un error.
 */
int historial_peek(const Historial *h, Accion *accion)
{
    if (h == NULL || accion == NULL || h->top == NULL) {
        return -1;
    }

    *accion = h->top->accion;
    return 0;
}

/* Devuelve 1 si esta vacia, 0 si no; -1 si h es NULL. 
 * Parametros: h - historial a verificar.
 * Retorna: 1 si esta vacia, 0 si no, -1 si h es NULL.
 */
int historial_esta_vacio(const Historial *h)
{
    if (h == NULL) {
        return -1;
    }
    return h->top == NULL;
}

/* Muestra las acciones desde la mas reciente a la mas antigua.
 * Parametros: h - historial a mostrar.
 * Retorna: nada.
 */
void historial_mostrar(const Historial *h)
{
    const NodoHistorial *actual;

    if (h == NULL) {
        printf("Error: historial invalido.\n");
        return;
    }
    if (h->top == NULL) {
        printf("El historial esta vacio.\n");
        return;
    }

    printf("\n===== HISTORIAL =====\n");
    actual = h->top;
    while (actual != NULL) {
        printf("- %s\n", actual->accion.descripcion);
        actual = actual->siguiente;
    }
}

/* Libera todos los nodos y deja el historial vacio. 
 * Parametros: h - historial a liberar.
 * Retorna: nada.
 */
void historial_liberar(Historial *h)
{
    NodoHistorial *actual;
    NodoHistorial *siguiente;

    if (h == NULL) {
        return;
    }

    actual = h->top;
    while (actual != NULL) {
        siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }

    h->top = NULL;
    h->cantidad = 0;
}