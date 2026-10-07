# AirportSimulator — práctica de sockets TCP en C con simulador gráfico de aeropuerto

Práctica de **sockets TCP** y **listas doblemente enlazadas** en C básico. Un **servidor** guarda las aeronaves
de un aeropuerto en una lista que programa quien hace la práctica, atiende las peticiones de un **cliente** de terminal
(`controlador`) y las muestra en una **ventana gráfica** en tiempo real (SDL2), dejando constancia en un fichero
`.log`. El cliente **se conecta en cada petición**, como en el ejemplo
[`socket_c`](https://github.com/jmguerreroh/socket_c).

> El simulador gráfico, las estructuras comunes, el menú del cliente y los bucles principales de ambos programas ya
> están hechos. El estudiantado implementa las funciones marcadas con `TODO (estudiante)`: comunicación por socket,
> operaciones del servidor, lista doblemente enlazada, validación del protocolo, registro y liberación de memoria.
> La carga opcional de un fichero inicial se explica en el enunciado. No necesita saber nada de hilos ni de mutex.

Antes de empezar, lee estos dos documentos:

1. [docs/ENUNCIADO.md](docs/ENUNCIADO.md): objetivos, funciones que debes implementar, flujo de trabajo y pruebas.
2. [docs/PROTOCOLO.md](docs/PROTOCOLO.md): formato exacto de los mensajes que intercambian `controlador` y
   `aeropuerto`.

El protocolo es textual y usa una línea por mensaje. Por ejemplo:

```text
Cliente → servidor:
ADD|103|Madrid|Paris|A320|75|62.5

Servidor → cliente:
OK|Aeronave 103 añadida correctamente.
STATE|1
AIRCRAFT|103|Madrid|Paris|A320|75|62.50
END_STATE
```

## Cómo empezar (lo más sencillo)

Crea una carpeta nueva para tu solución. Puede estar en cualquier lugar. Por ejemplo:

```bash
mkdir MiAeropuerto
cd MiAeropuerto
```

Dentro de esa carpeta, crea un único fichero llamado `CMakeLists.txt`:

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

Desde esa misma carpeta (`MiAeropuerto/`), ejecuta **una sola vez**:

```bash
cmake -S . -B build          # descarga y prepara las plantillas de la práctica
```

`-S .` indica que el código fuente está en la carpeta actual y `-B build` indica dónde guardar los ficheros
internos de CMake. Este comando descarga el proyecto base y crea en `MiAeropuerto/` las plantillas iniciales:

* `servidor/`: código del servidor y sus módulos.
* `cliente/`: código del cliente y sus módulos.
* `Makefile`: reglas de compilación que debes completar.
* `tests/`: pruebas unitarias y pruebas de sistema.
* `docs/`: documentación de la práctica.
* `ejemplos/`: datos iniciales para probar el servidor.

CMake no implementa la práctica ni compila tus programas. Solo prepara estas carpetas y genera `config.mk`.
No edites `config.mk`.

El primer fichero que debes completar es `MiAeropuerto/Makefile`, creado por el paso anterior. En él debes
rellenar los comentarios `TODO (estudiante)` relacionados con las fuentes, los objetos, las reglas de compilación,
el enlazado y `clean`. Cuando el Makefile esté completo, continúa con los `TODO (estudiante)` de `cliente/` y
`servidor/`. Todos esos ficheros pertenecen a tu solución.

Después, todavía desde `MiAeropuerto/`, compila con:

```bash
make
./aeropuerto 5000            # terminal 1: se abre la ventana
./aeropuerto 5000 ejemplos/aeropuerto_inicial.txt   # ...o cargando aeronaves de un fichero
./controlador 127.0.0.1 5000 # terminal 2, 3...
```

La primera ejecución de `make` solo comprueba que el proyecto puede compilarse y genera los ejecutables.
Es normal que `aeropuerto` todavía no abra una ventana, no escuche conexiones o termine inmediatamente:
las funciones del servidor y del cliente siguen siendo plantillas con `TODO (estudiante)`. El aeropuerto vacío
no es un problema; una vez implementado el servidor, debe arrancar y esperar peticiones aunque no haya aeronaves.
Después de esa primera compilación, implementa las funciones progresivamente y vuelve a ejecutar `make` y las
pruebas correspondientes. Para tener una idea más clara de cómo empezar y abordar la práctica, consulta la
[sección 9, Fases sugeridas](docs/ENUNCIADO.md#9-fases-sugeridas) del enunciado.

Tras el primer `cmake` tu carpeta queda así:

```
MiAeropuerto/
├── CMakeLists.txt
├── Makefile                  ← A COMPLETAR (el simulador ya está hecho; tú: servidor/ y cliente/)
├── config.mk                 ← lo regenera CMake (ruta de lo descargado, SDL2)
├── cliente/
│   ├── include/   comunicacion.h  constantes.h
│   └── src/       cliente.c  comunicacion.c
├── servidor/
│   ├── include/   estructuras.h  simulador.h  comunicacion.h  lista.h
│   └── src/       servidor.c  lista.c  comunicacion.c
├── tests/                    ← comprobaciones y pruebas de sistema
├── docs/                     ← ENUNCIADO.md, PROTOCOLO.md, API.md
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

API proporcionada: [docs/API.md](docs/API.md). Ejemplos de sockets que funcionan: [socket_c](https://github.com/jmguerreroh/socket_c) (`server_peticiones.c` y `client_peticiones.c`). Licencia: [MIT](LICENSE).
