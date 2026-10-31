/* Autor: <tu nombre> */
#ifndef INVENTARIO_H
#define INVENTARIO_H

#include "objeto.h"
#include "personaje.h"


typedef struct NodoInventario {
    Objeto objeto;
    struct NodoInventario *siguiente;
} NodoInventario;

typedef struct {
    NodoInventario *cabeza;
    int cantidad;
} Inventario;

NodoInventario *inventario_buscar_por_nombre(const Inventario *inv, const char *nombre);
NodoInventario *inventario_buscar_por_id(const Inventario *inv, int id);

void inventario_iniciar(Inventario *inv);
int  inventario_agregar(Inventario *inv, Objeto obj);
int  inventario_eliminar_por_id(Inventario *inv, int id, Objeto *eliminado);
void inventario_listar(const Inventario *inv);
void inventario_liberar(Inventario *inv);
int inventario_usar_objeto(Inventario *inv, int id, Personaje *p);







#endif
