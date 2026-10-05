# Práctica: Aeropuerto con sockets TCP

Servidor y cliente en **C** que se comunican por **TCP**. El servidor guarda las aeronaves de un
aeropuerto en una **lista doblemente enlazada** que programas tú, atiende las **peticiones** del
cliente (añadir, eliminar, modificar, ordenar, mostrar), las muestra en una ventana gráfica en
tiempo real y deja constancia de lo ocurrido en un fichero `.log`.

> Este documento es el enunciado completo. Léelo entero antes de empezar.

---

## 1. Objetivos

* Crear un servidor y un cliente con **sockets TCP**: `socket`, `bind`, `listen`, `accept`,
  `connect`, `read`, `write`, `close`, siguiendo el ejemplo
  <https://github.com/jmguerreroh/socket_c>.
* Entender que TCP es un **flujo de bytes** y enviar/recibir mensajes completos.
* Programar una **lista doblemente enlazada**: crear nodos (`malloc`), enlazarlos, ordenarlos,
  eliminarlos y mantener los punteros.
* Hacer que el servidor **llame a una función distinta por cada opción** de la petición.
* **Liberar** toda la memoria que reservas (`free`), sin fugas.
* Registrar lo que ocurre en un fichero y cerrar el servidor de forma ordenada.

Todo con **C básico**. Para la entrada y la salida se usa solo esto:

| Para... | Se usa |
|---|---|
| mostrar por pantalla | `printf` |
| leer del teclado | `scanf` |
| escribir en un fichero (el registro) | `fopen`, `fprintf`, `fflush`, `fclose` |
| leer un fichero (las aeronaves iniciales) | `fopen`, `fscanf`, `fclose` |
| leer datos de un texto que ya tienes (una petición) y construir texto | `sscanf`, `snprintf` |
| hablar por el socket | `read` y `write` |

No hace falta nada más.

---

## 2. Cómo funciona

**El controlador (cliente) se conecta una vez por cada petición**: abre una conexión, envía **una**
petición, lee la respuesta y cierra. El servidor atiende las conexiones **de una en una**.

```text
 controlador                                   aeropuerto (servidor)
 ───────────                                   ─────────────────────
 socket() + connect() ────────────────────────▶ accept()
 write("ANADIR 103 Madrid ...\n") ────────────▶ lee la línea ─▶ llama a la función de la opción
                                                modifica la lista ─▶ actualiza el simulador
 read() hasta que cierre ◀── resultado + estado ─ write() ... y close()
 close()
   ... (la siguiente opción del menú es otra conexión) ...
```

```text
┌─────────────────┐   TCP, texto, 1 línea    ┌──────────────────────────────────┐
│   CONTROLADOR   │ ───── petición ────────▶ │          AEROPUERTO              │
│ (solo terminal) │ ◀──── resultado+estado ─ │  lista doblemente enlazada (TÚ)  │
└─────────────────┘                          │  ventana gráfica (ya hecha)      │
                                             │  aeropuerto.log (TÚ)             │
                                             └──────────────────────────────────┘
```

No hay hilos, ni mutex, ni lista de clientes que gestionar.

---

## 3. Cómo empezar

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
cmake -S . -B build          # descarga todo lo necesario y CREA TUS CARPETAS
```

**CMake solo descarga y prepara; no compila tu código.** Descarga el simulador gráfico y **SDL2** si
no lo tienes instalado (lo compila él solo, la primera vez tarda un minuto) y **crea en tu carpeta**
las plantillas a completar. Después **completas el `Makefile`** y compilas con `make`:

```
MiAeropuerto/
├── CMakeLists.txt
├── Makefile                     ← A COMPLETAR POR TI (el simulador ya está hecho)
├── config.mk                    ← lo regenera CMake: dónde está lo descargado y cómo usar SDL2
├── cliente/
│   ├── include/   comunicacion.h  constantes.h
│   └── src/       cliente.c  comunicacion.c
├── servidor/
│   ├── include/   estructuras.h  simulador.h  comunicacion.h  lista.h
│   └── src/       servidor.c  lista.c  comunicacion.c
├── tests/                       ← comprobaciones y pruebas de sistema
├── docs/                        ← ENUNCIADO.md (este documento), PROTOCOLO.md (los mensajes), API.md
└── ejemplos/                    ← aeropuerto_inicial.txt (fichero de ejemplo) y rellenar.sh
```

Esos ficheros son **tuyos**: CMake nunca los vuelve a tocar (solo regenera `config.mk`, que no debes
editar). Tu código va **entre las líneas** `/*====== INICIO DE TU CODIGO ======*/` y
`/*====== FIN DE TU CODIGO ======*/` (en el `Makefile`, `#====== INICIO ... ======`): fuera de ellas
no tienes que tocar nada. Dentro hay un `TODO (estudiante)` y `PISTA:` con ideas; puedes borrar esas
líneas y la línea de ejemplo que deja el código compilando, y escribir lo tuyo.

