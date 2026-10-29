/*
 * Archivo: aventura.c
 * Descripción: exploración por pasos con eventos aleatorios.
 *
 * Nota de integración: esta versión crea eventos sencillos y aleatorios.
 * El encuentro con enemigos se registra en el historial; la llamada al
 * sistema de combate debe ajustarse cuando el compañero confirme la firma
 * definitiva de combate_iniciar() y el formato de los enemigos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "aventura.h"

/* Registra un mensaje genérico en el historial. */
static int registrar_evento(Historial *historial, TipoAccion tipo,
                            const char *mensaje)
{
    Accion accion;

    if (historial == NULL || mensaje == NULL) {
        return -1;
    }

    memset(&accion, 0, sizeof(accion));
    accion.tipo = tipo;
    (void)snprintf(accion.descripcion, sizeof(accion.descripcion),
                   "%s", mensaje);

    return historial_push(historial, accion);
}

/*
 * Propósito: explorar hasta encontrar un objeto o un grupo de enemigos.
 * Parámetros: personaje, inventario e historial válidos.
 * Retorno: 0 si termina normalmente; -1 ante parámetros inválidos o error.
 * Errores: valida punteros, entrada y reserva de memoria del inventario.
 */
int aventura_explorar(Personaje *personaje,
                      Inventario *inventario,
                      Historial *historial)
{
    int pasos = 0;
    int seguir = 1;

    if (personaje == NULL || inventario == NULL || historial == NULL) {
        return -1;
    }

    (void)personaje;

    /* Semilla única por ejecución; srand también puede hacerse en main. */
    srand((unsigned int)time(NULL));

    printf("\\n===== EXPLORACIÓN =====\\n");
    printf("Comienzas a explorar la zona.\\n");

    while (seguir) {
        int evento;

        pasos++;
        printf("Paso %d: avanzas por la zona.\\n", pasos);

        /* Cada tercer paso se genera un evento. */
        if (pasos % 3 != 0) {
            printf("No encuentras nada.\\n");
            continue;
        }

        evento = rand() % 3;

        if (evento == 0) {
            printf("No ocurre nada especial.\\n");
            continue;
        }

        if (evento == 1) {
            Objeto objeto;
            char respuesta[16];

            memset(&objeto, 0, sizeof(objeto));
            objeto.id = rand() % 9000 + 1000;
            objeto.tipo = POCION;
            objeto.valor = 10;
            (void)snprintf(objeto.nombre, sizeof(objeto.nombre),
                           "Pocion_%d", objeto.id);

            printf("¡Encuentras un objeto! [%d] %s (+%d)\\n",
                   objeto.id, objeto.nombre, objeto.valor);
            printf("¿Deseas recogerlo? (s/n): ");

            if (fgets(respuesta, sizeof(respuesta), stdin) == NULL) {
                return -1;
            }

            if (respuesta[0] == 's' || respuesta[0] == 'S') {
                if (inventario_agregar(inventario, objeto) != 0) {
                    printf("No se pudo agregar: memoria insuficiente.\\n");
                    return -1;
                }
                if (registrar_evento(historial, ACCION_ENCONTRAR_OBJETO,
                                     "Se encontró un objeto") != 0) {
                    printf("Advertencia: no se pudo registrar el historial.\\n");
                }
                printf("Objeto agregado al inventario.\\n");
            }

            seguir = 0;
        } else {
            if (registrar_evento(historial, ACCION_OTRA,
                                 "Se encontró un grupo de enemigos") != 0) {
                printf("Advertencia: no se pudo registrar el historial.\\n");
            }
            printf("¡Un grupo de enemigos te bloquea el paso!\\n");
            printf("La integración con combate se conectará al confirmar "
                   "la interfaz del módulo de combate.\\n");
            seguir = 0;
        }
    }

    return 0;
}
