# AirportSimulator — práctica de sockets TCP en C con simulador gráfico de aeropuerto

Práctica de **sockets TCP** y **listas doblemente enlazadas** en C básico. Un **servidor** guarda las aeronaves
de un aeropuerto en una lista que programa quien hace la práctica, atiende las peticiones de un **cliente** de terminal
(`controlador`) y las muestra en una **ventana gráfica** en tiempo real (SDL2), dejando constancia en un fichero
`.log`. El cliente **se conecta en cada petición**, como en el ejemplo
[`socket_c`](https://github.com/jmguerreroh/socket_c).

> El simulador gráfico y las estructuras ya están hechos. El estudiantado implementa **el envío y la recepción por el
> socket, el servidor (una función por cada opción, y la carga de un fichero inicial de aeronaves), la lista
> doblemente enlazada, el cliente (menú, lectura de datos y opciones incluidos), la lectura de los argumentos de ambos programas, el registro y la
> liberación de memoria**. C básico: no necesita saber nada de hilos ni de mutex.

Lee [docs/ENUNCIADO.md](docs/ENUNCIADO.md) (enunciado completo) y [docs/PROTOCOLO.md](docs/PROTOCOLO.md) (los
mensajes que se envían en cada sentido).

## Cómo empezar (lo más sencillo)

Crea una carpeta vacía con **un solo fichero**, `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MiAeropuerto C)

include(FetchContent)
FetchContent_Declare(Aeropuerto
  GIT_REPOSITORY https://github.com/jmguerreroh/AirportSimulator.git
  GIT_TAG main)
FetchContent_MakeAvailable(Aeropuerto)

aeropuerto_practica()
```

Y ejecuta **una sola vez**:

```bash
cmake -S . -B build          # SOLO descarga y prepara: CREA EN TU CARPETA las plantillas
```

Después **completas el `Makefile`** (la parte tuya, busca `TODO`) y compilas con `make`:

```bash
make
./aeropuerto 5000            # terminal 1: se abre la ventana
./aeropuerto 5000 ejemplos/aeropuerto_inicial.txt   # ...o cargando aeronaves de un fichero
./controlador 127.0.0.1 5000 # terminal 2, 3...
```

Tras el primer `cmake` tu carpeta queda así:

```
MiAeropuerto/
├── CMakeLists.txt
├── Makefile                  ← A COMPLETAR (el simulador ya está hecho; tú: servidor/ y cliente/)
├── config.mk                 ← lo regenera CMake (ruta de lo descargado, SDL2)
├── ENUNCIADO.md
├── cliente/
│   ├── include/   comunicacion.h  constantes.h
│   └── src/       cliente.c  comunicacion.c
├── servidor/
│   ├── include/   estructuras.h  simulador.h  comunicacion.h  lista.h
│   └── src/       servidor.c  lista.c  comunicacion.c
├── tests/                    ← comprobaciones y pruebas de sistema
├── docs/                     ← PROTOCOLO.md, API.md
└── ejemplos/                 ← aeropuerto_inicial.txt y rellenar.sh
```

Busca `TODO (estudiante)` en `cliente/`, `servidor/` y el `Makefile`: tu código va entre las líneas `INICIO DE TU CODIGO` y
`FIN DE TU CODIGO`. Esos ficheros son **tuyos**: CMake no los
vuelve a tocar (solo regenera `config.mk`). Si no tienes SDL2 instalado, CMake lo descarga y lo compila él solo.

## Requisitos

Compilador C11, CMake ≥ 3.16, `make`, `git`, `pkg-config`. Opcional: `libsdl2-dev` (si falta, CMake lo descarga),
`netcat` (`nc`) y `valgrind`.

```bash
# Ubuntu / Debian (una sola vez)
sudo apt install build-essential cmake git pkg-config libsdl2-dev netcat-openbsd valgrind
```

## Objetivos del Makefile

| Orden | Qué hace |
|---|---|
| `make` | compila `./aeropuerto` y `./controlador` (cuando hayas completado el `Makefile`) |
| `make tests` | comprueba `enviar_texto`, `recibir_linea` y `recibir_hasta_cierre` (al principio fallan) |
| `make prueba` | tu servidor real + `nc` (una conexión por petición): formato de las respuestas, cierre con `SALIR` y `aeropuerto.log` |
| `make memoria` | tu servidor real bajo valgrind: no debe dejar memoria sin liberar |
| `make SIN_SDL=1` | sin ventana gráfica |
| `make clean` | limpia |

## Qué hay en el repositorio

```
base/             el simulador gráfico (SDL2) que descarga CMake
estudiante/        plantillas que CMake copia a la carpeta de trabajo (cliente/, servidor/, Makefile, tests/)
cmake/            aeropuerto_practica(): descarga y prepara
docs/             ENUNCIADO, PROTOCOLO, API
examples/         aeropuerto_inicial.txt (fichero de ejemplo) y rellenar.sh
```

## Cómo funciona (resumen)

```text
 controlador                                   aeropuerto (servidor)
 socket() + connect() ────────────────────────▶ accept()
 write("ANADIR 103 Madrid ...\n") ────────────▶ lee la línea ─▶ llama a la función de la opción
                                                modifica la lista ─▶ actualiza el simulador
 read() hasta que cierre ◀── resultado + estado ─ write() ... y close()
 close()
```

* Mensajes de texto: `ANADIR id origen destino modelo capacidad combustible`, `ELIMINAR id`,
  `MODIFICAR id campo valor`, `ORDENAR`, `MOSTRAR`, `SALIR`. La respuesta siempre trae el resultado y el estado
  actual de la lista.
* El simulador **recorre tu lista** cuando llamas a `actualizar_simulador(&lista)` y dibuja una
  copia: una casilla por aeronave, en el orden de la lista.
* Ctrl+C **o** una petición `SALIR` cierran el servidor de forma ordenada (ventana, lista, sockets, log).

API proporcionada: [docs/API.md](docs/API.md). Licencia: [MIT](LICENSE).
