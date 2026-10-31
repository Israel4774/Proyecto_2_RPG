/* Autor: Tu Nombre */
/*
 * Archivo: aventura.c
 * Descripcion: exploracion por pasos con eventos aleatorios.
 * Base: version de B, conectada al combate (cola de enemigos), con eleccion
 * de zona, objetos variados con ID unico y sin srand repetido.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aventura.h"
#include "combate.h"
#include "entrada.h"

#define PASOS_POR_EVENTO 3

typedef struct {
    const char *nombre;
    TipoObjeto tipo;
    int valor;
} PlantillaObjeto;

typedef struct {
    const char *nombre;
    int vida;
    int dano;
} PlantillaEnemigo;

typedef struct {
    const char *nombre;
    const char *dificultad;
    PlantillaEnemigo enemigos[2];
} Zona;

static const Zona ZONAS[] = {
    {"Bosque Sombrio", "baja",  {{"Goblin", 15, 4},      {"Lobo", 20, 6}}},
    {"Cueva Humeda",   "media", {{"Murcielago", 18, 6},  {"Troll", 30, 8}}},
    {"Ruinas Antiguas", "alta", {{"Esqueleto", 25, 8},   {"Golem", 40, 10}}}
};
#define NUM_ZONAS ((int)(sizeof(ZONAS) / sizeof(ZONAS[0])))

static const PlantillaObjeto HALLAZGOS[] = {
    {"Espada corta", ARMA, 5},
    {"Daga", ARMA, 3},
    {"Escudo de madera", ARMADURA, 2},
    {"Pocion menor", POCION, 20},
    {"Somnifero", OTRO, 2}
};
#define NUM_HALLAZGOS ((int)(sizeof(HALLAZGOS) / sizeof(HALLAZGOS[0])))

static const char *const MENSAJES_PASO[] = {
    "Caminas entre los arboles.",
    "Sigues por el sendero.",
    "Cruzas un pequeno arroyo."
};
#define NUM_MENSAJES ((int)(sizeof(MENSAJES_PASO) / sizeof(MENSAJES_PASO[0])))

/* Registra un mensaje en el historial. Retorna 0 ok, -1 error. */
static int registrar_evento(Historial *historial, TipoAccion tipo, const char *mensaje)
{
    Accion accion;

    memset(&accion, 0, sizeof(accion));
    accion.tipo = tipo;
    snprintf(accion.descripcion, sizeof(accion.descripcion), "%s", mensaje);
    return historial_push(historial, accion);
}

/* Imprime un objeto como: " [3] Espada corta (arma, +5 ataque)". */
static void describir_objeto(const Objeto *o)
{
    switch (o->tipo) {
        case ARMA:     printf(" [%d] %s (arma, +%d ataque)\n", o->id, o->nombre, o->valor); break;
        case ARMADURA: printf(" [%d] %s (armadura, +%d defensa)\n", o->id, o->nombre, o->valor); break;
        case POCION:   printf(" [%d] %s (pocion, +%d vida)\n", o->id, o->nombre, o->valor); break;
        default:       printf(" [%d] %s (otro, duerme al enemigo %d turnos)\n", o->id, o->nombre, o->valor); break;
    }
}

/* Genera un ID entre 100 y 999 que no exista en el inventario. */
static int generar_id_libre(const Inventario *inventario)
{
    int id;
    do {
        id = 100 + rand() % 900;
    } while (inventario_buscar_por_id(inventario, id) != NULL);
    return id;
}

/*
 * Muestra las zonas y pide una.
 * Retorna: 1..NUM_ZONAS la zona elegida, 0 si vuelve atras, -1 si se acaba la entrada.
 */
static int elegir_zona(void)
{
    for (;;) {
        int opcion = 0;

        printf("\nZonas disponibles:\n");
        for (int i = 0; i < NUM_ZONAS; i++) {
            printf("%d. %s (dificultad %s)\n", i + 1, ZONAS[i].nombre, ZONAS[i].dificultad);
        }
        printf("0. Volver\n");
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2) {
            return -1;
        }
        if (lectura == 0 && opcion >= 0 && opcion <= NUM_ZONAS) {
            return opcion;
        }
        printf("Opcion invalida. Escribe un numero del 0 al %d.\n", NUM_ZONAS);
    }
}

