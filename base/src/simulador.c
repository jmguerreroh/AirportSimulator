/*
 * simulador.c - Simulador gráfico (PROPORCIONADO).
 *
 * Cómo funciona:
 *   - El servidor (tu código) llama a actualizar_simulador(&lista) cada vez que la lista cambia.
 *   - actualizar_simulador RECORRE tu lista (primero -> siguiente -> ...) y guarda una COPIA
 *     de las aeronaves. Se hace en el hilo del servidor, así que nunca coincide con tus cambios.
 *   - Un hilo propio dibuja esa copia unas 60 veces por segundo (ventana SDL2). Por eso la
 *     ventana no bloquea al servidor, y el servidor no necesita saber nada de hilos.
 */
#include "simulador.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "renderer.h"
#include "ventana.h"

#define ESPERA_FOTOGRAMA_US  (16 * 1000)
#define ESPERA_ARRANQUE_US   (10 * 1000)
#define ARRANQUE_MAX_ESPERAS 500           /* 5 s */
#define MAX_AERONAVES_SIM    10000         /* tope si la lista está mal enlazada (bucle infinito) */

enum { ARRANCANDO = 0, ACTIVO = 1, FALLO = -1, CERRADA = 2 };

static struct
{
    pthread_t hilo;
    pthread_mutex_t mutex;          /* protege la copia (uso interno del simulador) */
    int hilo_lanzado;
    volatile int parar;
    volatile int estado;
    Aeronave *items;                /* copia de la lista */
    int cantidad;
    int ordenada;
    unsigned long version;          /* sube con cada actualización */
} S = { .mutex = PTHREAD_MUTEX_INITIALIZER };

static void *hilo_grafico(void *arg)
{
    (void)arg;

    /* Ctrl+C lo gestiona solo el hilo principal */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    Ventana *ventana = ventana_crear("Aeropuerto - simulador", VENTANA_ANCHO, VENTANA_ALTO);
    if (ventana == NULL)
    {
        S.estado = FALLO;
        return NULL;
    }
    S.estado = ACTIVO;

    EstadoSimulador estado = { NULL, 0, 1 };      /* copia local que se dibuja */
    unsigned long version_vista = (unsigned long)-1;
    EntradaVentana entrada;
    memset(&entrada, 0, sizeof entrada);
    int seleccionado = -1;
    const char *captura = getenv("AEROPUERTO_CAPTURA");   /* solo para depurar */

    while (!S.parar)
    {
        ventana_eventos(ventana, &entrada);
        if (entrada.cerrar)
        {
            S.estado = CERRADA;
            break;
        }

        pthread_mutex_lock(&S.mutex);
        if (S.version != version_vista)
        {
            free(estado.items);
            estado.items = NULL;
            estado.cantidad = 0;
            if (S.cantidad > 0)
            {
                estado.items = malloc((size_t)S.cantidad * sizeof *estado.items);
                if (estado.items != NULL)
                {
                    memcpy(estado.items, S.items, (size_t)S.cantidad * sizeof *estado.items);
                    estado.cantidad = S.cantidad;
                }
            }
            estado.ordenada = S.ordenada;
            version_vista = S.version;
        }
        pthread_mutex_unlock(&S.mutex);

        if (entrada.click)
        {
            seleccionado = renderer_id_en(&estado, entrada.raton_x, entrada.raton_y);
        }

        renderer_dibujar(ventana, &estado, seleccionado, entrada.raton_x, entrada.raton_y);

        if (captura != NULL && version_vista != (unsigned long)-1)
        {
            ventana_guardar_bmp(ventana, captura);
        }
        usleep(ESPERA_FOTOGRAMA_US);
    }

    free(estado.items);
    ventana_destruir(ventana);
    return NULL;
}

int iniciar_simulador(void)
{
    if (S.hilo_lanzado)
    {
        return 0;
    }
    S.parar = 0;
    S.estado = ARRANCANDO;
    S.version = 0;
    if (pthread_create(&S.hilo, NULL, hilo_grafico, NULL) != 0)
    {
        return -1;
    }
    S.hilo_lanzado = 1;

    /* Esperar a saber si la ventana se pudo abrir */
    for (int i = 0; i < ARRANQUE_MAX_ESPERAS && S.estado == ARRANCANDO; i++)
    {
        usleep(ESPERA_ARRANQUE_US);
    }
    if (S.estado != ACTIVO)
    {
        detener_simulador();
        return -1;
    }
    return 0;
}

void actualizar_simulador(const ListaAeropuerto *lista)
{
    if (!S.hilo_lanzado || lista == NULL)
    {
        return;
    }

    /* Se recorre la lista SIN tener el mutex tomado: solo se toca tu lista en este hilo */
    Aeronave *copia = NULL;
    int n = 0;
    int ordenada = 1;
    if (lista->cantidad > 0)
    {
        int tope = lista->cantidad < MAX_AERONAVES_SIM ? lista->cantidad : MAX_AERONAVES_SIM;
        copia = malloc((size_t)tope * sizeof *copia);
        if (copia == NULL)
        {
            return;
        }
        for (const Nodo *actual = lista->primero; actual != NULL && n < tope;
             actual = actual->siguiente)
        {
            if (n > 0 && copia[n - 1].id > actual->aeronave.id)
            {
                ordenada = 0;
            }
            copia[n++] = actual->aeronave;
        }
    }

    pthread_mutex_lock(&S.mutex);
    free(S.items);
    S.items = copia;
    S.cantidad = n;
    S.ordenada = ordenada;
    S.version++;
    pthread_mutex_unlock(&S.mutex);
}

void detener_simulador(void)
{
    if (S.hilo_lanzado)
    {
        S.parar = 1;
        pthread_join(S.hilo, NULL);
        S.hilo_lanzado = 0;
    }
    pthread_mutex_lock(&S.mutex);
    free(S.items);
    S.items = NULL;
    S.cantidad = 0;
    pthread_mutex_unlock(&S.mutex);
}
