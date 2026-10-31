#ifndef OBJETO_H
#define OBJETO_H

#define MAX_NOMBRE 50

/* Tipos de objetos que se pueden encontrar en el juego */
typedef enum { 
    ARMA, 
    ARMADURA, 
    POCION, 
    OTRO 
} TipoObjeto;

/* Estructura que representa un objeto en el juego */
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    TipoObjeto tipo;
    int valor;
} Objeto;

#endif