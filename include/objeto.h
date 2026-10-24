#ifndef OBJETO_H
#define OBJETO_H

#define MAX_NOMBRE 50

typedef enum { ARMA, ARMADURA, POCION, OTRO } TipoObjeto;

typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    TipoObjeto tipo;
    int valor;
} Objeto;

#endif