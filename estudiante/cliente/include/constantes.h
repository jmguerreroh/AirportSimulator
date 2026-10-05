/*
 * constantes.h (CLIENTE) - Tamaños y constantes. [DADO: no lo modifiques]
 */
#ifndef CONSTANTES_H
#define CONSTANTES_H

/* Tamaños máximos de los textos de una aeronave (incluyen el '\0') */
#define MAX_ORIGEN   64
#define MAX_DESTINO  64
#define MAX_MODELO   32

/* Una petición cabe en una línea de este tamaño (con el '\n') */
#define MAX_PETICION 256

/* La respuesta del servidor (resultado + estado de todas las aeronaves) cabe en este buffer */
#define MAX_RESPUESTA 32768

#endif /* CONSTANTES_H */
