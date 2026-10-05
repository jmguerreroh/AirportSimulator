# Lo que te damos hecho

Todo lo demás lo programas tú. No hay funciones de ayuda: C básico (`stdio`, `string`, `stdlib`,
`sys/socket`, `unistd`). Entrada y salida: `printf`/`scanf` para el terminal, `fprintf`/`fscanf` para los
ficheros, `sscanf`/`snprintf` para texto en memoria y `read`/`write` para el socket.

## Estructuras y constantes (`servidor/include/estructuras.h`)

```c
typedef struct {
    int id;
    char origen[MAX_ORIGEN];  char destino[MAX_DESTINO];  char modelo[MAX_MODELO];
    int capacidad_porcentaje;          /* 0 a 100 */
    float combustible;                 /* >= 0 */
} Aeronave;

typedef struct Nodo {
    Aeronave aeronave;
    struct Nodo *anterior;
    struct Nodo *siguiente;
} Nodo;

typedef struct { Nodo *primero; Nodo *ultimo; int cantidad; } ListaAeropuerto;
```

Constantes: `MAX_ORIGEN` (64), `MAX_DESTINO` (64), `MAX_MODELO` (32), `MAX_PETICION` (256),
`TIMEOUT_RECEPCION_S` (3), `ARCHIVO_LOG` (`"aeropuerto.log"`). En el cliente
(`cliente/include/constantes.h`) están además `MAX_RESPUESTA` (32768).

## Simulador gráfico (`servidor/include/simulador.h`)

| Función | Qué hace |
|---|---|
| `int iniciar_simulador(void)` | abre la ventana (0 = bien, -1 = no se pudo). **La llamas tú al arrancar** |
| `void actualizar_simulador(const ListaAeropuerto *lista)` | refresca la ventana con tu lista. **Llámala después de cada petición** |
| `void detener_simulador(void)` | cierra la ventana. **Ya la llama `main` al terminar** |

`actualizar_simulador` **recorre tu lista** (`primero` → `siguiente` → …) y guarda una copia; no
la modifica. Una casilla por aeronave, en el mismo orden que tu lista: si un puntero está mal, se
verá. Sin ventana (sin entorno gráfico) no hace nada.

## Prototipos que tienes que implementar (`comunicacion.h`)

Son las **únicas** funciones cuyos prototipos te damos. Las demás (la lista, una función por
cada opción, el registro...) las diseñas tú, con los nombres y parámetros que quieras.

**Servidor** (`servidor/include/comunicacion.h`):

```c
int enviar_texto(int fd, const char *texto);        /* escribe TODO el texto; 0 = bien, -1 = error */
int recibir_linea(int fd, char *buffer, int tam);   /* lee hasta '\n'; devuelve la longitud, -1 = error */
```

**Cliente** (`cliente/include/comunicacion.h`):

```c
int enviar_texto(int fd, const char *texto);                    /* igual que en el servidor */
int recibir_hasta_cierre(int fd, char *buffer, int tam);        /* lee hasta que el servidor cierra */
```

## Ya hecho en `main`

* **Servidor:** solo que **Ctrl+C** pida cerrar (`g_parar`), ignorar `SIGPIPE` y llamar a
  `detener_simulador()` al terminar.
* **Cliente:** nada. Los argumentos, el menú, la lectura de datos del teclado y las opciones los
  programas tú.
