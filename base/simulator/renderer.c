#include "renderer.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define TXT_MAX 256

/* ---- Geometría de la pantalla ---- */
#define MARGEN        10
#define ALTO_CABECERA 56
#define ALTO_PANEL    118
#define CELDA_MAX_W   210
#define CELDA_MAX_H   175

/* ---- Paleta ---- */
static const Color C_FONDO     = { 24, 28, 36, 255 };
static const Color C_CABECERA  = { 18, 22, 30, 255 };
static const Color C_ASFALTO   = { 52, 56, 64, 255 };
static const Color C_CASILLA   = { 64, 69, 78, 255 };
static const Color C_LINEA     = { 230, 200, 60, 255 };
static const Color C_TEXTO     = { 235, 238, 245, 255 };
static const Color C_TENUE     = { 150, 158, 172, 255 };
static const Color C_SELEC     = { 255, 214, 10, 255 };
static const Color C_HOVER     = { 255, 255, 255, 255 };
static const Color C_OK        = { 80, 200, 120, 255 };
static const Color C_AVISO     = { 255, 159, 67, 255 };
static const Color C_BARRA_BG  = { 30, 34, 42, 255 };

typedef struct
{
    int cols, filas;
    int x0, y0;         /* esquina superior izquierda de la cuadrícula */
    int w, h;           /* tamaño de cada casilla */
} Cuadricula;

/*
 * Elige columnas/filas para que las casillas sean lo más grandes posible
 * dentro del área disponible (para 3 aviones, casillas grandes; para 100,
 * casillas pequeñas pero visibles).
 */
static Cuadricula calcular_cuadricula(int n)
{
    const int area_x = MARGEN;
    const int area_y = ALTO_CABECERA + MARGEN;
    const int area_w = VENTANA_ANCHO - 2 * MARGEN;
    const int area_h = VENTANA_ALTO - ALTO_CABECERA - ALTO_PANEL - 3 * MARGEN;

    Cuadricula mejor = { 1, 1, area_x, area_y, area_w, area_h };
    float mejor_puntuacion = -1.0f;

    for (int cols = 1; cols <= (n > 0 ? n : 1); cols++)
    {
        int filas = (n + cols - 1) / cols;
        if (filas < 1)
        {
            filas = 1;
        }
        int w = area_w / cols;
        int h = area_h / filas;
        if (w > CELDA_MAX_W) w = CELDA_MAX_W;
        if (h > CELDA_MAX_H) h = CELDA_MAX_H;

        float p = fminf((float)w / 1.25f, (float)h);
        if (p >= mejor_puntuacion) /* en empate, más columnas */
        {
            mejor_puntuacion = p;
            mejor.cols = cols;
            mejor.filas = filas;
            mejor.w = w;
            mejor.h = h;
        }
    }
    /* Centrar el bloque en el área disponible */
    mejor.x0 = area_x + (area_w - mejor.cols * mejor.w) / 2;
    mejor.y0 = area_y + (area_h - mejor.filas * mejor.h) / 2;
    return mejor;
}

static void casilla(const Cuadricula *g, int indice, int *x, int *y)
{
    *x = g->x0 + (indice % g->cols) * g->w;
    *y = g->y0 + (indice / g->cols) * g->h;
}

int renderer_id_en(const EstadoSimulador *s, int px, int py)
{
    Cuadricula g = calcular_cuadricula(s->cantidad);
    for (int i = 0; i < s->cantidad; i++)
    {
        int x, y;
        casilla(&g, i, &x, &y);
        if (px >= x && px < x + g.w && py >= y && py < y + g.h)
        {
            return s->items[i].id;
        }
    }
    return -1;
}

/* ---- Aviones ---- */

