/* Autor: Tu Nombre */
#ifndef ENEMIGO_H
#define ENEMIGO_H

#define MAX_NOMBRE_ENEMIGO 50


/* Estructura que representa a un enemigo en el juego. */
typedef struct {
    int id;                       /* numero de aparicion: 1, 2, 3... */
    char nombre[MAX_NOMBRE_ENEMIGO];
    int vida;
    int vida_max;
    int dano;
    int turnos_dormido;           /* 0 si esta despierto */
} Enemigo;

/* Crea un enemigo despierto y con vida completa. Retorna 0 ok, -1 si los datos son invalidos. */
int  enemigo_crear(Enemigo *e, int id, const char *nombre, int vida, int dano);

/* Imprime las stats del enemigo". */
void enemigo_mostrar(const Enemigo *e);

/* Resta daño a la vida (nunca baja de 0). Retorna la vida restante. */
int  enemigo_recibir_dano(Enemigo *e, int dano);

/* Retorna 1 si el enemigo tiene vida, 0 si no. */
int  enemigo_esta_vivo(const Enemigo *e);

/* Duerme al enemigo N turnos. Retorna 0 ok, -1 si turnos no es positivo. */
int  enemigo_dormir(Enemigo *e, int turnos);

#endif