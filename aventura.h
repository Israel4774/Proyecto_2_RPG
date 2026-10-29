#ifndef AVENTURA_H
#define AVENTURA_H

#include "personaje.h"
#include "inventario.h"
#include "historial.h"

/*
 * Explora una zona hasta encontrar un objeto o un grupo de enemigos.
 * Retorno: 0 si termina normalmente, -1 si hay parámetros inválidos
 * o un error de memoria/entrada.
 */
int aventura_explorar(Personaje *personaje,
                      Inventario *inventario,
                      Historial *historial);

#endif
