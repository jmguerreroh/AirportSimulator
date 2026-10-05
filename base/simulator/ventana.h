/*
 * ventana.h - Envoltorio mínimo de SDL2 (PROPORCIONADO).
 *
 * Oculta SDL al resto del simulador: ventana, eventos y primitivas 2D
 * (rectángulos, polígonos y texto con una fuente bitmap integrada, de modo
 * que no hace falta SDL_ttf ni ficheros de fuentes).
 */
#ifndef VENTANA_H
#define VENTANA_H

#include <stdint.h>

typedef struct Ventana Ventana;

typedef struct
{
    uint8_t r, g, b, a;
} Color;

/* Entrada del usuario acumulada desde la última llamada a ventana_eventos(). */
typedef struct
{
    int cerrar;         /* cerró la ventana o pulsó ESC */
    int raton_x;        /* posición actual del ratón */
    int raton_y;
    int click;          /* 1 si hubo clic izquierdo */
} EntradaVentana;

Ventana *ventana_crear(const char *titulo, int ancho, int alto);
void ventana_destruir(Ventana *v);

void ventana_eventos(Ventana *v, EntradaVentana *entrada);
void ventana_limpiar(Ventana *v, Color c);
void ventana_presentar(Ventana *v);

void ventana_rect(Ventana *v, int x, int y, int w, int h, Color c);
void ventana_rect_borde(Ventana *v, int x, int y, int w, int h, int grosor,
                        Color c);
void ventana_linea(Ventana *v, int x1, int y1, int x2, int y2, Color c);
void ventana_triangulo(Ventana *v, float x1, float y1, float x2, float y2,
                       float x3, float y3, Color c);
void ventana_cuadrilatero(Ventana *v, float x1, float y1, float x2, float y2,
                          float x3, float y3, float x4, float y4, Color c);

/* Texto con la fuente integrada. 'escala' = píxeles por punto de la fuente. */
void ventana_texto(Ventana *v, int x, int y, int escala, Color c,
                   const char *texto);
int ventana_ancho_texto(const char *texto, int escala);
int ventana_alto_texto(int escala);

/* Guarda el contenido actual de la ventana en un BMP. 0 si bien. */
int ventana_guardar_bmp(Ventana *v, const char *ruta);

#endif /* VENTANA_H */
