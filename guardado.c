/*
 * Archivo: guardado.c
 * Descripción: persistencia de personaje, inventario e historial.
 *
 * Formato de texto: cada registro ocupa una línea. Las cadenas se escriben
 * con espacios reemplazados por guiones bajos para simplificar el parseo.
 * El inventario y el historial existentes se reemplazan al cargar.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guardado.h"

#define MAX_LINEA 512

static void codificar_texto(const char *origen, char *destino, size_t capacidad)
{
    size_t i;

    if (capacidad == 0) {
        return;
    }
    for (i = 0; origen[i] != '\\0' && i + 1 < capacidad; i++) {
        destino[i] = (origen[i] == ' ' || origen[i] == '\\t') ? '_' : origen[i];
    }
    destino[i] = '\\0';
}

static void decodificar_texto(char *texto)
{
    size_t i;
    for (i = 0; texto[i] != '\\0'; i++) {
        if (texto[i] == '_') {
            texto[i] = ' ';
        }
    }
}

int guardado_guardar(const char *ruta,
                     const Personaje *personaje,
                     const Inventario *inventario,
                     const Historial *historial)
{
    FILE *archivo;
    const NodoInventario *ni;
    const NodoHistorial *nh;
    char nombre[50];
    char descripcion[MAX_DESCRIPCION];

    if (ruta == NULL || personaje == NULL || inventario == NULL ||
        historial == NULL) {
        return -1;
    }

    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        return -1;
    }

    codificar_texto(personaje->nombre, nombre, sizeof(nombre));
    if (fprintf(archivo, "RPG_SAVE_1\\nP %s %d %d %d %d\\n",
                nombre, personaje->vida, personaje->vida_max,
                personaje->ataque, personaje->defensa) < 0) {
        fclose(archivo);
        return -1;
    }

    if (fprintf(archivo, "I %d\\n", inventario->cantidad) < 0) {
        fclose(archivo);
        return -1;
    }
    for (ni = inventario->cabeza; ni != NULL; ni = ni->siguiente) {
        char obj_nombre[50];
        codificar_texto(ni->objeto.nombre, obj_nombre, sizeof(obj_nombre));
        if (fprintf(archivo, "O %d %s %d %d\\n",
                    ni->objeto.id, obj_nombre, (int)ni->objeto.tipo,
                    ni->objeto.valor) < 0) {
            fclose(archivo);
            return -1;
        }
    }

    if (fprintf(archivo, "H %d\\n", historial->cantidad) < 0) {
        fclose(archivo);
        return -1;
    }
    /* Se guarda de más antigua a más reciente para reconstruir la pila. */
    {
        size_t n = (size_t)historial->cantidad;
        Accion *acciones = NULL;
        size_t i = 0;

        if (n > 0) {
            acciones = malloc(n * sizeof(*acciones));
            if (acciones == NULL) {
                fclose(archivo);
                return -1;
            }
            for (nh = historial->top; nh != NULL && i < n;
                 nh = nh->siguiente) {
                acciones[i++] = nh->accion;
            }
            while (i > 0) {
                Accion a = acciones[--i];
                codificar_texto(a.descripcion, descripcion, sizeof(descripcion));
                if (fprintf(archivo, "A %d %s %d %s %d %d %d\\n",
                            (int)a.tipo, descripcion,
                            a.objeto_descartado.id,
                            a.objeto_descartado.nombre,
                            (int)a.objeto_descartado.tipo,
                            a.objeto_descartado.valor,
                            0) < 0) {
                    free(acciones);
                    fclose(archivo);
                    return -1;
                }
            }
            free(acciones);
        }
    }

    if (fclose(archivo) != 0) {
        return -1;
    }
    return 0;
}

