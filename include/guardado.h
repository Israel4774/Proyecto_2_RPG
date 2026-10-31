#ifndef GUARDADO_H
#define GUARDADO_H

#include "personaje.h"
#include "inventario.h"
#include "historial.h"

/*
 * Guarda personaje, inventario e historial en un archivo de texto.
 * Retorna 0 si funciona o -1 si ocurre un error.
 */
int guardado_guardar(const char *ruta, const Personaje *personaje,const Inventario *inventario,const Historial *historial);

/*
 * Carga personaje, inventario e historial desde un archivo de texto.
 * Los destinos deben estar inicializados antes de llamar.
 * Retorna 0 si funciona o -1 si ocurre un error.
 */
int guardado_cargar(const char *ruta,Personaje *personaje,Inventario *inventario, Historial *historial);

#endif
