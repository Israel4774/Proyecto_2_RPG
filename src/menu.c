/* Autor: Tu Nombre */
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "entrada.h"
#include "aventura.h"
#include "acciones.h"
#include "guardado.h"

#define RUTA_GUARDADO "saves/partida.txt"

/* Registra una accion generica en el historial.
 * Parametros: h - historial donde registrar, tipo - tipo de accion, texto - descripcion de la accion.
 */
static void registrar(Historial *h, TipoAccion tipo, const char *texto){
    Accion a;
    memset(&a, 0, sizeof(a));
    a.tipo = tipo;
    snprintf(a.descripcion, MAX_DESCRIPCION, "%s", texto);
    if (historial_push(h, a) != 0) {
        printf("Aviso: no hay memoria para registrar en el historial.\n");
    }
}

/* Traduce el resultado de descartar a un mensaje claro.
 * Parametros: resultado - resultado de la operacion, que - descripcion del objeto.
 */
static void informar_descarte(int resultado, const char *que){
    if (resultado == ACCION_OK) {
        printf("Se descarto %s. Puedes deshacerlo desde el historial.\n", que);
    } else if (resultado == ACCION_ERR_MEMORIA) {
        printf("No hay memoria para registrar la accion. No se descarto nada.\n");
    } else {
        printf("No existe un objeto '%s' en el inventario.\n", que);
    }
}

/* Usa o equipa un objeto por ID y lo registra. 
 * Parametros: p - personaje, inv - inventario, h - historial.*/
static void opcion_usar(Personaje *p, Inventario *inv, Historial *h)
{
    int id = 0;
    int lectura = entrada_leer_entero("ID del objeto > ", &id);
    if (lectura != 0) {
        if (lectura == -1) {
            printf("ID invalido.\n");
        }
        return;
    }
    NodoInventario *nodo = inventario_buscar_por_id(inv, id);
    if (nodo == NULL) {
        printf("No existe un objeto con ID %d.\n", id);
        return;
    }
    char texto[MAX_DESCRIPCION];
    snprintf(texto, sizeof(texto), "El personaje uso %s", nodo->objeto.nombre);
    if (inventario_usar_objeto(inv, id, p) == 0) {
        registrar(h, ACCION_USAR_OBJETO, texto);
    }
}

/* Busca por nombre y muestra el resultado.
 * Parametros: inv - inventario donde buscar.
 */
static void opcion_buscar(const Inventario *inv){
    char nombre[MAX_NOMBRE];
    if (entrada_leer_texto("Nombre > ", nombre, sizeof(nombre)) != 0) {
        return;
    }
    NodoInventario *nodo = inventario_buscar_por_nombre(inv, nombre);
    if (nodo == NULL) {
        printf("No existe un objeto llamado '%s'.\n", nombre);
    } else {
        printf("Encontrado: [%d] %s (valor %d)\n",
               nodo->objeto.id, nodo->objeto.nombre, nodo->objeto.valor);
    }
}

/* Elimina un objeto por ID o por nombre.
 * Parametros: inv - inventario donde eliminar, h - historial donde registrar, por_id - 1 para eliminar por ID, 
   0 para eliminar por nombre.
 */
static void opcion_eliminar(Inventario *inv, Historial *h, int por_id){
    if (por_id) {
        int id = 0;
        int lectura = entrada_leer_entero("ID del objeto > ", &id);
        if (lectura != 0) {
            if (lectura == -1) {
                printf("ID invalido.\n");
            }
            return;
        }
        char texto[16];
        snprintf(texto, sizeof(texto), "ID %d", id);
        informar_descarte(descartar_objeto_por_id(inv, h, id), texto);
    } else {
        char nombre[MAX_NOMBRE];
        if (entrada_leer_texto("Nombre > ", nombre, sizeof(nombre)) != 0) {
            return;
        }
        informar_descarte(descartar_objeto_por_nombre(inv, h, nombre), nombre);
    }
}

/* Submenu del inventario.
 * Parametros: p - personaje, inv - inventario, h - historial.
 */