Requisitos: compilador de C11, CMake ≥ 3.16, `git`, `make`, `pkg-config`. Recomendable: `libsdl2-dev`
(si no está, CMake descarga SDL2), `netcat` (`nc`) y `valgrind`.

```bash
sudo apt install build-essential cmake git pkg-config libsdl2-dev netcat-openbsd valgrind
```

### El Makefile (lo completas tú)

El `Makefile` trae hecho todo menos **la parte tuya, que está al final del fichero** (busca `TODO
(estudiante)`); lo de arriba (simulador gráfico, `make tests`, `make prueba`...) no hace falta tocarlo:

1. variables con los ficheros de tu servidor y de tu cliente (`SRV_FUENTES`, `SRV_OBJ`,
   `CLI_FUENTES`, `CLI_OBJ`);
2. reglas para compilar cada `.c` de `servidor/src` y de `cliente/src` a un objeto (cada lado con su
   propio `-I...`);
3. el enlazado de `./aeropuerto` y de `./controlador`;
4. el objetivo `clean`.

Mientras no lo completes, `make` te avisa de lo que falta (`Falta completar el Makefile: no sé cómo
construir 'aeropuerto'`).

### Compilar y ejecutar

```bash
make                          # genera ./aeropuerto y ./controlador
make tests                    # comprueba tus funciones de envío y recepción
make prueba                   # prueba de sistema con tu servidor real
make memoria                  # tu servidor real bajo valgrind: no debe haber fugas
make SIN_SDL=1                # sin ventana gráfica
```

Terminal 1 (se abre la ventana gráfica):

```bash
./aeropuerto 5000
./aeropuerto 5000 ejemplos/aeropuerto_inicial.txt   # carga las aeronaves de ese fichero al arrancar
```

Si no hay entorno gráfico (por ejemplo, por SSH), el servidor avisa y sigue sin ventana. Para no abrirla
a propósito: `SDL_VIDEODRIVER=dummy ./aeropuerto 5000`.

Terminal 2 (y las que quieras):

```bash
./controlador 127.0.0.1 5000
sh ejemplos/rellenar.sh 5000 10        # añade 10 aeronaves de ejemplo (para ver el simulador)
tail -f aeropuerto.log                 # para ver lo que va pasando en el servidor
```

### Los argumentos ★

**Los argumentos de los dos programas los lees y validas tú** (con `argc` y `argv`).

| Programa | Argumentos | Reglas |
|---|---|---|
| `./aeropuerto` | `PUERTO [FICHERO]` | el puerto es obligatorio y debe estar entre 1 y 65535; el fichero es opcional y, si se da, debe poder abrirse; ni más argumentos |
| `./controlador` | `IP PUERTO` | exactamente dos argumentos; la IP debe ser una dirección IPv4 válida (`inet_pton`) y el puerto estar entre 1 y 65535 |

Si faltan argumentos, son incorrectos (puerto no numérico o fuera de rango, IP inválida, opción
de más) o el fichero no se puede abrir, el programa muestra por la **salida
de error** (`stderr`) un mensaje y termina con un **código distinto de 0** (`EXIT_FAILURE`), sin
arrancar. Para los argumentos mal escritos, el mensaje de uso es:

```text
Uso:
    ./aeropuerto PUERTO [FICHERO]
    ./controlador IP PUERTO
```

(puedes añadir antes una línea que diga qué argumento falla: `Puerto inválido: abc`). `make prueba`
lo comprueba.

---

## 4. Qué te damos y qué haces tú

**Te damos hecho** (no lo modificas):

