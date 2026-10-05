#include "ventana.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------
 * Fuente bitmap 5x7 (ASCII 32..95). Cada glifo son 5 columnas; en cada
 * columna el bit 0 es la fila superior. Las minúsculas se dibujan como
 * mayúsculas.
 * --------------------------------------------------------------------- */
#define FUENTE_PRIMERO  32
#define FUENTE_ULTIMO   95
#define GLIFO_ANCHO     5
#define GLIFO_ALTO      7
#define GLIFO_AVANCE    6   /* ancho + 1 columna de separación */

static const uint8_t FUENTE[FUENTE_ULTIMO - FUENTE_PRIMERO + 1][GLIFO_ANCHO] = {
    {0x00,0x00,0x00,0x00,0x00}, /* ' ' */ {0x00,0x00,0x5F,0x00,0x00}, /* ! */
    {0x00,0x07,0x00,0x07,0x00}, /* " */   {0x14,0x7F,0x14,0x7F,0x14}, /* # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */   {0x23,0x13,0x08,0x64,0x62}, /* % */
    {0x36,0x49,0x56,0x20,0x50}, /* & */   {0x00,0x08,0x07,0x03,0x00}, /* ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* ( */   {0x00,0x41,0x22,0x1C,0x00}, /* ) */
    {0x2A,0x1C,0x7F,0x1C,0x2A}, /* * */   {0x08,0x08,0x3E,0x08,0x08}, /* + */
    {0x00,0x50,0x30,0x00,0x00}, /* , */   {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x00,0x00,0x60,0x60,0x00}, /* . */   {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */   {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
    {0x72,0x49,0x49,0x49,0x46}, /* 2 */   {0x21,0x41,0x49,0x4D,0x33}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4 */   {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x31}, /* 6 */   {0x41,0x21,0x11,0x09,0x07}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */   {0x46,0x49,0x49,0x29,0x1E}, /* 9 */
    {0x00,0x00,0x14,0x00,0x00}, /* : */   {0x00,0x40,0x34,0x00,0x00}, /* ; */
    {0x00,0x08,0x14,0x22,0x41}, /* < */   {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x00,0x41,0x22,0x14,0x08}, /* > */   {0x02,0x01,0x59,0x09,0x06}, /* ? */
    {0x3E,0x41,0x5D,0x59,0x4E}, /* @ */   {0x7C,0x12,0x11,0x12,0x7C}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */   {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x41,0x3E}, /* D */   {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */   {0x3E,0x41,0x41,0x51,0x73}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */   {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */   {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */   {0x7F,0x02,0x1C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */   {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */   {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */   {0x26,0x49,0x49,0x49,0x32}, /* S */
    {0x03,0x01,0x7F,0x01,0x03}, /* T */   {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */   {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */   {0x03,0x04,0x78,0x04,0x03}, /* Y */
    {0x61,0x59,0x49,0x4D,0x43}, /* Z */   {0x00,0x7F,0x41,0x41,0x00}, /* [ */
    {0x02,0x04,0x08,0x10,0x20}, /* \ */   {0x00,0x41,0x41,0x7F,0x00}, /* ] */
    {0x04,0x02,0x01,0x02,0x04}, /* ^ */   {0x40,0x40,0x40,0x40,0x40}, /* _ */
};

#define FUENTE_NUM (FUENTE_ULTIMO - FUENTE_PRIMERO + 1)

struct Ventana
{
    SDL_Window *ventana;
    SDL_Renderer *renderer;
    SDL_Texture *atlas;     /* todos los glifos en una textura blanca */
    int ancho, alto;
};

/* Crea la textura con los glifos: se dibuja cada letra con SDL_RenderCopy. */
static SDL_Texture *crear_atlas(SDL_Renderer *r)
{
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(
        0, FUENTE_NUM * GLIFO_ANCHO, GLIFO_ALTO, 32, SDL_PIXELFORMAT_RGBA32);
    if (s == NULL)
    {
        return NULL;
    }

    uint32_t blanco = SDL_MapRGBA(s->format, 255, 255, 255, 255);
    uint32_t vacio = SDL_MapRGBA(s->format, 255, 255, 255, 0);
    for (int g = 0; g < FUENTE_NUM; g++)
    {
        for (int col = 0; col < GLIFO_ANCHO; col++)
        {
            for (int fila = 0; fila < GLIFO_ALTO; fila++)
            {
                uint32_t *px = (uint32_t *)((uint8_t *)s->pixels +
                                            fila * s->pitch) +
                               g * GLIFO_ANCHO + col;
                *px = ((FUENTE[g][col] >> fila) & 1) ? blanco : vacio;
            }
        }
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    SDL_FreeSurface(s);
    if (t != NULL)
    {
        SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    }
    return t;
}

Ventana *ventana_crear(const char *titulo, int ancho, int alto)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return NULL;
    }

    Ventana *v = calloc(1, sizeof *v);
    if (v == NULL)
    {
        SDL_Quit();
        return NULL;
    }
    v->ancho = ancho;
    v->alto = alto;

    v->ventana = SDL_CreateWindow(titulo, SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, ancho, alto,
                                  SDL_WINDOW_SHOWN);
    if (v->ventana != NULL)
    {
        v->renderer = SDL_CreateRenderer(v->ventana, -1,
                                         SDL_RENDERER_ACCELERATED |
                                             SDL_RENDERER_PRESENTVSYNC);
        if (v->renderer == NULL) /* sin aceleración (máquinas virtuales...) */
        {
            v->renderer = SDL_CreateRenderer(v->ventana, -1, SDL_RENDERER_SOFTWARE);
        }
    }
    if (v->renderer != NULL)
    {
        SDL_SetRenderDrawBlendMode(v->renderer, SDL_BLENDMODE_BLEND);
        v->atlas = crear_atlas(v->renderer);
    }
    if (v->ventana == NULL || v->renderer == NULL || v->atlas == NULL)
    {
        fprintf(stderr, "No se pudo crear la ventana: %s\n", SDL_GetError());
        ventana_destruir(v);
        return NULL;
    }
    return v;
}

void ventana_destruir(Ventana *v)
{
    if (v == NULL)
    {
        return;
    }
    if (v->atlas != NULL)
    {
        SDL_DestroyTexture(v->atlas);
    }
    if (v->renderer != NULL)
    {
        SDL_DestroyRenderer(v->renderer);
    }
    if (v->ventana != NULL)
    {
        SDL_DestroyWindow(v->ventana);
    }
    free(v);
    SDL_Quit();
}

void ventana_eventos(Ventana *v, EntradaVentana *entrada)
{
    (void)v;
    entrada->click = 0;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            entrada->cerrar = 1;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                entrada->cerrar = 1;
            }
            break;
        case SDL_MOUSEMOTION:
            entrada->raton_x = e.motion.x;
            entrada->raton_y = e.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                entrada->click = 1;
                entrada->raton_x = e.button.x;
                entrada->raton_y = e.button.y;
            }
            break;
        default:
            break;
        }
    }
}

static void poner_color(Ventana *v, Color c)
{
    SDL_SetRenderDrawColor(v->renderer, c.r, c.g, c.b, c.a);
}

void ventana_limpiar(Ventana *v, Color c)
{
    poner_color(v, c);
    SDL_RenderClear(v->renderer);
}

void ventana_presentar(Ventana *v)
{
    SDL_RenderPresent(v->renderer);
}

void ventana_rect(Ventana *v, int x, int y, int w, int h, Color c)
{
    SDL_Rect r = { x, y, w, h };
    poner_color(v, c);
    SDL_RenderFillRect(v->renderer, &r);
}

void ventana_rect_borde(Ventana *v, int x, int y, int w, int h, int grosor,
                        Color c)
{
    ventana_rect(v, x, y, w, grosor, c);
    ventana_rect(v, x, y + h - grosor, w, grosor, c);
    ventana_rect(v, x, y, grosor, h, c);
    ventana_rect(v, x + w - grosor, y, grosor, h, c);
}

void ventana_linea(Ventana *v, int x1, int y1, int x2, int y2, Color c)
{
    poner_color(v, c);
    SDL_RenderDrawLine(v->renderer, x1, y1, x2, y2);
}

void ventana_triangulo(Ventana *v, float x1, float y1, float x2, float y2,
                       float x3, float y3, Color c)
{
    SDL_Color sc = { c.r, c.g, c.b, c.a };
    SDL_Vertex vert[3] = {
        { { x1, y1 }, sc, { 0, 0 } },
        { { x2, y2 }, sc, { 0, 0 } },
        { { x3, y3 }, sc, { 0, 0 } },
    };
    SDL_RenderGeometry(v->renderer, NULL, vert, 3, NULL, 0);
}

void ventana_cuadrilatero(Ventana *v, float x1, float y1, float x2, float y2,
                          float x3, float y3, float x4, float y4, Color c)
{
    ventana_triangulo(v, x1, y1, x2, y2, x3, y3, c);
    ventana_triangulo(v, x1, y1, x3, y3, x4, y4, c);
}

/*
 * Traduce UN carácter UTF-8 al índice de glifo. Devuelve cuántos bytes ha
 * consumido (>= 1). Las vocales acentuadas y la ñ se dibujan sin acento.
 */
static int siguiente_glifo(const char *s, int *glifo)
{
    unsigned char c = (unsigned char)s[0];
    int consumidos = 1;

    if (c >= 0xC0) /* inicio de carácter multibyte */
    {
        unsigned char c2 = (unsigned char)s[1];
        consumidos = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : 2;
        c = '?';
        if (s[0] == (char)0xC3 && c2 != 0)
        {
            switch (c2)
            {
            case 0xA1: case 0x81: c = 'A'; break; /* á Á */
            case 0xA9: case 0x89: c = 'E'; break; /* é É */
            case 0xAD: case 0x8D: c = 'I'; break; /* í Í */
            case 0xB3: case 0x93: c = 'O'; break; /* ó Ó */
            case 0xBA: case 0x9A: case 0xBC: case 0x9C: c = 'U'; break; /* ú ü */
            case 0xB1: case 0x91: c = 'N'; break; /* ñ Ñ */
            default: break;
            }
        }
        /* No salirse de la cadena si está truncada */
        for (int i = 1; i < consumidos; i++)
        {
            if (s[i] == '\0')
            {
                consumidos = i;
                break;
            }
        }
    }
    else if (c >= 0x80) /* byte suelto de continuación */
    {
        c = '?';
    }

    if (c >= 'a' && c <= 'z')
    {
        c = (unsigned char)(c - 'a' + 'A');
    }
    if (c < FUENTE_PRIMERO || c > FUENTE_ULTIMO)
    {
        c = '?';
    }
    *glifo = c - FUENTE_PRIMERO;
    return consumidos;
}

void ventana_texto(Ventana *v, int x, int y, int escala, Color c,
                   const char *texto)
{
    SDL_SetTextureColorMod(v->atlas, c.r, c.g, c.b);
    SDL_SetTextureAlphaMod(v->atlas, c.a);

    while (*texto != '\0')
    {
        int g;
        texto += siguiente_glifo(texto, &g);

        SDL_Rect origen = { g * GLIFO_ANCHO, 0, GLIFO_ANCHO, GLIFO_ALTO };
        SDL_Rect destino = { x, y, GLIFO_ANCHO * escala, GLIFO_ALTO * escala };
        SDL_RenderCopy(v->renderer, v->atlas, &origen, &destino);
        x += GLIFO_AVANCE * escala;
    }
}

int ventana_ancho_texto(const char *texto, int escala)
{
    int n = 0;
    while (*texto != '\0')
    {
        int g;
        texto += siguiente_glifo(texto, &g);
        n++;
    }
    return n * GLIFO_AVANCE * escala;
}

int ventana_alto_texto(int escala)
{
    return GLIFO_ALTO * escala;
}

int ventana_guardar_bmp(Ventana *v, const char *ruta)
{
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, v->ancho, v->alto, 32,
                                                    SDL_PIXELFORMAT_ARGB8888);
    if (s == NULL)
    {
        return -1;
    }
    int r = SDL_RenderReadPixels(v->renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                                 s->pixels, s->pitch);
    if (r == 0)
    {
        r = SDL_SaveBMP(s, ruta);
    }
    SDL_FreeSurface(s);
    return r;
}