static void menu_inventario(Personaje *p, Inventario *inv, Historial *h)
{
    int opcion = 0;
    do {
        printf("\n===== INVENTARIO =====\n");
        printf("1. Listar objetos\n2. Buscar por nombre\n3. Usar o equipar por ID\n");
        printf("4. Eliminar por ID\n5. Eliminar por nombre\n6. Volver\n");
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2) {
            return;
        }
        if (lectura == -1) {
            printf("Entrada invalida. Escribe un numero del 1 al 6.\n");
            continue;
        }
        switch (opcion) {
            case 1: inventario_listar(inv); break;
            case 2: opcion_buscar(inv); break;
            case 3: opcion_usar(p, inv, h); break;
            case 4: opcion_eliminar(inv, h, 1); break;
            case 5: opcion_eliminar(inv, h, 0); break;
            case 6: break;
            default: printf("Opcion no valida.\n"); break;
        }
    } while (opcion != 6);
}

/* Intenta deshacer la ultima accion y explica el resultado. 
 * Parametros: inv - inventario, h - historial.
 */
static void opcion_deshacer(Inventario *inv, Historial *h)
{
    switch (deshacer_ultima_accion(inv, h)) {
        case ACCION_OK:
            printf("Se deshizo el descarte: el objeto volvio al inventario.\n");
            break;
        case ACCION_ERR_VACIO:
            printf("No hay acciones para deshacer.\n");
            break;
        case ACCION_ERR_NO_DESHACER:
            printf("La ultima accion no se puede deshacer (solo descartar objetos).\n");
            break;
        case ACCION_ERR_ID_REPETIDO:
            printf("No se puede deshacer: ya existe otro objeto con ese ID.\n");
            break;
        default:
            printf("No hay memoria para deshacer la accion.\n");
            break;
    }
}

/* Submenu del historial. 
 * Parametros: inv - inventario, h - historial.
 */
static void menu_historial(Inventario *inv, Historial *h){
    int opcion = 0;
    do {
        printf("\n===== HISTORIAL =====\n");
        printf("1. Ver historial\n2. Deshacer ultima accion\n3. Volver\n");
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2) {
            return;
        }
        if (lectura == -1) {
            printf("Entrada invalida. Escribe un numero del 1 al 3.\n");
            continue;
        }
        switch (opcion) {
            case 1: historial_mostrar(h); break;
            case 2: opcion_deshacer(inv, h); break;
            case 3: break;
            default: printf("Opcion no valida.\n"); break;
        }
    } while (opcion != 3);
}

/* Explora una zona; el fin de entrada se trata como salir. 
 * Parametros: p - personaje, inv - inventario, h - historial.
 */
static int opcion_explorar(Personaje *p, Inventario *inv, Historial *h){
    if (aventura_explorar(p, inv, h) != 0) {
        printf("La exploracion se interrumpio.\n");
        return -1;
    }
    return 0;
}

void menu_principal(Personaje *p, Inventario *inv, Historial *h){
    int opcion = 0;
    do {
        printf("\n===== MENU DE JUEGO =====\n");
        printf("Partida: personaje %s\n", p->nombre);
        printf("1. Ver personaje\n2. Explorar zona\n3. Inventario\n4. Historial\n");
        printf("5. Guardar partida\n6. Cargar partida\n7. Salir\n");
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2) {
            return;
        }
        if (lectura == -1) {
            printf("Entrada invalida. Escribe un numero del 1 al 7.\n");
            continue;
        }
        switch (opcion) {
            case 1: personaje_mostrar(p); break;
            case 2:
                if (opcion_explorar(p, inv, h) != 0) {
                    return;
                }
                break;
            case 3: menu_inventario(p, inv, h); break;
            case 4: menu_historial(inv, h); break;
            case 5:
                printf(guardado_guardar(RUTA_GUARDADO, p, inv, h) == 0
                       ? "Partida guardada en " RUTA_GUARDADO ".\n"
                       : "No se pudo guardar la partida.\n");
                break;
            case 6:
                printf(guardado_cargar(RUTA_GUARDADO, p, inv, h) == 0
                       ? "Partida cargada: %s.\n"
                       : "No se pudo cargar: el archivo no existe o esta danado.\n", p->nombre);
                break;
            case 7: printf("Hasta luego!\n"); break;
            default: printf("Opcion no valida.\n"); break;
        }
    } while (opcion != 7);
}