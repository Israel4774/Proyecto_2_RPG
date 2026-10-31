/* Autor: tu nombre */
#include <stdio.h>
#include <string.h>
#include "personaje.h"

/* Crea un personaje con stats iniciales. Retorna 0 ok, -1 si el nombre es NULL o vacio. 
 * Parametros: p - personaje a crear, nombre - nombre del personaje.
 */
int personaje_crear(Personaje *p, const char *nombre) {
    if (p == NULL || nombre == NULL || nombre[0] == '\0') {
        return -1;
    }
    strncpy(p->nombre, nombre, MAX_NOMBRE_PERSONAJE - 1);
    p->nombre[MAX_NOMBRE_PERSONAJE - 1] = '\0';
    p->vida = 30;
    p->vida_max = 30;
    p->ataque = 8;
    p->defensa = 2;
    return 0;
}

/* Muestra los stats del personaje. 
 * Parametros: p - personaje a mostrar.
 */
void personaje_mostrar(const Personaje *p) { 
    printf("%s: %d/%d PV | Ataque %d | Defensa %d\n", p->nombre, p->vida, p->vida_max, p->ataque, p->defensa);
}

/* Resta vida segun el dano recibido. Retorna la vida restante. 
 * Parametros: p - personaje, dano - cantidad de dano recibido.
 */
int personaje_recibir_dano(Personaje *p, int dano) {   
    int dano_real = dano - p->defensa;
    if (dano_real < 0) {
        dano_real = 0;
    }
    p->vida -= dano_real;
    
    if (p->vida < 0) {
        p->vida = 0;
    }   
    return p->vida;
}

/* Cura al personaje. Retorna los PV realmente recuperados. 
 * Parametros: p - personaje, cantidad - cantidad de PV a recuperar.
 */
int personaje_curar(Personaje *p, int cantidad) {   
    int vida_antes = p->vida;
    p->vida += cantidad;
    if (p->vida > p->vida_max) {
        p->vida = p->vida_max;
    }
    return p->vida - vida_antes;
}

