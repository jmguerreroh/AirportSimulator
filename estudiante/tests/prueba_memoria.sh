#!/bin/sh
# prueba_memoria.sh - Ejecuta el servidor REAL bajo valgrind y comprueba que NO deja fugas.
#
#   tests/prueba_memoria.sh [RUTA_AEROPUERTO]        (por defecto ./aeropuerto)
#
# Se arranca con un fichero inicial y se hace una sesión completa (varios ANADIR, MODIFICAR, ORDENAR, MOSTRAR, ELIMINAR, errores, SALIR),
# una conexión por petición, y se exige:
#   - el servidor termina solo (por el SALIR) con código 0;
#   - valgrind: 0 errores y 0 bytes perdidos (definitely/indirectly lost).
# Cuenta la memoria que reserva TU código: sobre todo los nodos de la lista y el fichero del
# registro. El simulador no interviene (se arranca con SDL_VIDEODRIVER=dummy: sin ventana real).
# Sale con 77 (prueba omitida) si falta valgrind o nc.
set -u

# Sin ventanas: SDL usa un controlador de vídeo "dummy" (sin pantalla)
SDL_VIDEODRIVER=dummy
export SDL_VIDEODRIVER

SERVIDOR=${1:-./aeropuerto}
[ -x "$SERVIDOR" ] || { echo "No encuentro $SERVIDOR (compila primero)" >&2; exit 2; }
SERVIDOR=$(cd "$(dirname "$SERVIDOR")" && pwd)/$(basename "$SERVIDOR")
command -v valgrind >/dev/null 2>&1 || { echo "Omitida: falta valgrind"; exit 77; }
command -v nc >/dev/null 2>&1 || { echo "Omitida: falta nc"; exit 77; }
# valgrind y AddressSanitizer no se pueden combinar
if grep -q "__asan_init" "$SERVIDOR" 2>/dev/null; then echo "Omitida: el ejecutable usa AddressSanitizer (ya detecta fugas)"; exit 77; fi

PUERTO=$((20000 + $$ % 20000))
DIR=$(mktemp -d)
SPID=""
trap '[ -n "$SPID" ] && kill $SPID 2>/dev/null; rm -rf "$DIR"' EXIT
cd "$DIR" || exit 2

# Fichero inicial: sus aeronaves también son nodos que hay que liberar
cat > inicial.txt <<'FIN'
# aeronaves iniciales
301 Madrid Paris A320 80 75.4
302 Sevilla Roma B737 50 10
303 Mal formada
304 Bilbao Oslo A350 40 61.0
FIN

valgrind --leak-check=full --show-leak-kinds=definite,indirect --errors-for-leak-kinds=definite,indirect \
         --error-exitcode=9 --log-file=valgrind.log \
         "$SERVIDOR" "$PUERTO" inicial.txt > servidor.out 2>&1 &
SPID=$!

# valgrind es lento al arrancar: esperar a que el puerto acepte conexiones
i=0
until nc -z 127.0.0.1 "$PUERTO" 2>/dev/null; do
    sleep 0.3; i=$((i + 1))
    if [ $i -gt 100 ] || ! kill -0 "$SPID" 2>/dev/null; then
        echo "El servidor no arrancó bajo valgrind:"; cat servidor.out valgrind.log; exit 1
    fi
done

for p in 'ANADIR 1 Madrid Paris A320 75 62.5' 'ANADIR 3 Sevilla Roma B737 50 10' \
         'ANADIR 2 Bilbao Oslo A350 10 80' 'ANADIR 1 x y z 1 1' 'MODIFICAR 2 DESTINO Lima' \
         'MODIFICAR 2 CAPACIDAD 30' 'ORDENAR' 'ORDENAR' 'ELIMINAR 1' 'ELIMINAR 3' 'ELIMINAR 99' \
         'FOO' 'ANADIR 1' 'MOSTRAR' 'ANADIR 5 Cadiz Faro A320 20 30' 'SALIR'; do
    printf '%s\n' "$p" | timeout 20 nc 127.0.0.1 "$PUERTO" > /dev/null 2>&1
done

# El SALIR debe haber cerrado el servidor
i=0
while kill -0 "$SPID" 2>/dev/null && [ $i -lt 300 ]; do sleep 0.1; i=$((i + 1)); done
if kill -0 "$SPID" 2>/dev/null; then
    echo "[FAIL] el servidor no se cerró tras el SALIR"; exit 1
fi
wait "$SPID"; CODIGO=$?
SPID=""

FALLOS=0
if [ "$CODIGO" -eq 0 ]; then echo "  [ OK ] el servidor terminó con código 0"; else echo "  [FAIL] código de salida $CODIGO (9 = valgrind detectó errores/fugas)"; FALLOS=1; fi
if grep -q "ERROR SUMMARY: 0 errors" valgrind.log; then echo "  [ OK ] valgrind: 0 errores"; else echo "  [FAIL] valgrind: hay errores"; FALLOS=1; fi
if grep -q "definitely lost: [1-9]\|indirectly lost: [1-9]" valgrind.log; then echo "  [FAIL] valgrind: memoria perdida"; FALLOS=1; else echo "  [ OK ] valgrind: 0 bytes perdidos"; fi
grep -q "Memoria liberada correctamente" servidor.out && echo "  [ OK ] mensaje final del servidor" || { echo "  [FAIL] falta 'Memoria liberada correctamente'"; FALLOS=1; }

if [ $FALLOS -ne 0 ]; then echo "--- valgrind.log ---"; cat valgrind.log; echo "== PRUEBA DE MEMORIA: FALLOS =="; exit 1; fi
echo "== PRUEBA DE MEMORIA OK =="
