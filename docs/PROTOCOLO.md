# Los mensajes entre el controlador y el aeropuerto

Es un protocolo **de texto**, lo más simple posible. Puedes comprobarlo a mano con `nc`
(netcat) sin necesidad del controlador.

## Reglas

* **Una conexión por petición.** El controlador se conecta, envía **una** petición, lee la
  respuesta completa y cierra. El servidor atiende, responde y **cierra** la conexión.
* La **petición** es **una línea** de texto terminada en `\n`, con las palabras separadas por
  **espacios**.
* Los textos (origen, destino, modelo) son **una sola palabra, sin espacios**. Para un nombre
  compuesto se usa `_`: `Nueva_York`.
* La **respuesta** es texto en varias líneas. El cliente la lee **hasta que el servidor cierra
  la conexión** (no hace falta ninguna marca de final) y la muestra tal cual.
* Los números decimales usan **punto**: `62.5`.
* Tamaños máximos (con el `\0`): origen 64, destino 64, modelo 32, petición 256 bytes.

## Peticiones (cliente → servidor)

| Petición | Significado |
|---|---|
| `ANADIR id origen destino modelo capacidad combustible` | añadir una aeronave |
| `ELIMINAR id` | eliminar la aeronave con ese ID |
| `MODIFICAR id campo valor` | cambiar un dato (`campo`: `ORIGEN`, `DESTINO`, `MODELO`, `CAPACIDAD` o `COMBUSTIBLE`) |
| `ORDENAR` | ordenar el aeropuerto por ID ascendente |
| `MOSTRAR` | pedir el estado actual |
| `SALIR` | pedir que el servidor se cierre |

Ejemplos (cada uno termina en `\n`):

```text
ANADIR 103 Madrid Paris A320 75 62.5
ELIMINAR 103
MODIFICAR 103 DESTINO Roma
MODIFICAR 103 COMBUSTIBLE 50.5
ORDENAR
MOSTRAR
SALIR
```

Las órdenes se escriben en **mayúsculas** y sin tilde (`ANADIR`). Para cambiar varios datos de
una aeronave se envían varias peticiones `MODIFICAR`.

## Respuesta (servidor → cliente)

**Toda** petición recibe una respuesta, sea válida o no, con este formato exacto:

```text
OK: <mensaje>                                      (o  ERROR: <mensaje>)
--- ESTADO ACTUAL: <n> aeronaves ---
ID <id> | <origen> -> <destino> | <modelo> | <capacidad>% | <combustible>
ID <id> | ...                                       (una línea por aeronave, en el orden de la lista)
```

El combustible se escribe con **un decimal** (`%.1f`) y la capacidad con el signo `%` pegado.

Ejemplo de respuesta a `ANADIR 103 Madrid Paris A320 75 62.5` cuando ya había dos aeronaves:

```text
OK: Aeronave 103 añadida correctamente.
--- ESTADO ACTUAL: 3 aeronaves ---
ID 101 | Madrid -> London | A320 | 80% | 75.4
ID 102 | Paris -> Rome | B737 | 65% | 54.2
ID 103 | Madrid -> Paris | A320 | 75% | 62.5
```

Y si falla (el estado se envía **igualmente**):

```text
ERROR: ya existe una aeronave con ID 103.
--- ESTADO ACTUAL: 3 aeronaves ---
ID 101 | Madrid -> London | A320 | 80% | 75.4
...
```

## Mensajes del servidor

La primera línea de la respuesta es `OK: ` o `ERROR: ` seguido de uno de estos textos:

| Situación | Texto |
|---|---|
| `ANADIR` correcto | `Aeronave 103 añadida correctamente.` |
| `ELIMINAR` correcto | `Aeronave 103 eliminada correctamente.` |
| `MODIFICAR` correcto | `Aeronave 103 modificada correctamente.` |
| `ORDENAR` ha ordenado | `Aeropuerto ordenado por ID.` |
| `ORDENAR` ya estaba ordenado (**es un `OK:`**) | `El aeropuerto ya estaba ordenado por ID. No ha sido necesario modificar la lista.` |
| `MOSTRAR` | `Estado actual del aeropuerto.` |
| `SALIR` | `Cierre aceptado. El aeropuerto se cierra.` |
| ID repetido | `ya existe una aeronave con ID 103.` |
| ID que no existe | `no existe ninguna aeronave con ID 103.` |
| ID ≤ 0 | `ID inválido.` |
| capacidad fuera de 0..100 | `capacidad fuera de rango.` |
| combustible negativo | `combustible inválido.` |
| campo de `MODIFICAR` desconocido | `campo desconocido.` |
| faltan o sobran palabras / número mal escrito | `formato de petición incorrecto.` |
| primera palabra desconocida | `operación desconocida.` |

El servidor **valida siempre** (el ID, la capacidad y el combustible), aunque el cliente ya lo
haya comprobado.

## Fichero inicial (no es un mensaje de red)

Si se arranca el servidor con un fichero (`./aeropuerto 5000 aeronaves.txt`), carga esas aeronaves antes
de atender peticiones. Es texto: **una aeronave por línea**, con los mismos datos que `ANADIR` pero sin la
palabra `ANADIR`:

```text
# comentario (se ignora, igual que las líneas vacías)
id origen destino modelo capacidad combustible
105 Sevilla Roma A350 90 88.0
```

Cada línea pasa por las mismas validaciones que `ANADIR`; una línea incorrecta se salta y se apunta en el
registro. Ver `ejemplos/aeropuerto_inicial.txt`.

## Probarlo a mano

Cada `nc` es una conexión, es decir, una petición:

```text
$ printf 'ANADIR 103 Madrid Paris A320 75 62.5\n' | nc 127.0.0.1 5000
OK: Aeronave 103 añadida correctamente.
--- ESTADO ACTUAL: 1 aeronaves ---
ID 103 | Madrid -> Paris | A320 | 75% | 62.5

$ printf 'ORDENAR\n' | nc 127.0.0.1 5000
OK: El aeropuerto ya estaba ordenado por ID. No ha sido necesario modificar la lista.
--- ESTADO ACTUAL: 1 aeronaves ---
ID 103 | Madrid -> Paris | A320 | 75% | 62.5
```

Para añadir varias aeronaves de ejemplo de una vez: `sh ejemplos/rellenar.sh 5000 10`.

## Por qué TCP necesita cuidado

TCP es un **flujo de bytes**, no de mensajes: un `write()` puede enviar menos de lo que pides y un
`read()` puede devolverte solo una parte. Por eso `enviar_texto()` repite `write()` hasta enviarlo
todo, `recibir_linea()` sigue leyendo hasta encontrar el `\n` y `recibir_hasta_cierre()` sigue
leyendo hasta que el servidor cierra.
