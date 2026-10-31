/* Autor: Tu Nombre */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guardado.h"

#define MAX_TOKEN 256               /* un texto codificado ocupa hasta el doble */
#define CABECERA "RPG_SAVE_2"

/* Codifica origen en destino (una sola palabra, sin espacios).
 * Parametros: origen - texto a codificar, destino - buffer para el texto codificado, capacidad - tamaño del buffer.
 */
static void codificar_texto(const char *origen, char *destino, size_t capacidad){
    size_t j = 0;

    if (capacidad == 0) {
        return;
    }
    if (origen[0] == '\0') {
        snprintf(destino, capacidad, "\\e");
        return;
    }
    for (size_t i = 0; origen[i] != '\0' && j + 2 < capacidad; i++) {
        char c = origen[i];
        if (c == ' ' || c == '\t' || c == '\n') {
            destino[j++] = '_';
        } else if (c == '_' || c == '\\') {
            destino[j++] = '\\';
            destino[j++] = c;
        } else {
            destino[j++] = c;
        }
    }
    destino[j] = '\0';
}

/* Deshace codificar_texto, modificando el texto en su lugar.
 * Parametros: texto - texto a decodificar.
 */
static void decodificar_texto(char *texto){
    size_t i = 0;
    size_t j = 0;

    if (strcmp(texto, "\\e") == 0) {
        texto[0] = '\0';
        return;
    }
    while (texto[i] != '\0') {
        if (texto[i] == '\\' && (texto[i + 1] == '_' || texto[i + 1] == '\\')) {
            texto[j++] = texto[i + 1];
            i += 2;
        } else if (texto[i] == '_') {
            texto[j++] = ' ';
            i++;
        } else {
            texto[j++] = texto[i++];
        }
    }
    texto[j] = '\0';
}

/* Copia origen en destino truncando si no cabe (siempre deja el terminador).
 * Parametros: destino - buffer para almacenar el texto, capacidad - tamaño del buffer, origen - texto a copiar.
 */
static void copiar_texto(char *destino, size_t capacidad, const char *origen){
    size_t n = strlen(origen);

    if (n >= capacidad) {
        n = capacidad - 1;
    }
    memcpy(destino, origen, n);
    destino[n] = '\0';
}

/* Escribe el historial de la mas antigua a la mas reciente. 0 ok, -1 error.
 * Parametros: archivo - archivo donde escribir, historial - historial a escribir.
 * Retorna: 0 si se escribio correctamente, -1 si hubo un error.
 */
static int escribir_historial(FILE *archivo, const Historial *historial){
    size_t n = (size_t)historial->cantidad;
    Accion *acciones;
    const NodoHistorial *nh;
    size_t i = 0;
    int resultado = 0;

    if (fprintf(archivo, "H %d\n", historial->cantidad) < 0) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }
    acciones = malloc(n * sizeof(*acciones));
    if (acciones == NULL) {
        return -1;
    }
    for (nh = historial->top; nh != NULL && i < n; nh = nh->siguiente) {
        acciones[i++] = nh->accion;             /* queda de reciente a antigua */
    }
    while (i > 0 && resultado == 0) {
        Accion a = acciones[--i];
        char desc[MAX_TOKEN];
        char nombre[MAX_TOKEN];

        codificar_texto(a.descripcion, desc, sizeof(desc));
        codificar_texto(a.objeto_descartado.nombre, nombre, sizeof(nombre));
        if (fprintf(archivo, "A %d %s %d %s %d %d\n", (int)a.tipo, desc,
                    a.objeto_descartado.id, nombre,
                    (int)a.objeto_descartado.tipo,
                    a.objeto_descartado.valor) < 0) {
            resultado = -1;
        }
    }
    free(acciones);
    return resultado;
}

/* Guarda el juego en un archivo. 0 ok, -1 error. 
 * Parametros: ruta - ruta del archivo, personaje - personaje a guardar,
   inventario - inventario a guardar, historial - historial a guardar.
 * Retorna: 0 si se guardo correctamente, -1 si hubo un error.
 */
int guardado_guardar(const char *ruta,const Personaje *personaje, const Inventario *inventario,
    const Historial *historial){
    FILE *archivo;
    const NodoInventario *ni;
    char nombre[MAX_TOKEN];
    int ok = 1;

    if (ruta == NULL || personaje == NULL || inventario == NULL ||
        historial == NULL) {
        return -1;
    }
    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        return -1;
    }

    codificar_texto(personaje->nombre, nombre, sizeof(nombre));
    ok = ok && fprintf(archivo, "%s\nP %s %d %d %d %d\n", CABECERA, nombre,
                       personaje->vida, personaje->vida_max,
                       personaje->ataque, personaje->defensa) > 0;
    ok = ok && fprintf(archivo, "I %d\n", inventario->cantidad) > 0;
    for (ni = inventario->cabeza; ok && ni != NULL; ni = ni->siguiente) {
        codificar_texto(ni->objeto.nombre, nombre, sizeof(nombre));
        ok = fprintf(archivo, "O %d %s %d %d\n", ni->objeto.id, nombre,
                     (int)ni->objeto.tipo, ni->objeto.valor) > 0;
    }
    ok = ok && escribir_historial(archivo, historial) == 0;

    if (fclose(archivo) != 0) {
        ok = 0;
    }
    return ok ? 0 : -1;
}

