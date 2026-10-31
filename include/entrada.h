/* Autor: Tu Nombre */
#ifndef ENTRADA_H
#define ENTRADA_H

/*
 * Lee un entero desde la entrada estandar (una linea completa).
 * Retorna: 0 si se leyo un entero valido, -1 si el texto no es un numero,
 *          -2 si se acabo la entrada.
 */
int entrada_leer_entero(const char *mensaje, int *valor);

/*
 * Lee una linea de texto sin el salto de linea.
 * Retorna: 0 si salio bien, -1 si se acabo la entrada.
 */
int entrada_leer_texto(const char *mensaje, char *destino, int tam);

/*
 * Pregunta hasta recibir 's' o 'n'.
 * Retorna: 1 si responde s, 0 si responde n, -1 si se acabo la entrada.
 */
int entrada_preguntar_si_no(const char *mensaje);

#endif