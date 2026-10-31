/* Autor: Tu Nombre */
#ifndef AVENTURA_H
#define AVENTURA_H

#include "personaje.h"
#include "inventario.h"
#include "historial.h"

/*Explora una zona y permite al jugador interactuar con el entorno. 
    Retorna: 0 si el jugador sale de la zona o si el personaje muere,
            -1 si hay error (parametros invalidos, falta de memoria o se acaba la entrada).
             */

int aventura_explorar(Personaje *personaje,
                      Inventario *inventario,
                      Historial *historial);

#endif