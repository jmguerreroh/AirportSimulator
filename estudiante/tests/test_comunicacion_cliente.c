/*
 * Comprobaciones de cliente/src/comunicacion.c: enviar_texto() y recibir_hasta_cierre().
 * Se usa socketpair(), que se comporta como una conexión TCP local.
 */
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "comunicacion.h"
#include "constantes.h"
#include "test_util.h"

static void test_envio(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    char buf[64];

    CHECK_EQ(enviar_texto(sv[0], "ANADIR 1 a b c 5 1.0\n"), 0);
    ssize_t n = read(sv[1], buf, sizeof buf - 1);
    CHECK_EQ(n, 21);
    buf[n > 0 ? n : 0] = '\0';
    CHECK(strcmp(buf, "ANADIR 1 a b c 5 1.0\n") == 0);
    close(sv[0]);
    close(sv[1]);

    /* enviar a un servidor que ya cerró: error, no una caída del programa */
    signal(SIGPIPE, SIG_IGN);
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    close(sv[1]);
    CHECK_EQ(enviar_texto(sv[0], "hola\n"), -1);
    close(sv[0]);
}

static void test_respuesta_en_trozos(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    char buf[MAX_RESPUESTA];

    if (fork() == 0)
    {
        close(sv[1]);
        enviar_texto(sv[0], "OK: Aeronave 1 a");
        usleep(150 * 1000);
        enviar_texto(sv[0], "ñadida correctamente.\n--- ESTADO ACTUAL: 1 aeronaves ---\n");
        usleep(150 * 1000);
        enviar_texto(sv[0], "ID 1 | a -> b | c | 5% | 1.0\n");
        close(sv[0]);               /* el servidor cierra: respuesta completa */
        _exit(0);
    }
    close(sv[0]);
    int n = recibir_hasta_cierre(sv[1], buf, sizeof buf);
    CHECK(n > 0);
    CHECK_EQ((size_t)n, strlen(buf));
    CHECK(strncmp(buf, "OK: Aeronave 1 a", 16) == 0);
    CHECK(strstr(buf, "--- ESTADO ACTUAL: 1 aeronaves ---\n") != NULL);
    CHECK(strcmp(buf + strlen(buf) - 29, "ID 1 | a -> b | c | 5% | 1.0\n") == 0);
    close(sv[1]);
    wait(NULL);
}

static void test_vacia_y_truncada(void)
{
    int sv[2];
    char buf[MAX_RESPUESTA];

    /* el servidor cierra sin enviar nada */
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    close(sv[0]);
    CHECK_EQ(recibir_hasta_cierre(sv[1], buf, sizeof buf), 0);
    CHECK_EQ(buf[0], '\0');
    close(sv[1]);

    /* la respuesta no cabe: se guarda lo que quepa, terminado en '\0' y sin salirse */
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    enviar_texto(sv[0], "0123456789ABCDEF");
    close(sv[0]);
    char pequeno[8];
    memset(pequeno, 'Z', sizeof pequeno);
    int n = recibir_hasta_cierre(sv[1], pequeno, sizeof pequeno);
    CHECK_EQ(n, 7);
    CHECK(strcmp(pequeno, "0123456") == 0);
    close(sv[1]);
}

static void test_respuesta_grande(void)
{
    int sv[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    enum { N = 30000 };
    static char datos[N + 1];
    static char buf[MAX_RESPUESTA];
    memset(datos, 'x', N);
    datos[N] = '\0';

    if (fork() == 0)
    {
        close(sv[1]);
        enviar_texto(sv[0], datos);
        close(sv[0]);
        _exit(0);
    }
    close(sv[0]);
    CHECK_EQ(recibir_hasta_cierre(sv[1], buf, sizeof buf), N);
    close(sv[1]);
    wait(NULL);
}

int main(void)
{
    INICIO_TESTS();
    EJECUTAR(test_envio);
    EJECUTAR(test_respuesta_en_trozos);
    EJECUTAR(test_vacia_y_truncada);
    EJECUTAR(test_respuesta_grande);
    RESUMEN("test_comunicacion_cliente");
}
