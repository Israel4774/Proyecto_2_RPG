#ifndef PERSONAJE_H
#define PERSONAJE_H

#define MAX_NOMBRE_PERSONAJE 50

/* Estructura que representa un personaje en el juego */
typedef struct {
    char nombre[MAX_NOMBRE_PERSONAJE];
    int vida;
    int vida_max;
    int ataque;
    int defensa;
} Personaje;

/* Crea un personaje con stats iniciales. Retorna 0 ok, -1 si el nombre es NULL o vacio. */
int  personaje_crear(Personaje *p, const char *nombre);

/* Imprime: Aria: 30/30 PV | Ataque 8 | Defensa 2 */
void personaje_mostrar(const Personaje *p);

/* Resta (daño - defensa) a la vida, minimo 0 de dano y de vida. Retorna la vida restante. */
int  personaje_recibir_dano(Personaje *p, int dano);

/* Suma vida sin superar vida_max. Retorna los PV realmente recuperados. */
int  personaje_curar(Personaje *p, int cantidad);

#endif