/* Escala (0..1 del espacio disponible) y color según el modelo. */
static void estilo_modelo(const char *modelo, float *escala, Color *color)
{
    char m = modelo[0];
    if (strncmp(modelo, "A35", 3) == 0 || strncmp(modelo, "A38", 3) == 0 ||
        strncmp(modelo, "B74", 3) == 0 || strncmp(modelo, "B77", 3) == 0)
    {
        *escala = 1.00f;                       /* grande */
    }
    else if (strncmp(modelo, "B73", 3) == 0 || strncmp(modelo, "A33", 3) == 0)
    {
        *escala = 0.82f;                       /* mediano */
    }
    else
    {
        *escala = 0.66f;                       /* pequeño / desconocido */
    }

    if (m == 'A' || m == 'a')
    {
        *color = (Color){ 90, 170, 255, 255 }; /* Airbus */
    }
    else if (m == 'B' || m == 'b')
    {
        *color = (Color){ 255, 140, 90, 255 }; /* Boeing */
    }
    else
    {
        *color = (Color){ 190, 150, 255, 255 };
    }
}

/* Avión visto desde arriba, centrado en (cx, cy), 'tam' = alto total. */
static void dibujar_avion(Ventana *v, float cx, float cy, float tam, Color c)
{
    float mitad = tam / 2.0f;
    float fw = tam * 0.075f;                    /* semiancho del fuselaje */
    float top = cy - mitad;
    float ala_y = cy - tam * 0.10f;             /* raíz del ala */
    float envergadura = tam * 0.50f;            /* semi-envergadura */
    Color sombra = { (uint8_t)(c.r * 0.7f), (uint8_t)(c.g * 0.7f),
                     (uint8_t)(c.b * 0.7f), 255 };

    /* alas (detrás del fuselaje) */
    ventana_cuadrilatero(v, cx - fw, ala_y, cx - envergadura, ala_y + tam * 0.24f,
                         cx - envergadura, ala_y + tam * 0.31f, cx - fw,
                         ala_y + tam * 0.20f, sombra);
    ventana_cuadrilatero(v, cx + fw, ala_y, cx + envergadura, ala_y + tam * 0.24f,
                         cx + envergadura, ala_y + tam * 0.31f, cx + fw,
                         ala_y + tam * 0.20f, sombra);
    /* estabilizadores de cola */
    float cola_y = cy + mitad - tam * 0.17f;
    float cola_w = tam * 0.19f;
    ventana_cuadrilatero(v, cx - fw, cola_y, cx - cola_w, cola_y + tam * 0.11f,
                         cx - cola_w, cola_y + tam * 0.15f, cx - fw,
                         cola_y + tam * 0.12f, sombra);
    ventana_cuadrilatero(v, cx + fw, cola_y, cx + cola_w, cola_y + tam * 0.11f,
                         cx + cola_w, cola_y + tam * 0.15f, cx + fw,
                         cola_y + tam * 0.12f, sombra);
    /* fuselaje y morro */
    ventana_cuadrilatero(v, cx - fw, top + fw * 2.0f, cx + fw, top + fw * 2.0f,
                         cx + fw * 0.8f, cy + mitad, cx - fw * 0.8f, cy + mitad, c);
    ventana_triangulo(v, cx, top, cx - fw, top + fw * 2.0f, cx + fw,
                      top + fw * 2.0f, c);
}

static void barra(Ventana *v, int x, int y, int w, int h, float fraccion,
                  Color relleno)
{
    if (fraccion < 0.0f) fraccion = 0.0f;
    if (fraccion > 1.0f) fraccion = 1.0f;
    ventana_rect(v, x, y, w, h, C_BARRA_BG);
    ventana_rect(v, x, y, (int)(w * fraccion), h, relleno);
}

static Color color_combustible(float fuel_pct)
{
    if (fuel_pct < 20.0f) return (Color){ 235, 80, 80, 255 };
    if (fuel_pct < 50.0f) return (Color){ 240, 190, 60, 255 };
    return C_OK;
}

