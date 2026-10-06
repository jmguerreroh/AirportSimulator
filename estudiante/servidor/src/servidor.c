/*
 * servidor.c - Programa "aeropuerto" (servidor).
 *
 *   ./aeropuerto PUERTO [FICHERO]
 *
 * Sigue la estructura de https://github.com/jmguerreroh/socket_c (server.c):
 *     socket() -> bind() -> listen() -> accept() -> read()/write() -> close()
 * pero aquí el cliente se conecta POR CADA PETICIÓN, así que el servidor repite:
 *
 *     accept -> leer UNA petición -> hacer lo que pide -> enviar la respuesta -> close
 *
 * hasta que llega la petición SALIR (o se pulsa Ctrl+C).
 *
 * Marcas:  [DADO] ya está hecho.   [ESTUDIANTE] lo programas tú (busca "TODO (estudiante)").
 */
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "comunicacion.h"
#include "estructuras.h"
#include "lista.h"
#include "simulador.h"

/* ---------------------------------------------------------------------------------------- */
/* [DADO] Ctrl+C                                                                           */
/* ---------------------------------------------------------------------------------------- */

static volatile sig_atomic_t g_parar = 0;   /* 1 => hay que cerrar el servidor */
static volatile sig_atomic_t g_senal = 0;   /* 1 => ha sido por Ctrl+C */

/* En un manejador de señal solo se pueden hacer cosas muy simples. */
static void manejador_senal(int signo)
{
    (void)signo;
    g_senal = 1;
    g_parar = 1;
}

/* ---------------------------------------------------------------------------------------- */
/* [ESTUDIANTE] Registro, una función por opción del menú y la respuesta                        */
/* ---------------------------------------------------------------------------------------- */

/*================== INICIO DE TU CODIGO ==================*/
/* TODO (estudiante): implementar. */
/* PISTA: aquí van TUS funciones (nombres y parámetros los eliges tú). Necesitas, como mínimo: */
/* PISTA:   - una que muestre el mensaje de uso (ver docs/ENUNCIADO.md, sección 3) y otra que valide el puerto; */
/* PISTA:   - una para escribir una línea en aeropuerto.log (fopen "a", fprintf, fflush); */
/* PISTA:   - una función POR CADA opción: añadir, eliminar, modificar, ordenar, mostrar y salir; */
/* PISTA:     cada una recibe la petición, trabaja con la lista y deja un mensaje de resultado; */
/* PISTA:   - una que, según la primera palabra de la petición (strcmp), llame a la función de la opción; */
/* PISTA:   - una que envíe la respuesta completa con enviar_texto() (ver docs/PROTOCOLO.md): 1.ª llamada la línea OK:/ERROR:, */
/* PISTA:     2.ª la cabecera "--- ESTADO ACTUAL: n aeronaves ---" y después una llamada por cada aeronave de la lista; */
/* PISTA: puedes leer los datos de una petición con sscanf(), por ejemplo: sscanf(linea, "ANADIR %d %63s ...", ...) */
/*=================== FIN DE TU CODIGO ====================*/

