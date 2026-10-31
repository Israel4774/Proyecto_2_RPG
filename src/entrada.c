/* Autor: Tu Nombre */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "entrada.h"

/* Descarta lo que quede de una linea mas larga que el buffer.
 * Parametros: buffer - linea de entrada.
 */
static void descartar_resto_de_linea(const char *buffer) {
    if (strchr(buffer, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
}

/* Lee un texto desde la entrada estandar.
 * Parametros: mensaje - mensaje a mostrar, destino - buffer para almacenar el texto, tam - tamaño del buffer.
 * Retorna: 0 si se leyo correctamente, -1 si hubo un error.
 */
int entrada_leer_texto(const char *mensaje, char *destino, int tam) {
    printf("%s", mensaje);
    if (fgets(destino, tam, stdin) == NULL) {
        return -1;
    }
    descartar_resto_de_linea(destino);
    destino[strcspn(destino, "\n")] = '\0';
    return 0;
}

/* Lee un entero desde la entrada estandar.
 * Parametros: mensaje - mensaje a mostrar, valor - puntero al entero donde almacenar el valor leido.
 * Retorna: 0 si se leyo correctamente, -1 si hubo un error.
 */
int entrada_leer_entero(const char *mensaje, int *valor) {
    char buffer[32];
    char *fin;

    printf("%s", mensaje);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -2;
    }
    descartar_resto_de_linea(buffer);
    long numero = strtol(buffer, &fin, 10);
    if (fin == buffer) {
        return -1;                      /* no hay digitos */
    }
    while (*fin == ' ' || *fin == '\t') {
        fin++;
    }
    if (*fin != '\n' && *fin != '\0') {
        return -1;                      /* basura despues del numero, ej: "12abc" */
    }
    if (numero < -1000000 || numero > 1000000) {
        return -1;
    }
    *valor = (int)numero;
    return 0;
}

/* Pregunta al usuario una pregunta de si/no.
 * Parametros: mensaje - mensaje a mostrar.
 * Retorna: 1 si responde si, 0 si responde no, -1 si hubo un error.
 */
int entrada_preguntar_si_no(const char *mensaje) {
    char buffer[16];

    for (;;) {
        if (entrada_leer_texto(mensaje, buffer, sizeof(buffer)) != 0) {
            return -1;
        }
        if (tolower((unsigned char)buffer[0]) == 's' && buffer[1] == '\0') {
            return 1;
        }
        if (tolower((unsigned char)buffer[0]) == 'n' && buffer[1] == '\0') {
            return 0;
        }
        printf("Responde s o n.\n");
    }
}