/* Evento: objeto en el suelo. Retorna 0 ok, -1 si falla la memoria o se acaba la entrada. */
static int evento_objeto(Inventario *inventario, Historial *historial)
{
    const PlantillaObjeto *plantilla = &HALLAZGOS[rand() % NUM_HALLAZGOS];
    Objeto objeto;
    char texto[MAX_DESCRIPCION];

    memset(&objeto, 0, sizeof(objeto));
    objeto.id = generar_id_libre(inventario);
    objeto.tipo = plantilla->tipo;
    objeto.valor = plantilla->valor;
    snprintf(objeto.nombre, sizeof(objeto.nombre), "%s", plantilla->nombre);

    printf("Encuentras un objeto en el suelo!\n");
    describir_objeto(&objeto);
    snprintf(texto, sizeof(texto), "Se encontro %s", objeto.nombre);
    if (registrar_evento(historial, ACCION_ENCONTRAR_OBJETO, texto) != 0) {
        printf("Advertencia: no se pudo registrar en el historial.\n");
    }

    int respuesta = entrada_preguntar_si_no("Deseas recogerlo? (s/n) > ");
    if (respuesta < 0) {
        return -1;
    }
    if (respuesta == 0) {
        printf("Dejas %s en el suelo.\n", objeto.nombre);
        return 0;
    }
    if (inventario_agregar(inventario, objeto) != 0) {
        printf("No se pudo agregar: memoria insuficiente.\n");
        return -1;
    }
    printf("%s agregado al inventario.\n", objeto.nombre);
    return 0;
}

/*
 * Evento: grupo de enemigos. Arma la cola (el numero de aparicion define el
 * orden) y llama al combate.
 * Retorna: 1 si gana, 0 si muere el personaje, -1 si hay error o se acaba la entrada.
 */
static int evento_enemigos(const Zona *zona, Personaje *personaje,
                           Inventario *inventario, Historial *historial)
{
    ColaCombate cola;
    int cantidad = 2 + rand() % 2;
    int resultado;

    cola_iniciar(&cola);
    printf("Un grupo de enemigos te bloquea el paso!\nEnemigos:\n");
    for (int i = 1; i <= cantidad; i++) {
        const PlantillaEnemigo *pl = &zona->enemigos[rand() % 2];
        Enemigo e;

        if (enemigo_crear(&e, i, pl->nombre, pl->vida, pl->dano) != 0 ||
            cola_encolar(&cola, e) != 0) {
            printf("No hay memoria para crear los enemigos.\n");
            cola_liberar(&cola);
            return -1;
        }
        enemigo_mostrar(&e);
    }
    resultado = combate_iniciar(personaje, inventario, historial, &cola);
    cola_liberar(&cola);                        /* libera los que no se enfrentaron */
    return resultado;
}

int aventura_explorar(Personaje *personaje, Inventario *inventario, Historial *historial)
{
    int pasos = 0;
    int zona_elegida;

    if (personaje == NULL || inventario == NULL || historial == NULL) {
        return -1;
    }
    if (personaje->vida <= 0) {
        printf("%s no tiene vida para explorar.\n", personaje->nombre);
        return 0;
    }
    zona_elegida = elegir_zona();
    if (zona_elegida <= 0) {
        return zona_elegida;
    }
    const Zona *zona = &ZONAS[zona_elegida - 1];
    printf("Entras a %s...\n", zona->nombre);

    for (;;) {
        int evento;
        int resultado = 0;

        pasos++;
        printf("%s\n", MENSAJES_PASO[(pasos - 1) % NUM_MENSAJES]);
        if (pasos % PASOS_POR_EVENTO != 0) {
            printf("No encuentras nada.\n");
            continue;
        }
        evento = rand() % 3;                    /* 0 nada, 1 objeto, 2 enemigos */
        if (evento == 0) {
            printf("No ocurre nada especial.\n");
            continue;
        }
        if (evento == 1) {
            resultado = evento_objeto(inventario, historial) == 0 ? 1 : -1;
        } else {
            resultado = evento_enemigos(zona, personaje, inventario, historial);
        }
        if (resultado < 0) {
            return -1;
        }
        if (resultado == 0) {
            printf("Fin de la aventura: %s no sobrevivio.\n", personaje->nombre);
            return 0;
        }
        int seguir = entrada_preguntar_si_no("Seguir explorando? (s/n) > ");
        if (seguir < 0) {
            return -1;
        }
        if (seguir == 0) {
            return 0;
        }
    }
}