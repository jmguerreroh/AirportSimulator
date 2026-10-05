/*
 * estructuras.h - Estructuras y constantes del servidor. [DADO: no lo modifiques]
 *
 * La lista es doblemente enlazada:
 *
 *           primero                                  ultimo
 *              |                                       |
 *              v                                       v
 *          +------+ siguiente  +------+ siguiente  +------+
 *          | 105  | ---------> | 101  | ---------> | 103  | --> NULL
 * NULL <-- |      | <--------- |      | <--------- |      |
 *          +------+  anterior  +------+  anterior  +------+
 *
 * El simulador gráfico recorre TU lista (primero -> siguiente -> ...), así que
 * si algún puntero está mal, se verá en pantalla.
 */
#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/* Tamaños máximos de los textos (incluyen el '\0' final) */
#define MAX_ORIGEN   64
#define MAX_DESTINO  64
#define MAX_MODELO   32

/* Tamaño máximo de una petición (una línea, con el '\n' final) */
#define MAX_PETICION 256

/* Segundos que el servidor espera a que un cliente envíe su petición */
#define TIMEOUT_RECEPCION_S 3

/* Fichero donde el servidor escribe lo que va ocurriendo */
#define ARCHIVO_LOG "aeropuerto.log"

typedef struct
{
    int id;                         /* único dentro del aeropuerto */

    char origen[MAX_ORIGEN];
    char destino[MAX_DESTINO];
    char modelo[MAX_MODELO];

    int capacidad_porcentaje;       /* 0 a 100 */

    float combustible;              /* >= 0 */

} Aeronave;

typedef struct Nodo
{
    Aeronave aeronave;

    struct Nodo *anterior;
    struct Nodo *siguiente;

} Nodo;

typedef struct
{
    Nodo *primero;
    Nodo *ultimo;
    int cantidad;

} ListaAeropuerto;

#endif /* ESTRUCTURAS_H */
