/*
 * renderer.h - Dibujo del aeropuerto a partir de una copia del estado (PROPORCIONADO).
 *
 * El renderer NO conoce tu lista: solo recibe una copia (EstadoSimulador) que
 * simulador.c hace al recorrer tu lista, así que dibujar nunca interfiere con el
 * servidor.
 *
 * Disposición: la aeronave en la posición i de la lista ocupa la casilla i
 * de una cuadrícula (de izquierda a derecha y de arriba abajo). Si la lista
 * está ordenada, los IDs se ven en orden creciente; tras un SORT las
 * casillas se reordenan.
 */
#ifndef RENDERER_H
#define RENDERER_H

#include "estructuras.h"
#include "ventana.h"

/* Copia del estado de la lista: las aeronaves en el MISMO orden que en la lista. */
typedef struct
{
    Aeronave *items;
    int cantidad;
    int ordenada;           /* 1 si los IDs van en orden ascendente */
} EstadoSimulador;

#define VENTANA_ANCHO 1000
#define VENTANA_ALTO  720

/* Id de la aeronave bajo el punto (x, y), o -1 si no hay ninguna. */
int renderer_id_en(const EstadoSimulador *s, int x, int y);

/*
 * Dibuja un fotograma completo. 'id_seleccionado' (o -1) se resalta;
 * (raton_x, raton_y) sirve para el resaltado "hover" y el panel de detalle.
 */
void renderer_dibujar(Ventana *v, const EstadoSimulador *s,
                      int id_seleccionado, int raton_x, int raton_y);

#endif /* RENDERER_H */