int guardado_cargar(const char *ruta,
                    Personaje *personaje,
                    Inventario *inventario,
                    Historial *historial)
{
    FILE *archivo;
    char linea[MAX_LINEA];
    char nombre[50];
    int vida, vida_max, ataque, defensa;
    int cantidad, i;

    if (ruta == NULL || personaje == NULL || inventario == NULL ||
        historial == NULL) {
        return -1;
    }

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return -1;
    }

    if (fgets(linea, sizeof(linea), archivo) == NULL ||
        strcmp(linea, "RPG_SAVE_1\\n") != 0) {
        fclose(archivo);
        return -1;
    }

    if (fscanf(archivo, " P %49s %d %d %d %d",
               nombre, &vida, &vida_max, &ataque, &defensa) != 5) {
        fclose(archivo);
        return -1;
    }
    decodificar_texto(nombre);

    if (fscanf(archivo, " I %d", &cantidad) != 1 || cantidad < 0) {
        fclose(archivo);
        return -1;
    }

    /* Cargar en estructuras temporales para no perder la partida actual
       si el archivo está corrupto o falla una reserva de memoria. */
    {
        Inventario nuevo_inv;
        Historial nuevo_hist;
        Objeto obj;
        int tipo, valor, id;
        int cargados = 0;

        inventario_iniciar(&nuevo_inv);
        historial_iniciar(&nuevo_hist);

        for (i = 0; i < cantidad; i++) {
            char obj_nombre[50];
            if (fscanf(archivo, " O %d %49s %d %d",
                       &id, obj_nombre, &tipo, &valor) != 4) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
            if (tipo < ARMA || tipo > OTRO) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
            decodificar_texto(obj_nombre);
            obj.id = id;
            (void)snprintf(obj.nombre, sizeof(obj.nombre), "%s", obj_nombre);
            obj.tipo = (TipoObjeto)tipo;
            obj.valor = valor;
            if (inventario_agregar(&nuevo_inv, obj) != 0) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
        }

        if (fscanf(archivo, " H %d", &cantidad) != 1 || cantidad < 0) {
            inventario_liberar(&nuevo_inv);
            historial_liberar(&nuevo_hist);
            fclose(archivo);
            return -1;
        }

        for (i = 0; i < cantidad; i++) {
            Accion a;
            char desc[MAX_DESCRIPCION];
            char obj_nombre[50];
            int tipo_accion, obj_id, obj_tipo, obj_valor, reservado;

            if (fscanf(archivo, " A %d %99s %d %49s %d %d %d",
                       &tipo_accion, desc, &obj_id, obj_nombre,
                       &obj_tipo, &obj_valor, &reservado) != 7) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
            if (tipo_accion < ACCION_ENCONTRAR_OBJETO ||
                tipo_accion > ACCION_OTRA ||
                obj_tipo < ARMA || obj_tipo > OTRO) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
            decodificar_texto(desc);
            decodificar_texto(obj_nombre);
            a.tipo = (TipoAccion)tipo_accion;
            (void)snprintf(a.descripcion, sizeof(a.descripcion), "%s", desc);
            a.objeto_descartado.id = obj_id;
            (void)snprintf(a.objeto_descartado.nombre,
                           sizeof(a.objeto_descartado.nombre), "%s", obj_nombre);
            a.objeto_descartado.tipo = (TipoObjeto)obj_tipo;
            a.objeto_descartado.valor = obj_valor;

            if (historial_push(&nuevo_hist, a) != 0) {
                inventario_liberar(&nuevo_inv);
                historial_liberar(&nuevo_hist);
                fclose(archivo);
                return -1;
            }
            cargados++;
        }

        (void)cargados;

        /* Reemplazar las estructuras destino solo tras cargar correctamente. */
        inventario_liberar(inventario);
        historial_liberar(historial);
        *inventario = nuevo_inv;
        *historial = nuevo_hist;
    }

    (void)snprintf(personaje->nombre, sizeof(personaje->nombre), "%s", nombre);
    personaje->vida = vida;
    personaje->vida_max = vida_max;
    personaje->ataque = ataque;
    personaje->defensa = defensa;

    if (fclose(archivo) != 0) {
        return -1;
    }
    return 0;
}