static void dibujar_casilla(Ventana *v, const Cuadricula *g, int indice,
                            const Aeronave *a, int seleccionada, int hover)
{
    int x, y;
    casilla(g, indice, &x, &y);

    /* plaza de aparcamiento */
    int pad = 3;
    ventana_rect(v, x + pad, y + pad, g->w - 2 * pad, g->h - 2 * pad, C_CASILLA);
    ventana_rect_borde(v, x + pad, y + pad, g->w - 2 * pad, g->h - 2 * pad, 1,
                       C_LINEA);
    if (seleccionada)
    {
        ventana_rect_borde(v, x + pad, y + pad, g->w - 2 * pad, g->h - 2 * pad,
                           3, C_SELEC);
    }
    else if (hover)
    {
        ventana_rect_borde(v, x + pad, y + pad, g->w - 2 * pad, g->h - 2 * pad,
                           2, C_HOVER);
    }

    int detalle = (g->w >= 150 && g->h >= 120);   /* texto completo */
    int medio = !detalle && g->w >= 70 && g->h >= 70; /* ID + modelo */

    char txt[TXT_MAX];
    int cx = x + g->w / 2;
    int ty = y + pad + 4;

    /* posición en la lista (útil para ver el efecto de SORT) */
    snprintf(txt, sizeof txt, "#%d", indice + 1);
    ventana_texto(v, x + pad + 4, ty, 1, C_TENUE, txt);

    /* avión */
    float escala;
    Color color;
    estilo_modelo(a->modelo, &escala, &color);
    float espacio = fminf((float)(g->w - 20), (float)g->h * (detalle ? 0.50f : 0.55f));
    float tam = espacio * escala;
    float cy_avion = y + pad + 12 + espacio / 2.0f;
    if (!detalle && !medio)
    {
        cy_avion = y + g->h * 0.42f;
        tam = fminf((float)g->h * 0.5f, (float)g->w - 8) * escala;
    }
    dibujar_avion(v, (float)cx, cy_avion, tam, color);

    /* texto */
    snprintf(txt, sizeof txt, "AV-%d", a->id);
    int esc = detalle ? 2 : 1;
    int linea_y = y + g->h - pad - 6;       /* se rellena de abajo a arriba */

    if (detalle)
    {
        int fh = ventana_alto_texto(1) + 3;
        int h_barras = 14;
        int y_bar = y + g->h - pad - 5 - h_barras;
        barra(v, x + 10, y_bar, g->w - 20, 5, a->combustible / 100.0f,
              color_combustible(a->combustible));
        barra(v, x + 10, y_bar + 8, g->w - 20, 5, a->capacidad_porcentaje / 100.0f,
              (Color){ 120, 160, 255, 255 });

        int ly = y_bar - fh;
        snprintf(txt, sizeof txt, "FUEL %.0f  CAP %d%%", a->combustible,
                 a->capacidad_porcentaje);
        ventana_texto(v, cx - ventana_ancho_texto(txt, 1) / 2, ly, 1, C_TENUE, txt);
        ly -= fh;
        snprintf(txt, sizeof txt, "%s -> %s", a->origen, a->destino);
        if (ventana_ancho_texto(txt, 1) > g->w - 10)
        {
            txt[(g->w - 10) / 6] = '\0';
        }
        ventana_texto(v, cx - ventana_ancho_texto(txt, 1) / 2, ly, 1, C_TEXTO, txt);
        ly -= fh;
        ventana_texto(v, cx - ventana_ancho_texto(a->modelo, 1) / 2, ly, 1,
                      color, a->modelo);
        snprintf(txt, sizeof txt, "AV-%d", a->id);
        ly -= ventana_alto_texto(esc) + 2;
        ventana_texto(v, cx - ventana_ancho_texto(txt, esc) / 2, ly, esc,
                      C_TEXTO, txt);
    }
    else if (medio)
    {
        int fh = ventana_alto_texto(1) + 3;
        ventana_texto(v, cx - ventana_ancho_texto(a->modelo, 1) / 2, linea_y - fh,
                      1, color, a->modelo);
        snprintf(txt, sizeof txt, "%d", a->id);
        ventana_texto(v, cx - ventana_ancho_texto(txt, 1) / 2, linea_y - 2 * fh,
                      1, C_TEXTO, txt);
    }
    else
    {
        snprintf(txt, sizeof txt, "%d", a->id);
        ventana_texto(v, cx - ventana_ancho_texto(txt, 1) / 2, linea_y - 2, 1,
                      C_TEXTO, txt);
    }
}