* Las **estructuras** (`Aeronave`, `Nodo`, `ListaAeropuerto`) y las constantes
  (`servidor/include/estructuras.h`).
* El **simulador gráfico**: tres funciones (`iniciar_simulador`, `actualizar_simulador`,
  `detener_simulador`). Solo tienes que llamar a `actualizar_simulador(&lista)` después de cada
  petición ([docs/API.md](API.md)).
* En el `main` del servidor: que **Ctrl+C** pida cerrar (el manejador de la señal) y cerrar la ventana
  del simulador al terminar (`detener_simulador`). **Todo lo demás del `main` lo escribes tú**,
  empezando por los argumentos.
* Los **prototipos de las funciones de envío y recepción por el socket**: lo único con cabecera
  (`comunicacion.h`).

**Lo programas tú** (los nombres y parámetros de tus funciones los eliges tú):

| Dónde | Qué |
|---|---|
| `servidor/src/comunicacion.c` | `enviar_texto()` y `recibir_linea()` |
| `cliente/src/comunicacion.c` | `enviar_texto()` y `recibir_hasta_cierre()` |
| `servidor/src/lista.c` | tu lista doblemente enlazada (sección 6) |
| `servidor/src/servidor.c` | **leer y validar los argumentos**, arrancar el simulador, el socket del servidor, el bucle, **una función por cada opción**, la respuesta, **la carga del fichero inicial**, el registro y el cierre |
| `cliente/src/cliente.c` | **leer y validar los argumentos, el menú, la lectura de los datos del teclado y las opciones**; construir cada petición, conectar, enviar, recibir y mostrar |
| `Makefile` | la parte de `servidor/` y `cliente/` |

---

## 5. Los mensajes

Todo el detalle está en **[docs/PROTOCOLO.md](PROTOCOLO.md)**. Resumen:

**Petición** (una línea terminada en `\n`, palabras separadas por espacios, textos sin espacios):

```text
ANADIR 103 Madrid Paris A320 75 62.5
ELIMINAR 103
MODIFICAR 103 DESTINO Roma
ORDENAR
MOSTRAR
SALIR
```

**Respuesta** (el servidor la envía y **cierra la conexión**; el cliente la lee hasta que cierre y la
muestra tal cual):

```text
OK: Aeronave 103 añadida correctamente.         (o  ERROR: ya existe una aeronave con ID 103.)
--- ESTADO ACTUAL: 3 aeronaves ---
ID 101 | Madrid -> London | A320 | 80% | 75.4
ID 102 | Paris -> Rome | B737 | 65% | 54.2
ID 103 | Madrid -> Paris | A320 | 75% | 62.5
```

Para leer los datos de una petición te basta `sscanf()`; no tienes que usar ninguna función de
troceo. Para construirla, `snprintf()`.

---

## 6. El servidor

### 6.1 Pasos (como `server.c` de `socket_c`)

0. **Leer y validar los argumentos**, **preparar la lista** (vacía), abrir el registro y **arrancar el simulador**. Si se ha indicado un fichero, **cargarlo**
   (ver 6.2).
1. **Crear el socket de escucha:** `socket()`, `setsockopt(SO_REUSEADDR)`, `bind()` y `listen()`.
   Comprueba el resultado de cada llamada.
2. **Repetir hasta que haya que cerrar:**
   1. `accept()` → socket del cliente **y su IP y puerto** (para el registro);
   2. leer la petición con tu `recibir_linea()`;
   3. **llamar a la función de la opción** que pide (ver 6.3);
   4. enviar la respuesta con tu `enviar_texto()` (ver 6.4);
   5. `actualizar_simulador(&lista)`;
   6. `close()` del socket del cliente.
3. **Cerrar ordenadamente** (ver 6.8).

### 6.2 Fichero inicial ★

Si al arrancar se indica un **fichero de texto** como argumento:

```bash
./aeropuerto 5000 ejemplos/aeropuerto_inicial.txt
```

el servidor tiene que **cargar esas aeronaves en la lista** antes de empezar a atender peticiones
(y refrescar el simulador, que las mostrará desde el primer momento). Al leer los argumentos
debes comprobar que el fichero se puede abrir (si no, el programa termina con un error); **leerlo (con `fscanf`) y
cargarlo también lo programas tú**.

**Formato:** una aeronave por línea, con los mismos datos y reglas que `ANADIR` pero **sin** la
palabra `ANADIR`:

