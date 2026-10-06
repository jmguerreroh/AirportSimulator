#include "comunicacion.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "estructuras.h"

int enviar_texto(int fd, const char *texto)
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: copia y adapta el de socket_c/server_peticiones.c (sección servidor/src/comunicacion.c) */
    /* PISTA: 'enviados' cuenta lo que ya se ha escrito; repite write() con el resto hasta llegar a strlen(texto) */
    /* PISTA: si write() devuelve -1 y errno es EINTR, reintenta; con otro error devuelve -1 */
    (void)fd; (void)texto; return -1;
    /*=================== FIN DE TU CODIGO ====================*/
}

int recibir_linea(int fd, char *buffer, int tam)
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: copia y adapta el de socket_c/server_peticiones.c (sección servidor/src/comunicacion.c) */
    /* PISTA: configura el tiempo máximo de espera con setsockopt(SO_RCVTIMEO) (ver comunicacion.h) */
    /* PISTA: bucle: read(fd, &c, 1); si devuelve 0 (conexión cerrada) o -1 (error/timeout): devuelve -1 */
    /* PISTA: si c es '\n' termina la línea; ignora los '\r'; no escribas más allá de tam-1 */
    (void)fd; (void)buffer; (void)tam; return -1;
    /*=================== FIN DE TU CODIGO ====================*/
}
