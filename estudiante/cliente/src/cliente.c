/*
 * cliente.c - Programa "controlador" (cliente).
 *
 *   ./controlador IP PUERTO
 *
 * Sigue la estructura de https://github.com/jmguerreroh/socket_c (client.c):
 *     socket() -> connect() -> write() -> read() -> close()
 * pero aquí el cliente se conecta POR CADA PETICIÓN:
 *
 *     conectar -> enviar UNA petición -> leer la respuesta hasta que el servidor cierre -> cerrar
 *
 * Marcas:  [DADO] ya está hecho.   [ESTUDIANTE] lo programas tú (busca "TODO (estudiante)").
 *
 * Lo tuyo: leer los argumentos, el menú, leer los datos del teclado, construir y hacer cada petición.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "comunicacion.h"
#include "constantes.h"

/* ---------------------------------------------------------------------------------------- */
/* [ESTUDIANTE] Menú y lectura de datos del teclado                                        */
/* ---------------------------------------------------------------------------------------- */

/*================== INICIO DE TU CODIGO ==================*/
/* TODO (estudiante): implementar. */
/* PISTA: aquí van TUS funciones del menú y de la lectura de datos (nombres y parámetros los eliges tú). */
/* PISTA: el menú (ver docs/ENUNCIADO.md, sección 7): 7 opciones numeradas y "Seleccione una opción: ". */
/* PISTA: muestra con printf() y lee del teclado con scanf() ("%d", "%f", "%63s"...): nunca gets(). */
/* PISTA: scanf() devuelve cuántos datos ha leído, y EOF cuando se acaba la entrada (Ctrl+D). */
/* PISTA: si el dato no es válido (scanf devuelve 0), el texto escrito sigue sin consumir: descártalo con scanf("%*[^\n]") antes de volver a preguntar */
/* PISTA: (el compilador avisa si ignoras lo que devuelve scanf: guárdalo en una variable y escribe (void)variable;) */
/* PISTA: los textos (origen, destino, modelo) son UNA palabra sin espacios y deben caber en su array. */
/* PISTA: contempla que se acabe la entrada (scanf devuelve EOF con Ctrl+D): termina el programa sin cerrar el servidor. */
/* PISTA: pide los datos de cada opción y vuelve a preguntar si el valor no es válido (número mal escrito...). */
/*=================== FIN DE TU CODIGO ====================*/

/* ---------------------------------------------------------------------------------------- */
/* [ESTUDIANTE] Una petición = una conexión                                                     */
/* ---------------------------------------------------------------------------------------- */

/*================== INICIO DE TU CODIGO ==================*/
/* TODO (estudiante): implementar. */
/* PISTA: aquí van TUS funciones: la que muestra el mensaje de uso (ver docs/ENUNCIADO.md, sección 3) y la que hace UNA petición completa: */
/* PISTA: tu función de hacer una petición hace, por este orden: */
/* PISTA: copia el BLOQUE C de socket_c/client_peticiones.c y adáptalo; lo siguiente es lo que hace: */
/* PISTA:   1. socket(AF_INET, SOCK_STREAM, 0) */
/* PISTA:   2. rellena una struct sockaddr_in: sin_family = AF_INET, sin_port = htons(puerto) y la IP con inet_pton() */
/* PISTA:   3. connect() al servidor; si falla, avisa (¿está el servidor en marcha?), close(fd) y devuelve un error */
/* PISTA:   4. enviar_texto(fd, petición)      -> envía la petición (una línea terminada en '\n') */
/* PISTA:   5. recibir_hasta_cierre(fd, ...)   -> lee la respuesta completa en un array (MAX_RESPUESTA) */
/* PISTA:   6. printf() de la respuesta */
/* PISTA:   7. close(fd)   -> cada petición usa su propio socket */
/*=================== FIN DE TU CODIGO ====================*/

/* ---------------------------------------------------------------------------------------- */
/* main                                                                                     */
/* ---------------------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: leer y validar los argumentos de la línea de comandos (argc y argv) */
    /* PISTA: ./controlador IP PUERTO: hacen falta exactamente dos; la IP debe ser válida (inet_pton) y el puerto estar entre 1 y 65535 */
    /* PISTA: si falta algo o no es válido, muestra el mensaje de uso (ver docs/ENUNCIADO.md, sección 3) y termina con EXIT_FAILURE */
    /* PISTA: guarda la IP y el puerto en variables tuyas, que usarás en cada petición */
    (void)argc; (void)argv;
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: estado inicial y bucle del menú */
    /* PISTA: al arrancar, pide el estado con una petición MOSTRAR (así el usuario ve el aeropuerto) */
    /* PISTA: bucle: mostrar el menú, leer la opción y, según ella, pedir los datos al usuario */
    /* PISTA:        (con tus funciones de lectura del teclado) y construir la petición con snprintf */
    /* PISTA: formato de cada petición en docs/PROTOCOLO.md; cada petición termina en '\n' */
    /* PISTA: 6 = SALIR (el servidor se cierra) y termina; 7 = termina sin avisar al servidor */
    /*=================== FIN DE TU CODIGO ====================*/

    return EXIT_SUCCESS;
}