```text
# Aeronaves iniciales: id origen destino modelo capacidad combustible
105 Sevilla Roma A350 90 88.0
101 Madrid Paris A320 80 75.4

103 Barcelona Londres B737 65 54.2
```

* Las líneas **vacías** y las que empiezan por **`#`** se ignoran.
* Cada aeronave pasa por las **mismas validaciones** que al añadirla por la red (ID positivo y no
  repetido, capacidad de 0 a 100, combustible ≥ 0, textos que caben...). Un consejo: ya tienes una
  función para añadir; reutilízala.
* Una **línea incorrecta no detiene la carga**: se salta y se apunta en el registro (por ejemplo
  `Línea 4 del fichero ignorada: ya existe una aeronave con ID 101.`).
* Al terminar, se muestra y se apunta en el registro cuántas se han cargado:
  `Cargadas 3 aeronaves desde ejemplos/aeropuerto_inicial.txt`.
* El orden en la lista es el del fichero (si no está ordenado por ID, se verá `SIN ORDENAR` hasta
  que un cliente envíe `ORDENAR`).

### 6.3 Una función por cada opción ★

Al recibir una petición, el servidor mira su **primera palabra** y llama a **una función distinta
por cada opción**: una para añadir, otra para eliminar, otra para modificar, otra para ordenar,
otra para mostrar y otra para salir. **Las diseñas tú** (nombres, parámetros y qué devuelven).
Cada una trabaja con la lista y deja el texto del resultado (por ejemplo,
`Aeronave 103 añadida correctamente.` o `ya existe una aeronave con ID 103.`).

Ejemplo del esquema de la función que reparte (no es obligatorio):

```text
leer la primera palabra con sscanf(linea, "%15s", orden)
si orden es "ANADIR"    -> tu función de añadir
si orden es "ELIMINAR"  -> tu función de eliminar
...
en otro caso            -> "operación desconocida."
```

### 6.4 La respuesta

Para **toda** petición, válida o no, el servidor envía el resultado **y el estado actual**, con el
formato exacto de [docs/PROTOCOLO.md](PROTOCOLO.md): la línea `OK: ...`/`ERROR: ...`, la línea
`--- ESTADO ACTUAL: n aeronaves ---` y una línea por aeronave en el orden de la lista. Puedes enviar
línea a línea con `enviar_texto()` (sin necesidad de un buffer grande). Al terminar, el servidor
cierra la conexión.

### Dónde se usan las funciones de comunicación

Las funciones de `comunicacion.c` las llamas tú, **dentro de tu código**, en estos puntos:

| Programa | Dónde | Función | Para qué |
|---|---|---|---|
| Servidor | en el bucle, justo después de `accept()` | `recibir_linea()` | leer la petición del cliente |
| Servidor | en tu función que envía la respuesta (tres tipos de línea) | `enviar_texto()` | enviar `OK:`/`ERROR:`, la cabecera del estado y una línea por aeronave |
| Cliente | en tu función de hacer una petición, tras `connect()` | `enviar_texto()` | enviar la petición |
| Cliente | a continuación | `recibir_hasta_cierre()` | leer la respuesta completa |

Orden de una petición completa, vista desde los dos lados:

```text
 cliente                                  servidor
 1. socket()                              (esperando en accept())
 2. connect() ─────────────────────────▶  1. accept()
 3. enviar_texto(petición) ─────────────▶  2. recibir_linea()
                                           3. llama a la función de la opción (trabaja con la lista)
 5. recibir_hasta_cierre() ◀───────────  4. enviar_texto(resultado + estado)   (varias llamadas)
                                           5. actualizar_simulador(&lista)
 6. close()                               6. close()   ← el cliente deja de leer cuando el servidor cierra
```

### 6.5 Validación (el servidor es la autoridad)

El cliente puede validar, pero **el servidor valida siempre**:

| Dato | Regla | Mensaje |
|---|---|---|
| ID | entero positivo | `ID inválido.` |
| ID | único al añadir | `ya existe una aeronave con ID n.` |
| ID | existente al eliminar/modificar | `no existe ninguna aeronave con ID n.` |
| Capacidad | de 0 a 100 | `capacidad fuera de rango.` |
| Combustible | número ≥ 0 | `combustible inválido.` |
| Petición | nº de palabras y números correctos | `formato de petición incorrecto.` |
| Orden | una de las 6 | `operación desconocida.` |
| Textos | caben en su campo (nunca desbordar el array) | — |

