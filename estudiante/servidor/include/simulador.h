/*
 * simulador.h - API del simulador gráfico. [DADO]
 *
 * La ventana muestra una casilla por aeronave, en el MISMO orden que tu lista.
 * Solo tienes que avisarle cuando la lista cambia:
 *
 *     iniciar_simulador();              // al arrancar el servidor (ya lo hace main)
 *     ...
 *     actualizar_simulador(&lista);     // después de CADA cambio en la lista
 *     ...
 *     detener_simulador();              // al terminar (ya lo hace main)
 *
 * actualizar_simulador recorre tu lista y guarda una copia; no modifica tu lista.
 */
#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "estructuras.h"

/* Abre la ventana. Devuelve 0 si bien y -1 si no se pudo (el servidor sigue sin ventana). */
int iniciar_simulador(void);

/* Refresca la ventana con el estado actual de la lista. No hace nada si no hay ventana. */
void actualizar_simulador(const ListaAeropuerto *lista);

/* Cierra la ventana y libera lo que usa el simulador. */
void detener_simulador(void);

#endif /* SIMULADOR_H */
