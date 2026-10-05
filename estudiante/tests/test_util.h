/*
 * test_util.h - Mini-framework de comprobaciones (sin dependencias externas).
 */
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int g_total = 0;
static int g_fallos = 0;

/*
 * Si una comprobación se queda esperando (por ejemplo, porque tu función todavía no envía lo
 * que debería y el test lee de un socket que nunca recibe nada), se aborta a los 10 segundos
 * con un mensaje en lugar de quedarse colgada para siempre.
 */
static void alarma_tiempo_agotado(int signo)
{
    static const char MSG[] = "\n  TIEMPO AGOTADO: la comprobacion se quedo esperando "
                              "(funcion sin implementar o bucle infinito?)\n";
    (void)signo;
    if (write(1, MSG, sizeof MSG - 1) < 0)
    {
    }
    _exit(1);
}

#define INICIO_TESTS()                                                     \
    do {                                                                   \
        setvbuf(stdout, NULL, _IOLBF, 0);                                  \
        signal(SIGALRM, alarma_tiempo_agotado);                            \
        alarm(10);                                                         \
    } while (0)

#define CHECK(cond)                                                        \
    do {                                                                   \
        g_total++;                                                         \
        if (!(cond)) {                                                     \
            g_fallos++;                                                    \
            printf("  FALLO %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                  \
    } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))

#define EJECUTAR(fn)                                                       \
    do {                                                                   \
        int antes = g_fallos;                                              \
        fn();                                                              \
        printf("[%s] %s\n", g_fallos == antes ? " OK " : "FAIL", #fn);     \
    } while (0)

#define RESUMEN(nombre)                                                    \
    do {                                                                   \
        printf("%s: %d comprobaciones, %d fallos\n", nombre, g_total,      \
               g_fallos);                                                  \
        return g_fallos == 0 ? 0 : 1;                                      \
    } while (0)

#endif /* TEST_UTIL_H */
