/*
 * comunicacion.h (CLIENTE) - Envío y recepción por el socket.      [A COMPLETAR]
 *
 * Siguiendo el ejemplo https://github.com/jmguerreroh/socket_c : read() y write().
 *
 * OJO: TCP es un flujo de bytes. Un write() puede enviar MENOS bytes de los que pides y
 * un read() puede devolver solo PARTE de la respuesta. Por eso estas dos funciones repiten
 * la llamada hasta terminar.
 */
#ifndef COMUNICACION_H
#define COMUNICACION_H

/*
 * Envía TODO el texto (los 'strlen(texto)' bytes) por el socket 'fd'.
 * Devuelve 0 si se envió completo y -1 si hubo un error.
 *
 * Ayuda: si write() devuelve menos de lo pedido, vuelve a llamar con el resto.
 */
int enviar_texto(int fd, const char *texto);

/*
 * Lee TODO lo que envía el servidor hasta que cierra la conexión y lo deja en 'buffer'
 * (tamaño 'tam'), terminado en '\0'.
 * Devuelve cuántos bytes se han recibido (0 si el servidor cerró sin enviar nada) y -1 si
 * hay un error de lectura. Si la respuesta no cabe, guarda lo que quepa (sin salirte del buffer).
 *
 * Ayuda: repite read() sobre el resto del buffer hasta que devuelva 0 (el servidor ha cerrado).
 */
int recibir_hasta_cierre(int fd, char *buffer, int tam);

#endif /* COMUNICACION_H */