Los textos exactos de todos los mensajes están en [docs/PROTOCOLO.md](PROTOCOLO.md).

### 6.6 La lista doblemente enlazada ★

Las estructuras ya están en `estructuras.h` (**no las modifiques**):

```text
           primero                                   ultimo
              │                                         │
              ▼                                         ▼
          ┌──────┐  siguiente  ┌──────┐  siguiente  ┌──────┐
          │ 105  │ ──────────▶ │ 101  │ ──────────▶ │ 103  │ ─▶ NULL
  NULL ◀─ │      │ ◀────────── │      │ ◀────────── │      │
          └──────┘   anterior  └──────┘  anterior   └──────┘
```

En `lista.c` escribes las funciones que necesites (y sus prototipos en `lista.h`). Tu lista debe
permitir:

* **añadir** una aeronave **al final** (orden de llegada), sin repetir IDs; si `malloc` falla,
  comprobarlo. Cuidado con la **lista vacía**;
* **buscar** una aeronave por su ID;
* **eliminar** una aeronave por su ID: actualizar los punteros de los vecinos (y `primero` o
  `ultimo` si era el primero, el último o el único), `free` y `cantidad--`. Si el ID no existe, **no
  se libera nada**;
* **ordenar por ID ascendente** **reenlazando** los nodos (sin `malloc` ni copiar datos), y saber si
  **ya estaba ordenada** (en ese caso la respuesta lo indica y no se toca la lista);
* **vaciar** la lista liberando **todos** los nodos (guarda `siguiente` **antes** del `free`).

Mantén `cantidad` al día. La ventana gráfica **recorre tu lista**: si tus punteros están mal, lo
verás.

### 6.7 El simulador gráfico

Al arrancar, **tu `main`** llama a `iniciar_simulador()` (si no hay entorno gráfico, avisa y sigue
sin ventana). Después llamas a
`actualizar_simulador(&lista)` **después de cada petición**; la ventana muestra una casilla por
aeronave, **en el orden de tu lista**, y al ordenar se recolocan. Con la ventana cerrada, o sin entorno
gráfico, no hace nada y el servidor sigue igual.

### 6.8 Cierre del servidor y registro ★

El servidor termina por **dos** motivos:

1. **Ctrl+C**. Ya está preparado: el programa pone `g_parar = 1`.
2. **Una petición `SALIR`**: tu función de salir pone `g_parar = 1` (y la respuesta es
   `OK: Cierre aceptado. El aeropuerto se cierra.`); el bucle termina al acabar esa petición.

Al cerrar, y por este orden: cierra el socket de escucha, **libera todos los nodos de la lista**,
cierra el registro e imprime por pantalla:

```text
Memoria liberada correctamente.
Servidor finalizado.
```

**Registro `aeropuerto.log`:** en el directorio donde se ejecuta el servidor, abierto en modo
**añadir** (`fopen(..., "a")`, no se borra lo anterior). Una línea por evento, escrita con `fprintf` y con `fflush` para que
el fichero esté al día aunque el servidor muera de golpe. Eventos **obligatorios** (el texto exacto
puede variar, pero estas palabras deben aparecer):

| Evento | Ejemplo de línea |
|---|---|
| Arranque | `Servidor iniciado en el puerto 5000` |
| Fichero inicial (si se indica) | `Cargadas 3 aeronaves desde ejemplos/aeropuerto_inicial.txt` y, por cada línea incorrecta, `Línea 4 del fichero ignorada: ...` |
| Cada petición, **con la IP y el puerto del cliente** | `[127.0.0.1:50422] Petición: ANADIR 103 Madrid Paris A320 75 62.5` |
| Cada respuesta, con la IP y el puerto del cliente | `[127.0.0.1:50422] Respuesta: OK: Aeronave 103 añadida correctamente.` |
| Motivo del cierre | `Cierre del servidor: un cliente ha pedido cerrar (SALIR)` |
| Fin | `Servidor finalizado` |