/* Lee "cantidad" objetos y los agrega a destino. 0 ok, -1 error.
 * Parametros: archivo - archivo desde donde leer, cantidad - número de objetos a leer, 
   destino - inventario donde agregar los objetos.
 * Retorna: 0 si se leyeron correctamente, -1 si hubo un error.
 */
static int leer_inventario(FILE *archivo, int cantidad, Inventario *destino){
    for (int i = 0; i < cantidad; i++) {
        char nombre[MAX_TOKEN];
        int id, tipo, valor;
        Objeto obj;

        if (fscanf(archivo, " O %d %255s %d %d", &id, nombre, &tipo, &valor) != 4 ||
            tipo < ARMA || tipo > OTRO) {
            return -1;
        }
        decodificar_texto(nombre);
        obj.id = id;
        copiar_texto(obj.nombre, sizeof(obj.nombre), nombre);
        obj.tipo = (TipoObjeto)tipo;
        obj.valor = valor;
        if (inventario_agregar(destino, obj) != 0) {
            return -1;
        }
    }
    return 0;
}

/* Lee "cantidad" acciones (de antigua a reciente) y las apila. 0 ok, -1 error.
 * Parametros: archivo - archivo desde donde leer, cantidad - número de acciones a leer, 
   destino - historial donde agregar las acciones.
 * Retorna: 0 si se leyeron correctamente, -1 si hubo un error.
 */
static int leer_historial(FILE *archivo, int cantidad, Historial *destino){
    for (int i = 0; i < cantidad; i++) {
        char desc[MAX_TOKEN];
        char nombre[MAX_TOKEN];
        int tipo_accion, obj_id, obj_tipo, obj_valor;
        Accion a;

        if (fscanf(archivo, " A %d %255s %d %255s %d %d", &tipo_accion, desc,
                   &obj_id, nombre, &obj_tipo, &obj_valor) != 6 ||
            tipo_accion < ACCION_ENCONTRAR_OBJETO || tipo_accion > ACCION_OTRA ||
            obj_tipo < ARMA || obj_tipo > OTRO) {
            return -1;
        }
        decodificar_texto(desc);
        decodificar_texto(nombre);
        memset(&a, 0, sizeof(a));
        a.tipo = (TipoAccion)tipo_accion;
        copiar_texto(a.descripcion, sizeof(a.descripcion), desc);
        a.objeto_descartado.id = obj_id;
        copiar_texto(a.objeto_descartado.nombre, sizeof(a.objeto_descartado.nombre), nombre);
        a.objeto_descartado.tipo = (TipoObjeto)obj_tipo;
        a.objeto_descartado.valor = obj_valor;
        if (historial_push(destino, a) != 0) {
            return -1;
        }
    }
    return 0;
}


/* Carga el juego desde un archivo. 0 ok, -1 error.
 * Parametros: ruta - ruta del archivo, personaje - personaje a cargar,
 *  inventario - inventario a cargar, historial - historial a cargar.
 * Retorna: 0 si se cargo correctamente, -1 si hubo un error.
 */
int guardado_cargar(const char *ruta,Personaje *personaje,Inventario *inventario,Historial *historial){
    FILE *archivo;
    char linea[MAX_TOKEN];
    char nombre[MAX_TOKEN];
    int vida, vida_max, ataque, defensa;
    int cant_obj, cant_acc;
    Inventario nuevo_inv;
    Historial nuevo_hist;

    if (ruta == NULL || personaje == NULL || inventario == NULL ||
        historial == NULL) {
        return -1;
    }
    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return -1;
    }
    inventario_iniciar(&nuevo_inv);
    historial_iniciar(&nuevo_hist);

    /* Todo se lee en estructuras temporales: si el archivo esta danado,
       la partida actual no se pierde. */
    if (fgets(linea, sizeof(linea), archivo) == NULL ||
        strcmp(linea, CABECERA "\n") != 0 ||
        fscanf(archivo, " P %255s %d %d %d %d", nombre, &vida, &vida_max,
               &ataque, &defensa) != 5 ||
        vida_max <= 0 || vida < 0 || vida > vida_max || ataque < 0 || defensa < 0 ||
        fscanf(archivo, " I %d", &cant_obj) != 1 || cant_obj < 0 ||
        leer_inventario(archivo, cant_obj, &nuevo_inv) != 0 ||
        fscanf(archivo, " H %d", &cant_acc) != 1 || cant_acc < 0 ||
        leer_historial(archivo, cant_acc, &nuevo_hist) != 0) {
        inventario_liberar(&nuevo_inv);
        historial_liberar(&nuevo_hist);
        fclose(archivo);
        return -1;
    }
    fclose(archivo);

    decodificar_texto(nombre);
    inventario_liberar(inventario);             /* recien ahora se reemplaza lo actual */
    historial_liberar(historial);
    *inventario = nuevo_inv;
    *historial = nuevo_hist;
    copiar_texto(personaje->nombre, sizeof(personaje->nombre), nombre);
    personaje->vida = vida;
    personaje->vida_max = vida_max;
    personaje->ataque = ataque;
    personaje->defensa = defensa;
    return 0;
}