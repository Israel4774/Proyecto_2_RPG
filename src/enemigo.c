/* Autor: Tu Nombre */
#include <stdio.h>
#include <string.h>
#include "enemigo.h"

/*Crea un enemigo despierto y con vida completa. Retorna 0 ok, -1 si los datos son invalidos.
 *Parametros: e - enemigo a inicializar, id - numero de aparicion, nombre - nombre del enemigo, 
  vida - vida inicial, daño - daño que inflige.
*/
int enemigo_crear(Enemigo *e, int id, const char *nombre, int vida, int dano) {
    if (e == NULL || nombre == NULL || nombre[0] == '\0' || vida <= 0 || dano < 0) {
        return -1;
    }
    e->id = id;
    strncpy(e->nombre, nombre, MAX_NOMBRE_ENEMIGO - 1);
    e->nombre[MAX_NOMBRE_ENEMIGO - 1] = '\0';   /* strncpy no siempre pone el terminador */
    e->vida = vida;
    e->vida_max = vida;
    e->dano = dano;
    e->turnos_dormido = 0;
    return 0;
}

/* Muestra al enemigo 
 * Parametros: e - enemigo a mostrar.
 */
void enemigo_mostrar(const Enemigo *e) {
    printf(" %d. %s (%d PV)\n", e->id, e->nombre, e->vida);
}

/* Resta daño a la vida (nunca baja de 0). Retorna la vida restante.
 * Parametros: e - enemigo, dano - cantidad de daño a restar.
 * Retorna: vida restante.
 */
int enemigo_recibir_dano(Enemigo *e, int dano) {
    if (dano > 0) {
        e->vida -= dano;
    }
    if (e->vida < 0) {
        e->vida = 0;
    }
    return e->vida;
}

/* Verifica si el enemigo tiene vida, retorna 1 si tiene vida, 0 si no.
 * Parametros: e - enemigo a verificar.
 * Retorna: 1 si esta vivo, 0 si no.
 */
int enemigo_esta_vivo(const Enemigo *e) {
    return e->vida > 0;
}

/* Duerme al enemigo N turnos. Retorna 0 ok, -1 si turnos no es positivo.
 * Parametros: e - enemigo a dormir, turnos - cantidad de turnos para dormir.
 * Retorna: 0 si se duro correctamente, -1 si los parametros son invalidos.
 */
int enemigo_dormir(Enemigo *e, int turnos) {
    if (turnos <= 0) {
        return -1;
    }
    e->turnos_dormido = turnos;
    return 0;
}


