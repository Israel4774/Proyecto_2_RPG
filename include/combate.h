/* Autor: Tu Nombre */
#ifndef COMBATE_H
#define COMBATE_H

#include "personaje.h"
#include "inventario.h"
#include "historial.h"
#include "cola_combate.h"

/*
 * Retorna: 1 si el personaje sobrevive y derrota a todos los enemigos,
 *          0 si el personaje muere en combate,
 *         -1 si hay error (parametros invalidos o falta de memoria).
*/
int combate_iniciar(Personaje *p, Inventario *inv, Historial *h, ColaCombate *enemigos);

#endif