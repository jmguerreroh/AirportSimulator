#include "comunicacion.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

int enviar_texto(int fd, const char *texto)
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: 'enviados' cuenta lo que ya se ha escrito; repite write() con el resto hasta llegar a strlen(texto) */
    /* PISTA: si write() devuelve -1 y errno es EINTR, reintenta; con otro error devuelve -1 */
    (void)fd; (void)texto; return -1;
    /*=================== FIN DE TU CODIGO ====================*/
}

int recibir_hasta_cierre(int fd, char *buffer, int tam)
{
    /*================== INICIO DE TU CODIGO ==================*/
    /* TODO (estudiante): implementar. */
    /* PISTA: lee con read(fd, buffer + recibidos, tam - 1 - recibidos) en un bucle */
    /* PISTA: read() devuelve 0 cuando el servidor cierra: ahí termina; -1 es un error (reintenta si errno es EINTR) */
    /* PISTA: al final pon '\0' en buffer[recibidos] */
    (void)fd; (void)buffer; (void)tam; return -1;
    /*=================== FIN DE TU CODIGO ====================*/
}