La IP y el puerto del cliente los obtienes del propio `accept()`: pásale una `struct sockaddr_in` y
el tamaño, y convierte la dirección con `inet_ntop()` y el puerto con `ntohs()`. Cada línea que se
deba a un cliente (petición, respuesta, conexión descartada) empieza por `[IP:puerto]`.

*(Opcional: empieza cada línea con la fecha y la hora, con `time()`, `localtime()` y `strftime()`.)*

### 6.9 Gestión de memoria ★

**La memoria que reservas es tuya y tienes que liberarla.** En el servidor es, sobre todo, la de los
**nodos de la lista** (`malloc` al añadir, `free` al eliminar y al cerrar) y el fichero del registro
(`fopen`/`fclose`). Lo que **no** es tuyo: lo del simulador gráfico (lo libera `detener_simulador`).
`make memoria` ejecuta el servidor real bajo valgrind con una sesión completa y **falla si queda
memoria sin liberar**.

---

## 7. El cliente (`controlador`)

`./controlador IP PUERTO` valida la IP y el puerto (ver «Los argumentos»), **pide el estado inicial** con una
petición `MOSTRAR` y entra en un bucle con este menú. **El menú, la lectura de los datos del teclado
y las opciones los programas tú** (mostrando con `printf` y leyendo con `scanf`):

```text
================================
       CONTROLADOR ATC
================================

1. Añadir aeronave
2. Eliminar aeronave
3. Modificar aeronave
4. Ordenar aeropuerto
5. Mostrar aeropuerto
6. Salir y CERRAR el aeropuerto
7. Salir sin cerrar el aeropuerto
```

Para cada opción: **pedir los datos al usuario** (volviendo a preguntar si un número está mal escrito,
y aceptando como texto una sola palabra que quepa en su campo) → **construir la petición** con `snprintf` (formato en
[docs/PROTOCOLO.md](PROTOCOLO.md)) → **hacer la petición** (tu función de cliente):
`socket()`, `connect()`, `enviar_texto()`, `recibir_hasta_cierre()`, mostrar la respuesta por
pantalla y `close()` → volver al menú.

* `3. Modificar` pide el ID y qué dato cambiar (uno por petición).
* `6` envía `SALIR`: el aeropuerto **se cierra**. `7` sale solo del controlador (también `Ctrl+D`, que
  termina el programa sin cerrar el servidor).
* Si el servidor no está en marcha, el cliente lo dice y no se cuelga.

---

## 8. Casos de error que debes manejar

| Situación | Qué debe ocurrir |
|---|---|
| ID repetido al añadir | `ERROR: ya existe una aeronave con ID n.` + estado |
| Eliminar/modificar un ID inexistente | `ERROR: no existe ninguna aeronave con ID n.`; no se libera nada |
| Capacidad o combustible no válidos | error específico; la lista no cambia |
| Orden desconocida / palabras de más o de menos | `operación desconocida.` / `formato de petición incorrecto.` |
| Texto más largo que el campo | no desbordar el array; devolver error |
| Línea incorrecta en el fichero inicial | se salta, se apunta en el registro y se sigue cargando |
| Fichero inicial que no se puede abrir | avisar y terminar con `EXIT_FAILURE` (sin arrancar) |
| Argumentos incorrectos (puerto, IP, de más o de menos) | mensaje de uso por `stderr` y `EXIT_FAILURE` |
| Mensaje que llega en trozos | se lee hasta el `\n` |
| Cliente que se conecta y no envía nada | `recibir_linea` devuelve -1 a los 3 s; se registra (con su IP y puerto) y se sigue |
| Cliente que se va a mitad de la respuesta | el servidor no se cae (SIGPIPE ya ignorado) |
| `SALIR` | respuesta `OK:`, el servidor se cierra y lo registra |
| Servidor no disponible (en el cliente) | avisar y no colgarse |
| `socket`, `bind`, `accept`, `read`, `write`, `malloc` que fallan | comprobar siempre el retorno |
| `aeropuerto.log` no se puede abrir | avisar y seguir sin registro |

Reglas de código: nunca `gets()`; nunca `strcpy` sin comprobar tamaños (usa `snprintf` o `%63s` en
`sscanf`); constantes de `estructuras.h`, sin números mágicos; sin avisos con `-Wall -Wextra
-Wpedantic`.

---

## 9. Fases sugeridas

