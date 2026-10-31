/* Autor: Tu Nombre */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "personaje.h"
#include "inventario.h"
#include "historial.h"
#include "guardado.h"
#include "entrada.h"
#include "menu.h"

#define RUTA_GUARDADO "saves/partida.txt"

/* Entrega dos objetos iniciales a una partida nueva.
 * Parametros: inv - inventario donde agregar los objetos.
 */
static void cargar_objetos_iniciales(Inventario *inv){
    Objeto objetos[] = {
        {1, "Pocion menor", POCION, 20},
        {2, "Somnifero", OTRO, 2}
    };
    
    for (size_t i = 0; i < sizeof(objetos) / sizeof(objetos[0]); i++) {
        if (inventario_agregar(inv, objetos[i]) != 0) {
            printf("Aviso: no hay memoria para el objeto %s.\n", objetos[i].nombre);
        }
    }
}

/* Crea un personaje nuevo pidiendo el nombre. Retorna 0 ok, -1 si se acaba la entrada.
 * Parametros: p - personaje a crear, inv - inventario donde agregar los objetos.
 * Retorna: 0 si se creo correctamente, -1 si hubo un error.
 */
static int nueva_partida(Personaje *p, Inventario *inv){
    char nombre[MAX_NOMBRE_PERSONAJE];

    for (;;) {
        if (entrada_leer_texto("Nombre del personaje > ", nombre, sizeof(nombre)) != 0) {
            return -1;
        }
        if (personaje_crear(p, nombre) == 0) {
            break;
        }
        printf("El nombre no puede estar vacio.\n");
    }
    cargar_objetos_iniciales(inv);
    return 0;
}

/* Pantalla inicial. Retorna 0 si hay partida lista, -1 si el usuario sale. 
 * Parametros: p - personaje, inv - inventario, h - historial.
 * Retorna: 0 si hay partida lista, -1 si el usuario sale.
 */
static int pantalla_inicial(Personaje *p, Inventario *inv, Historial *h)
{
    for (;;) {
        int opcion = 0;
        printf("\n===== RPG DE TEXTO =====\n1. Nueva partida\n2. Cargar partida\n3. Salir\n");
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2 || (lectura == 0 && opcion == 3)) {
            return -1;
        }
        if (lectura == 0 && opcion == 1) {
            return nueva_partida(p, inv);
        }
        if (lectura == 0 && opcion == 2) {
            if (guardado_cargar(RUTA_GUARDADO, p, inv, h) == 0) {
                printf("Partida cargada: %s.\n", p->nombre);
                return 0;
            }
            printf("No se pudo cargar: el archivo no existe o esta danado.\n");
        } else {
            printf("Opcion invalida. Escribe 1, 2 o 3.\n");
        }
    }
}

int main(int argc, char **argv){   
    
    /* Funcion principal del juego. */
    Personaje personaje;
    Inventario inventario;
    Historial historial;
    int estado;

    /* Semilla unica; con ./rpg <numero> el azar es repetible (util para pruebas). */
    srand(argc > 1 ? (unsigned int)strtoul(argv[1], NULL, 10) : (unsigned int)time(NULL));

    inventario_iniciar(&inventario);
    historial_iniciar(&historial);
    memset(&personaje, 0, sizeof(personaje));

    estado = pantalla_inicial(&personaje, &inventario, &historial);
    
    if (estado == 0) {
        menu_principal(&personaje, &inventario, &historial);
    }

    inventario_liberar(&inventario);
    historial_liberar(&historial);
    return 0;
}