/* ---------------------------------------------------------------------------------------- */
/* main                                                                                     */
/* ---------------------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: leer y validar los argumentos de la línea de comandos (argc y argv) */
    /* PISTA: ./aeropuerto PUERTO [FICHERO]: el puerto es obligatorio (1 a 65535) y el fichero, opcional */
    /* PISTA: si falta el puerto, no es válido o sobra algo, muestra el mensaje de uso (ver docs/ENUNCIADO.md, sección 3) y termina con EXIT_FAILURE */
    /* PISTA: si se da un FICHERO, comprueba que se puede abrir (fopen) y, si no, avisa y termina con EXIT_FAILURE */
    /* PISTA: guarda el resultado en variables tuyas (puerto, fichero_inicial...) que usarás más abajo */
    (void)argc; (void)argv;
    /*=================== FIN DE TU CODIGO ====================*/

    /* [DADO] Ctrl+C pide cerrar el servidor. Sin SA_RESTART, accept() se interrumpe y el bucle
     * puede comprobar g_parar. Si un cliente se va a mitad de la respuesta, no morimos por SIGPIPE. */
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = manejador_senal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: preparar la lista y el registro */
    /* PISTA: declara tu ListaAeropuerto, inicialízala vacía y abre el registro (aeropuerto.log) */
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: arrancar el simulador gráfico */
    /* PISTA: iniciar_simulador() devuelve 0 si se abrió la ventana; si devuelve -1, avisa y sigue sin ventana */
    /* PISTA: al terminar el programa ya se llama a detener_simulador() más abajo (código dado) */
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: si se ha indicado un FICHERO (fichero_inicial no es NULL), cargar sus aeronaves en la lista */
    /* PISTA: formato: una aeronave por línea, "id origen destino modelo capacidad combustible"; ver docs/PROTOCOLO.md */
    /* PISTA: lee el fichero con fscanf() (por ejemplo, una línea entera con "%255[^\n]"); aplica las MISMAS validaciones que al añadir; ignora líneas vacías y las que empiezan por '#' */
    /* PISTA: una línea incorrecta no detiene la carga: se salta y se apunta en el registro */
    /* PISTA: al terminar, apunta en el registro cuántas se cargaron y llama a actualizar_simulador(&lista) */
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: crear el socket de escucha (como server.c de socket_c) */
    /* PISTA: copia el PASO A de socket_c/server_peticiones.c y adapta lo que marca */
    /* PISTA: socket(AF_INET, SOCK_STREAM, 0); setsockopt(SO_REUSEADDR); bind() al puerto; listen() */
    /* PISTA: struct sockaddr_in: sin_family = AF_INET, sin_addr.s_addr = htonl(INADDR_ANY), sin_port = htons(puerto) */
    /* PISTA: comprueba el resultado de CADA llamada; si falla, muestra el motivo (perror) y termina */
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: bucle principal: aceptar, leer la petición, atenderla, responder y cerrar */
    /* PISTA: copia el PASO B de socket_c/server_peticiones.c: ya trae accept, IP:puerto y recibir_linea; tú pones lo del hueco */
    /* PISTA: repite mientras no haya que cerrar (g_parar), y en cada vuelta, por este orden: */
    /* PISTA:   1. accept()           -> socket del cliente (y su IP y puerto, para el registro) */
    /* PISTA:   2. recibir_linea()    -> la petición; si devuelve -1: apúntalo en el registro, close() y sigue */
    /* PISTA:   3. mira la primera palabra y llama a TU función de esa opción: trabaja con la lista y deja el mensaje */
    /* PISTA:   4. enviar_texto()     -> el resultado y el estado actual (tu función de respuesta) */
    /* PISTA:   5. actualizar_simulador(&lista)   -> refresca la ventana con la lista actual */
    /* PISTA:   6. close() del socket del cliente (el controlador espera a que cierres para dejar de leer) */
    /* PISTA: si accept() falla (por ejemplo, por Ctrl+C), vuelve a comprobar g_parar */
    /* PISTA: apunta en el registro cada petición recibida y la primera línea de la respuesta */
    /* PISTA: accept() puede decirte quién se ha conectado: pásale una struct sockaddr_in y su tamaño (socklen_t) */
    /* PISTA: la IP en texto con inet_ntop(AF_INET, &dir.sin_addr, ip, sizeof ip) y el puerto con ntohs(dir.sin_port) */
    /* PISTA: cada línea del registro que se deba a un cliente empieza por su IP y puerto: "[127.0.0.1:50422] Petición: ..." */
    /*=================== FIN DE TU CODIGO ====================*/

    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: PASO: cierre ordenado y liberación de memoria */
    /* PISTA: cierra el socket de escucha, libera TODOS los nodos de la lista y cierra el registro */
    /* PISTA: apunta en el registro el motivo del cierre (Ctrl+C o SALIR) y que el servidor ha finalizado */
    /* PISTA: al terminar imprime "Memoria liberada correctamente." y "Servidor finalizado." */
    /*=================== FIN DE TU CODIGO ====================*/

    /* [DADO] Cierra la ventana y libera lo que usa el simulador */
    detener_simulador();
    return EXIT_SUCCESS;
}