| Fase | Contenido | Cómo comprobarla |
|---|---|---|
| 0 | El `Makefile`: compilar y enlazar `servidor/` y `cliente/` | `make` genera `./aeropuerto` y `./controlador` |
| 1 | Envío y recepción por el socket: los dos `comunicacion.c` | `make tests` |
| 2 | Servidor: **argumentos**, socket de escucha, `accept`, recibir una línea y **responder** algo fijo | `./aeropuerto 5000` y `nc 127.0.0.1 5000` |
| 3 | Cliente: **argumentos**, conectar, enviar `MOSTRAR` y mostrar lo recibido | `./controlador 127.0.0.1 5000` |
| 4 | La lista (`lista.c`) | recorrerla con `printf` desde tu `main` |
| 5 | **Una función por opción**: añadir y mostrar, con la respuesta completa | `nc` y la ventana |
| 6 | Eliminar, modificar y ordenar | `nc`, el controlador y la ventana |
| 7 | **Fichero inicial**: cargarlo al arrancar | `./aeropuerto 5000 ejemplos/aeropuerto_inicial.txt` y `make prueba` |
| 8 | **`SALIR` cierra el servidor** y liberación de memoria | `make prueba` |
| 9 | **Registro `aeropuerto.log`** | `make prueba` |
| 10 | **Memoria**: sin fugas | `make memoria` |

---

## 10. Pruebas: cómo comprobar tu trabajo

No tienes que escribir ninguna prueba: **ya están escritas** (carpeta `tests/`) y solo las
**ejecutas**. Funcionan como un corrector automático que te dice qué falla. **No las modifiques.**

### `make tests`: tus funciones de envío y recepción

Comprueban `enviar_texto()`, `recibir_linea()` y `recibir_hasta_cierre()` **sin necesidad de que el
resto del programa esté hecho**. Al principio **fallan: es lo normal**. Cada fallo indica qué no se
cumple:

```text
  FALLO tests/test_comunicacion_servidor.c:21: (enviar_texto(sv[0], "MOSTRAR\n")) == (0)
  FALLO tests/test_comunicacion_servidor.c:22: (recibir_linea(sv[1], linea, sizeof linea)) == (7)
  FALLO tests/test_comunicacion_servidor.c:23: strcmp(linea, "MOSTRAR") == 0
[FAIL] test_una_linea
test_comunicacion_servidor: 16 comprobaciones, 11 fallos
```

Aquí se esperaba que `enviar_texto` devolviera 0 y que `recibir_linea` devolviera 7 (la longitud
de `MOSTRAR`).

* Las líneas `FALLO fichero:línea: expresión` salen primero: cada una es una comprobación concreta
  que **no se cumple**; abre ese fichero en esa línea para ver qué esperaba. Justo debajo, `[ OK ]` /
  `[FAIL]` es el resultado de ese grupo.
* La última línea resume cuántas comprobaciones hay y cuántas fallan. Has terminado esa parte cuando
  pone `0 fallos`; al final aparece `== TODAS LAS COMPROBACIONES OK ==`.
* Si una comprobación se queda esperando datos que tu función no envía, se aborta a los 10 segundos
  con `TIEMPO AGOTADO` en lugar de colgarse.
* Los ejecutables quedan en `build/make/tests/`; para pasar solo uno:
  `./build/make/tests/test_comunicacion_servidor`.

### Pruebas con el programa real

Arrancan **tu servidor de verdad** y le hablan con `nc`, como un cliente. Son scripts (`tests/*.sh`).

* `make prueba`: manda peticiones **una por conexión** y comprueba que la respuesta tiene el
  formato exacto, que hay estado también en los errores, que el estado se comparte entre conexiones,
  que `ORDENAR` ordena, que el servidor **no** se cierra hasta el `SALIR`, que carga bien un
  fichero inicial (ignorando las líneas incorrectas), que **los argumentos de los dos programas se
  validan** y que `aeropuerto.log` contiene lo ocurrido. Muestra `[ OK ]` o `[FAIL]` con la causa en cada paso.
* `make memoria`: tu servidor bajo `valgrind` con una sesión completa; exige 0 errores y 0 bytes
  perdidos. Si falla, muestra el informe: busca `definitely lost` y la línea de `malloc`.

Orden recomendado antes de entregar: `make tests` → `make prueba` → `make memoria`.

