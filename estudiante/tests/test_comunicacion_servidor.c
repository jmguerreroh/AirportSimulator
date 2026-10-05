/*
 * Comprobaciones de servidor/src/comunicacion.c: enviar_texto() y recibir_linea().
 * Se usa socketpair(), que se comporta como una conexión TCP local.
 */
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "comunicacion.h"
#include "estructuras.h"
#include "test_util.h"

static void test_una_linea(void)
{
    int sv[2];
    CHECK_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    char linea[MAX_PETICION];

    CHECK_EQ(enviar_texto(sv[0], "MOSTRAR\n"), 0);
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), 7);
    CHECK(strcmp(linea, "MOSTRAR") == 0);
    close(sv[0]);
    close(sv[1]);
}

static void test_cr_y_vacia(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    char linea[MAX_PETICION];

    enviar_texto(sv[0], "ORDENAR\r\n\nSALIR\n");
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), 7);   /* el '\r' no cuenta */
    CHECK(strcmp(linea, "ORDENAR") == 0);
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), 0);   /* línea vacía */
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), 5);   /* la siguiente línea sigue ahí */
    CHECK(strcmp(linea, "SALIR") == 0);
    close(sv[0]);
    close(sv[1]);
}

static void test_mensaje_en_trozos(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    char linea[MAX_PETICION];

    if (fork() == 0)
    {
        close(sv[1]);
        enviar_texto(sv[0], "ANADIR 1 Mad");
        usleep(150 * 1000);
        enviar_texto(sv[0], "rid Paris A320 5");
        usleep(150 * 1000);
        enviar_texto(sv[0], "0 1.5\n");
        _exit(0);
    }
    close(sv[0]);
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), 33);
    CHECK(strcmp(linea, "ANADIR 1 Madrid Paris A320 50 1.5") == 0);
    close(sv[1]);
    wait(NULL);
}

static void test_errores(void)
{
    int sv[2];
    char linea[MAX_PETICION];
    char pequeno[5];

    /* la línea no cabe en el buffer */
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    enviar_texto(sv[0], "UNA LINEA MUY LARGA\n");
    CHECK_EQ(recibir_linea(sv[1], pequeno, sizeof pequeno), -1);
    close(sv[0]);
    close(sv[1]);

    /* el cliente cierra antes de terminar la línea */
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    enviar_texto(sv[0], "ANADIR 1 Mad");
    close(sv[0]);
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), -1);
    close(sv[1]);

    /* el cliente se conecta y no envía nada: debe vencer el tiempo (unos 3 s) */
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    CHECK_EQ(recibir_linea(sv[1], linea, sizeof linea), -1);
    close(sv[0]);
    close(sv[1]);
}

static void test_envio_grande_y_cierre(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    enum { N = 600000 };
    static char datos[N + 1];
    memset(datos, 'x', N);
    datos[N] = '\0';

    /* write() parciales: mucho más de lo que cabe en el socket */
    if (fork() == 0)
    {
        close(sv[1]);
        enviar_texto(sv[0], datos);
        _exit(0);
    }
    close(sv[0]);
    size_t total = 0;
    char buf[4096];
    ssize_t r;
    while ((r = read(sv[1], buf, sizeof buf)) > 0) total += (size_t)r;
    CHECK_EQ(total, (size_t)N);
    close(sv[1]);
    wait(NULL);

    /* enviar a un cliente que ya se fue: error, no una caída del programa */
    signal(SIGPIPE, SIG_IGN);
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    close(sv[1]);
    CHECK_EQ(enviar_texto(sv[0], "hola\n"), -1);
    close(sv[0]);
}

int main(void)
{
    INICIO_TESTS();
    EJECUTAR(test_una_linea);
    EJECUTAR(test_cr_y_vacia);
    EJECUTAR(test_mensaje_en_trozos);
    EJECUTAR(test_errores);
    EJECUTAR(test_envio_grande_y_cierre);
    RESUMEN("test_comunicacion_servidor");
}
