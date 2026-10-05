/*
 * simulador_nulo.c - Sustituto sin gráficos (compilado con SIN_SDL=1 o sin SDL2).
 * Las tres funciones existen pero no hacen nada, así que tu servidor no cambia.
 */
#include <stdio.h>

#include "simulador.h"

int iniciar_simulador(void)
{
    fprintf(stderr, "Aviso: compilado sin SDL2, no hay simulador gráfico.\n");
    return -1;
}

void actualizar_simulador(const ListaAeropuerto *lista)
{
    (void)lista;
}

void detener_simulador(void)
{
}
