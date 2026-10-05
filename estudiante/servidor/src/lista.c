/*
 * lista.c - La lista doblemente enlazada de aeronaves.   [A COMPLETAR]
 *
 * Aquí está la memoria dinámica y los punteros de la práctica:
 * crear nodos (malloc), enlazarlos, ordenarlos, desenlazarlos y liberarlos (free).
 *
 * Qué debe poder hacer tu lista (los nombres y parámetros los eliges tú):
 *   - inicializarla vacía;
 *   - añadir una aeronave AL FINAL (sin repetir IDs);
 *   - buscar una aeronave por su ID;
 *   - eliminar una aeronave por su ID (actualizando los punteros de los vecinos);
 *   - saber si está ordenada por ID y ordenarla REENLAZANDO nodos (sin copiar datos);
 *   - vaciarla liberando TODOS los nodos.
 */
#include "lista.h"

#include <stdlib.h>

/*================== INICIO DE TU CODIGO ==================*/
/* TODO (estudiante): implementar. */
/* PISTA: añadir: malloc del nodo (¡comprueba NULL!), anterior = ultimo, siguiente = NULL; si la lista estaba vacía primero = nuevo */
/* PISTA: eliminar: si el nodo tiene anterior, anterior->siguiente salta al nodo; si no, cambia lista->primero (y lo mismo con siguiente/ultimo) */
/* PISTA: vaciar: guarda nodo->siguiente ANTES de hacer free(nodo) */
/* PISTA: ordenar: ve sacando los nodos de la lista vieja y metiéndolos en su sitio en una lista nueva (inserción ordenada) */
/*=================== FIN DE TU CODIGO ====================*/
