/* Autor: Tu Nombre */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "combate.h"
#include "entrada.h"

/* Registra una accion en el historial. Retorna 0 ok, -1 sin memoria.
 * Parametros: h - historial, tipo - tipo de accion, texto - descripcion de la accion.
  */
static int registrar_accion(Historial *h, TipoAccion tipo, const char *texto) {
    Accion a;
    memset(&a, 0, sizeof(a));
    a.tipo = tipo;
    snprintf(a.descripcion, MAX_DESCRIPCION, "%s", texto);
    return historial_push(h, a);
}

/* Muestra el estado del personaje y del enemigo actual.
 * Parametros: p - personaje, e - enemigo, en_espera - cantidad de enemigos restantes.
  */
static void mostrar_estado(const Personaje *p, const Enemigo *e, int en_espera) {
    printf("\n===== COMBATE =====\n");
    personaje_mostrar(p);
    printf("Enemigo actual: %s (%d/%d PV)\n", e->nombre, e->vida, e->vida_max);
    printf("Enemigos en espera: %d\n", en_espera);
    printf("1. Atacar\n2. Ver inventario\n3. Usar objeto\n");
}

/* El personaje ataca al enemigo.
 * Parametros: p - personaje, e - enemigo.
 */
static void atacar_enemigo(const Personaje *p, Enemigo *e) {
    enemigo_recibir_dano(e, p->ataque);
    printf("Atacas a %s: %d de dano. %s: %d/%d PV.\n",
           e->nombre, p->ataque, e->nombre, e->vida, e->vida_max);
}

/* Turno del enemigo: ataca, o pierde el turno si esta dormido.
 * Parametros: p - personaje, e - enemigo.
 */
static void turno_enemigo(Personaje *p, Enemigo *e) {
    if (e->turnos_dormido > 0) {
        e->turnos_dormido--;
        printf("%s esta dormido y no ataca.\n", e->nombre);
        return;
    }
    int vida_antes = p->vida;
    personaje_recibir_dano(p, e->dano);
    printf("%s te ataca: recibes %d de dano. %s: %d/%d PV.\n",
           e->nombre, vida_antes - p->vida, p->nombre, p->vida, p->vida_max);
}

/*
 * Usa un objeto durante el combate. Los de tipo OTRO duermen al enemigo.
 * Parametros: p - personaje, inv - inventario, h - historial, e - enemigo, id - ID del objeto.
 * Retorna: 0 si se uso (gasta turno), -1 si el ID no existe o no se pudo usar.
 */
static int usar_objeto_en_combate(Personaje *p, Inventario *inv, Historial *h, Enemigo *e, int id) {
    NodoInventario *nodo = inventario_buscar_por_id(inv, id);
    if (nodo == NULL) {
        printf("No existe un objeto con ID %d.\n", id);
        return -1;
    }
    Objeto obj = nodo->objeto;      /* copia: el nodo puede liberarse */
    char texto[MAX_DESCRIPCION];

    if (obj.tipo == OTRO) {
        if (enemigo_dormir(e, obj.valor) != 0) {
            printf("%s no tiene efecto.\n", obj.nombre);
            return -1;
        }
        printf("Usas %s: %s duerme %d turnos.\n", obj.nombre, e->nombre, obj.valor);
        inventario_eliminar_por_id(inv, id, NULL);
    } else if (inventario_usar_objeto(inv, id, p) != 0) {
        return -1;
    }
    snprintf(texto, sizeof(texto), "El personaje uso %s", obj.nombre);
    registrar_accion(h, ACCION_USAR_OBJETO, texto);
    return 0;
}

/*
 * Combate contra un solo enemigo.
 * Parametros: p - personaje, inv - inventario, h - historial, e - enemigo, en_espera - cantidad de enemigos restantes.
 * Retorna: 1 si el enemigo cae, 0 si muere el personaje, -1 si se acaba la entrada.
 */
static int pelear_contra(Personaje *p, Inventario *inv, Historial *h, Enemigo *e, int en_espera) {
    while (enemigo_esta_vivo(e) && p->vida > 0) {
        int opcion = 0;
        int gasto_turno = 0;

        mostrar_estado(p, e, en_espera);
        int lectura = entrada_leer_entero("> ", &opcion);
        if (lectura == -2) {
            return -1;
        }
        if (lectura == -1) {
            printf("Entrada invalida. Escribe 1, 2 o 3.\n");
            continue;
        }

        if (opcion == 1) {
            atacar_enemigo(p, e);
            gasto_turno = 1;
        } else if (opcion == 2) {
            inventario_listar(inv);
        } else if (opcion == 3) {
            int id = 0;
            lectura = entrada_leer_entero("ID del objeto > ", &id);
            if (lectura == -2) {
                return -1;
            }
            if (lectura == 0 && usar_objeto_en_combate(p, inv, h, e, id) == 0) {
                gasto_turno = 1;
            } else if (lectura == -1) {
                printf("ID invalido.\n");
            }
        } else {
            printf("Opcion no valida.\n");
        }

        if (gasto_turno && enemigo_esta_vivo(e)) {
            turno_enemigo(p, e);
        }
    }
    return enemigo_esta_vivo(e) ? 0 : 1;
}

/* Ver combate.h para la descripcion. 
 * Parametros: p - personaje, inv - inventario, h - historial, enemigos - cola de enemigos. */
int combate_iniciar(Personaje *p, Inventario *inv, Historial *h, ColaCombate *enemigos) {
    if (cola_esta_vacia(enemigos)) {
        printf("No hay enemigos.\n");
        return 1;
    }

    while (!cola_esta_vacia(enemigos)) {
        Enemigo actual;
        char texto[MAX_DESCRIPCION];

        cola_desencolar(enemigos, &actual);     /* el siguiente en el orden de aparicion */
        int resultado = pelear_contra(p, inv, h, &actual, enemigos->cantidad);

        if (resultado == 1) {
            printf("Derrotaste a %s!\n", actual.nombre);
            snprintf(texto, sizeof(texto), "Se derroto al enemigo %s", actual.nombre);
            registrar_accion(h, ACCION_DERROTAR_ENEMIGO, texto);
        } else if (resultado == 0) {
            printf("%s ha caido en combate...\n", p->nombre);
            return 0;
        } else {
            return -1;
        }
    }
    printf("Combate terminado!\n");
    return 1;
}