static void dibujar_cabecera(Ventana *v, const EstadoSimulador *s)
{
    char txt[128];
    ventana_rect(v, 0, 0, VENTANA_ANCHO, ALTO_CABECERA, C_CABECERA);
    ventana_texto(v, MARGEN + 6, 14, 4, C_TEXTO, "AEROPUERTO");

    snprintf(txt, sizeof txt, "AERONAVES: %d", s->cantidad);
    ventana_texto(v, VENTANA_ANCHO - 300, 10, 2, C_TEXTO, txt);

    Color c = s->ordenada ? C_OK : C_AVISO;
    ventana_texto(v, VENTANA_ANCHO - 300, 32, 2, c,
                  s->ordenada ? "ORDENADO POR ID" : "SIN ORDENAR");
}

static void dibujar_panel(Ventana *v, const EstadoSimulador *s, int id)
{
    int y = VENTANA_ALTO - ALTO_PANEL - MARGEN;
    int w = VENTANA_ANCHO - 2 * MARGEN;
    ventana_rect(v, MARGEN, y, w, ALTO_PANEL, C_CABECERA);
    ventana_rect_borde(v, MARGEN, y, w, ALTO_PANEL, 1, C_TENUE);

    const Aeronave *a = NULL;
    for (int i = 0; i < s->cantidad; i++)
    {
        if (s->items[i].id == id)
        {
            a = &s->items[i];
            break;
        }
    }

    if (a == NULL)
    {
        ventana_texto(v, MARGEN + 14, y + 44, 2, C_TENUE,
                      "PASE EL RATON O HAGA CLIC SOBRE UNA AERONAVE");
        return;
    }

    char txt[TXT_MAX];
    snprintf(txt, sizeof txt, "AV-%d   %s", a->id, a->modelo);
    ventana_texto(v, MARGEN + 14, y + 10, 3, C_SELEC, txt);

    snprintf(txt, sizeof txt, "RUTA: %s -> %s", a->origen, a->destino);
    ventana_texto(v, MARGEN + 14, y + 44, 2, C_TEXTO, txt);

    snprintf(txt, sizeof txt, "CAPACIDAD: %d%%     COMBUSTIBLE: %.2f",
             a->capacidad_porcentaje, a->combustible);
    ventana_texto(v, MARGEN + 14, y + 70, 2, C_TEXTO, txt);
}

void renderer_dibujar(Ventana *v, const EstadoSimulador *s,
                      int id_seleccionado, int raton_x, int raton_y)
{
    ventana_limpiar(v, C_FONDO);
    dibujar_cabecera(v, s);

    /* pista / zona de estacionamiento */
    int area_h = VENTANA_ALTO - ALTO_CABECERA - ALTO_PANEL - 3 * MARGEN;
    ventana_rect(v, MARGEN, ALTO_CABECERA + MARGEN, VENTANA_ANCHO - 2 * MARGEN,
                 area_h, C_ASFALTO);

    int hover_id = renderer_id_en(s, raton_x, raton_y);

    if (s->cantidad == 0)
    {
        const char *m = "NO HAY AERONAVES";
        int w = ventana_ancho_texto(m, 4);
        ventana_texto(v, (VENTANA_ANCHO - w) / 2,
                      ALTO_CABECERA + MARGEN + area_h / 2 - 14, 4, C_TENUE, m);
    }
    else
    {
        Cuadricula g = calcular_cuadricula(s->cantidad);
        for (int i = 0; i < s->cantidad; i++)
        {
            const Aeronave *a = &s->items[i];
            dibujar_casilla(v, &g, i, a, a->id == id_seleccionado,
                            a->id == hover_id);
        }
    }

    dibujar_panel(v, s, hover_id >= 0 ? hover_id : id_seleccionado);
    ventana_presentar(v);
}