---

## 11. Entrega y evaluación

**Entrega**

* El `Makefile` y los ficheros de `cliente/` y `servidor/` (con todos los `TODO (estudiante)`
  resueltos). No modifiques `tests/`, `config.mk`, `estructuras.h`, `simulador.h` ni la parte
  «DADA» del `Makefile`.
* Debe compilar con `make` **sin avisos** y pasar `make tests`, `make prueba` y `make memoria`.
* Incluye un `aeropuerto.log` de una sesión tuya (varias peticiones, errores, y al final un `SALIR`).

**Evaluación (propuesta)**

| % | Criterio |
|---|---|
| 20 | Comunicación TCP: `socket/bind/listen/accept/connect` bien usados y comprobados, `close`, envío y lectura completos |
| 15 | Mensajes y argumentos: formato exacto de peticiones y respuestas, estado en **toda** respuesta, argumentos validados |
| 20 | Lista y operaciones: nodos, punteros, orden, **una función por opción**, fichero inicial, uso del simulador |
| 10 | Gestión de errores: mensajes exactos, validación en el servidor, desconexiones |
| 15 | Memoria: sin fugas (`make memoria`), nada de `strcpy`/`gets` sin controlar |
| 10 | Cierre con `SALIR` y fichero `.log` correctos |
| 10 | Calidad: `Makefile` correcto, sin avisos, constantes, comentarios |

---

## 12. Relación con el repositorio de referencia (`socket_c`)

| `socket_c` | Esta práctica |
|---|---|
| `server.c`: `socket()`, `bind()`, `listen()` | el principio de `main` en `servidor.c` |
| `server.c`: `accept()` | el bucle de `main` en `servidor.c` |
| `server.c`: `read()` / `write()` con el cliente | `recibir_linea()` / `enviar_texto()` |
| `client.c`: `socket()`, `connect()` | tu función de cliente en `cliente.c` |
| `client.c`: `write()` / `read()` | `enviar_texto()` / `recibir_hasta_cierre()` |
| `read()` de 80 bytes suponiendo que es un mensaje | leer hasta el `\n` (servidor) o hasta que cierre (cliente) |
| `"exit"` para terminar | `SALIR` (el servidor se cierra) |
| `close()` | tras cada petición (cliente y servidor) |
| `fgets()` seguro | `scanf` con tamaño máximo (`"%63s"`) en toda la entrada del cliente |
| Mensaje libre | formato fijo de petición y de respuesta |
| Nada | lista doblemente enlazada, una función por opción y `aeropuerto.log` |

---

## 13. Errores habituales

* Suponer que un `read()` trae el mensaje entero, o ignorar lo que devuelve `write()`.
* No poner `'\0'` tras recibir, o usar `strcpy`/`%s` en `sscanf` sin límite de tamaño
  (usa `%63s`, `%31s`...).
* Olvidar `htons()` al rellenar el puerto, o `SO_REUSEADDR` (`Address already in use`).
* Responder solo al éxito y no a las peticiones erróneas: el cliente se queda esperando.
* Olvidar **cerrar el socket del cliente** en el servidor: el controlador no termina de leer, porque
  espera a que el servidor cierre.
* **Lista:** `free(nodo)` y luego leer `nodo->siguiente`; no actualizar `primero`/`ultimo` al
  insertar o eliminar el primero, el último o el único; olvidar `cantidad`; liberar un nodo
  inexistente.
* No llamar a `actualizar_simulador(&lista)` después de cambiar la lista (la ventana se queda
  desactualizada).
* No liberar la lista al cerrar, o no cerrar el fichero del registro.
* Abrir el log en modo `"w"` (borra lo anterior) o no hacer `fflush`.
* **`scanf`:** no descartar el resto de la línea después de leer (lo que sobra se cuela en la siguiente
  pregunta); no mirar lo que devuelve (`1` = leído, `0` = texto no válido, `EOF` = se acabó la entrada).
  Con `scanf("%d", ...)` y letras, el texto **no se consume**: si no lo descartas
  (`scanf("%*[^\n]")`), el bucle vuelve a leer lo mismo para siempre.
* Leer una palabra con `scanf("%s", ...)` sin límite de tamaño (usa `"%63s"` o lee en un array
  mayor y comprueba la longitud).
