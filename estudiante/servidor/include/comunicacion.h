/*
 * comunicacion.h (SERVIDOR) - Envío y recepción por el socket.      [A COMPLETAR]
 *
 * Siguiendo el ejemplo https://github.com/jmguerreroh/socket_c : read() y write().
 *
 * OJO: TCP es un flujo de bytes. Un write() puede enviar MENOS bytes de los que pides y
 * un read() puede devolver solo PARTE del mensaje. Por eso estas dos funciones repiten
 * la llamada hasta terminar.
 */
#ifndef COMUNICACION_H
#define COMUNICACION_H

/*
 * Envía TODO el texto (los 'strlen(texto)' bytes) por el socket 'fd'.
 * Devuelve 0 si se envió completo y -1 si hubo un error (por ejemplo, el cliente se fue).
 *
 * Ayuda: si write() devuelve menos de lo pedido, vuelve a llamar con el resto.
 */
int enviar_texto(int fd, const char *texto);

/*
 * Lee UNA línea (hasta el '\n') del socket 'fd' y la deja en 'buffer' (tamaño 'tam'),
 * SIN el '\n' ni un '\r' anterior, y terminada en '\0'.
 * Devuelve cuántos caracteres tiene la línea (0 si estaba vacía) y -1 si hay un error:
 * la conexión se cerró antes del '\n', la línea no cabe en 'buffer', o pasaron
 * TIMEOUT_RECEPCION_S segundos sin recibir nada.
 *
 * Ayuda: lee de byte en byte con read(fd, &c, 1) hasta encontrar '\n'.
 * Ayuda: para el límite de tiempo, antes de leer:
 *     struct timeval t = { TIMEOUT_RECEPCION_S, 0 };
 *     setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof t);
 */
int recibir_linea(int fd, char *buffer, int tam);

#endif /* COMUNICACION_H */
