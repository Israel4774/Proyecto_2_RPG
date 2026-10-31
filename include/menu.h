/* Autor: Tu Nombre */
#ifndef MENU_H
#define MENU_H

#include "personaje.h"
#include "inventario.h"
#include "historial.h"

/*Muestra el menu principal y atiende las opciones hasta que el usuario sale.*/
void menu_principal(Personaje *p, Inventario *inv, Historial *h);

#endif