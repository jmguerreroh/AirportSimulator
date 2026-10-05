#!/bin/sh
# prueba_sistema.sh - Prueba de extremo a extremo de TU servidor real (sin abrir ventanas).
#
#   tests/prueba_sistema.sh [RUTA_AEROPUERTO [RUTA_CONTROLADOR]]     (por defecto ./aeropuerto y ./controlador)
#
# Cada petición va en su PROPIA conexión (como hace el controlador), usando 'nc':
#   1. MOSTRAR devuelve "OK:" y el estado;
#   2. ANADIR, errores (duplicado, comando desconocido, formato, rangos) devuelven OK:/ERROR: + estado;
#   3. varias conexiones seguidas ven el MISMO estado;
#   4. ORDENAR, MODIFICAR y ELIMINAR funcionan;
#   5. el servidor SIGUE vivo tras todo eso y se cierra solo cuando llega SALIR;
#   6. aeropuerto.log existe y contiene lo ocurrido;
#   7. si se pasa un FICHERO como argumento, el servidor carga esas aeronaves al arrancar
#      (ignorando las líneas incorrectas) y si el fichero no existe termina con error;
#   8. los argumentos de ./aeropuerto y de ./controlador se validan (mensaje de uso y código de
#      salida distinto de 0), y el controlador avisa si no hay servidor.
# Requisitos: nc (netcat), timeout, grep.
set -u

# Las pruebas no deben abrir ventanas: SDL usa un controlador de vídeo "dummy" (sin pantalla)
SDL_VIDEODRIVER=dummy
export SDL_VIDEODRIVER

SERVIDOR=${1:-./aeropuerto}
if [ ! -x "$SERVIDOR" ]; then
    echo "No encuentro el ejecutable $SERVIDOR (compila primero)" >&2
    exit 2
fi
SERVIDOR=$(cd "$(dirname "$SERVIDOR")" && pwd)/$(basename "$SERVIDOR")
CLIENTE=${2:-$(dirname "$SERVIDOR")/controlador}
[ -x "$CLIENTE" ] && CLIENTE=$(cd "$(dirname "$CLIENTE")" && pwd)/$(basename "$CLIENTE")
command -v nc >/dev/null 2>&1 || { echo "Falta 'nc' (netcat)" >&2; exit 2; }

PUERTO=$((20000 + $$ % 20000))
DIR=$(mktemp -d)
FALLOS=0
SPID=""

ok()    { echo "  [ OK ] $1"; }
fallo() { echo "  [FAIL] $1"; FALLOS=$((FALLOS + 1)); }
comprueba() {   # comprueba "descripcion" fichero patron
    if grep -q -- "$3" "$2" 2>/dev/null; then ok "$1"; else fallo "$1 (no aparece: $3)"; fi
}
peticion() {    # peticion "linea" fichero_salida   (UNA conexión por petición)
    printf '%s\n' "$1" | timeout 5 nc 127.0.0.1 "$PUERTO" > "$2" 2>&1
}
limpiar() {
    [ -n "$SPID" ] && kill "$SPID" 2>/dev/null
    wait 2>/dev/null
    rm -rf "$DIR"
}
trap limpiar EXIT

cd "$DIR" || exit 2
"$SERVIDOR" "$PUERTO" > servidor.out 2>&1 &
SPID=$!
sleep 1
if ! kill -0 "$SPID" 2>/dev/null; then
    echo "El servidor no arrancó:"; cat servidor.out; exit 1
fi
echo "Servidor en el puerto $PUERTO (pid $SPID)"

echo "Peticiones (una conexión cada una):"
peticion 'MOSTRAR' r1.out
comprueba "MOSTRAR: primera línea OK:"            r1.out '^OK: '
comprueba "MOSTRAR: estado vacío"                 r1.out '^--- ESTADO ACTUAL: 0 aeronaves ---'
peticion 'ANADIR 103 Madrid Paris A320 75 62.5' r2.out
comprueba "ANADIR correcto"                       r2.out '^OK: Aeronave 103 a.*adida correctamente\.'
comprueba "estado tras el ANADIR"                 r2.out '^ID 103 | Madrid -> Paris | A320 | 75% | 62.5'
peticion 'ANADIR 101 Sevilla Roma B737 50 10' r3.out
peticion 'ANADIR 103 X Y Z 1 1' r4.out
comprueba "ANADIR duplicado es un ERROR:"         r4.out '^ERROR: ya existe una aeronave con ID 103\.'
comprueba "el ERROR también trae estado"          r4.out '^--- ESTADO ACTUAL: 2 aeronaves ---'
peticion 'FOO' r5.out
comprueba "comando desconocido"                   r5.out '^ERROR: operaci.*n desconocida\.'
peticion 'ANADIR 1' r6.out
comprueba "formato incorrecto"                    r6.out '^ERROR: formato de petici.*n incorrecto\.'
peticion 'ANADIR 7 a b c 101 1' r7.out
comprueba "capacidad fuera de rango"              r7.out '^ERROR: capacidad fuera de rango\.'
peticion 'ANADIR 8 a b c 10 -3' r8.out
comprueba "combustible inválido"                  r8.out '^ERROR: combustible inv.*lido\.'
peticion 'ANADIR -5 a b c 10 3' r9.out
comprueba "ID inválido"                           r9.out '^ERROR: ID inv.*lido\.'
peticion 'ORDENAR' r10.out
comprueba "ORDENAR ordena"                        r10.out '^OK: Aeropuerto ordenado por ID\.'
peticion 'ORDENAR' r11.out
comprueba "ORDENAR sobre lista ordenada"          r11.out '^OK: El aeropuerto ya estaba ordenado por ID\.'
peticion 'MODIFICAR 103 DESTINO Roma' r12.out
comprueba "MODIFICAR actualiza el campo"          r12.out '^ID 103 | Madrid -> Roma | A320 | 75% | 62.5'
peticion 'MODIFICAR 999 DESTINO Roma' r13.out
comprueba "MODIFICAR de un ID inexistente"        r13.out '^ERROR: no existe ninguna aeronave con ID 999\.'
peticion 'ELIMINAR 101' r14.out
comprueba "ELIMINAR correcto"                     r14.out '^OK: Aeronave 101 eliminada correctamente\.'
peticion 'ELIMINAR 101' r15.out
comprueba "ELIMINAR de un ID inexistente"         r15.out '^ERROR: no existe ninguna aeronave con ID 101\.'
peticion 'MOSTRAR' r16.out
comprueba "otra conexión ve el estado compartido" r16.out '^--- ESTADO ACTUAL: 1 aeronaves ---'

if kill -0 "$SPID" 2>/dev/null; then ok "el servidor sigue vivo tras las peticiones"; else fallo "el servidor se cerró sin recibir SALIR"; fi

echo "Cierre:"
peticion 'SALIR' q.out
comprueba "SALIR: respuesta OK: con estado"       q.out '^OK: Cierre aceptado\. El aeropuerto se cierra\.'
i=0
while kill -0 "$SPID" 2>/dev/null && [ $i -lt 50 ]; do sleep 0.1; i=$((i + 1)); done
if kill -0 "$SPID" 2>/dev/null; then
    fallo "el servidor NO se cerró tras el SALIR"
else
    ok "el servidor se cerró tras el SALIR"
fi
SPID=""

echo "Registro (aeropuerto.log):"
if [ -f aeropuerto.log ]; then
    ok "existe aeropuerto.log"
    comprueba "arranque"                          aeropuerto.log 'Servidor iniciado en el puerto'
    comprueba "petición registrada con la IP y el puerto del cliente" aeropuerto.log '\[127\.0\.0\.1:[0-9][0-9]*\] Petici.*n: ANADIR 103 Madrid Paris'
    comprueba "respuesta registrada con la IP y el puerto"            aeropuerto.log '\[127\.0\.0\.1:[0-9][0-9]*\] Respuesta: ERROR: ya existe'
    comprueba "petición SALIR"                    aeropuerto.log '\[127\.0\.0\.1:[0-9][0-9]*\] Petici.*n: SALIR'
    comprueba "motivo del cierre"                 aeropuerto.log 'Cierre del servidor: .*SALIR'
    comprueba "fin"                               aeropuerto.log 'Servidor finalizado'
else
    fallo "no existe aeropuerto.log"
fi
comprueba "mensaje final por pantalla"            servidor.out 'Memoria liberada correctamente'
comprueba "mensaje final por pantalla"            servidor.out 'Servidor finalizado'

# ---------------------------------------------------------------------------------------------
echo "Fichero inicial como argumento:"
cat > inicial.txt <<'FIN'
# comentario: se ignora

201 Madrid Paris A320 80 75.4
202 Sevilla Roma B737 50 10
203 Mal formada
201 Repetida X Y 10 5
204 Bilbao Oslo A350 101 5
205 Cadiz Faro E190 20 30
FIN
PUERTO=$((PUERTO + 1))
"$SERVIDOR" "$PUERTO" inicial.txt > servidor2.out 2> servidor2.err &
SPID=$!
sleep 1
if ! kill -0 "$SPID" 2>/dev/null; then
    fallo "el servidor no arrancó con un fichero inicial"; cat servidor2.out servidor2.err
else
    peticion 'MOSTRAR' f1.out
    comprueba "carga 3 aeronaves del fichero"       f1.out '^--- ESTADO ACTUAL: 3 aeronaves ---'
    comprueba "carga la 201 (primera, tras comentario)" f1.out '^ID 201 | Madrid -> Paris | A320 | 80% | 75.4'
    comprueba "carga la 202"                        f1.out '^ID 202 | Sevilla -> Roma | B737 | 50% | 10.0'
    comprueba "carga la 205 (tras las líneas malas)" f1.out '^ID 205 | Cadiz -> Faro | E190 | 20% | 30.0'
    if grep -q '^ID 204' f1.out; then fallo "cargó la 204 (capacidad fuera de rango)"; else ok "ignora la 204 (capacidad inválida)"; fi
    if grep -q 'Repetida' f1.out; then fallo "cargó el ID repetido"; else ok "ignora el ID repetido"; fi
    peticion 'SALIR' f2.out
    i=0
    while kill -0 "$SPID" 2>/dev/null && [ $i -lt 50 ]; do sleep 0.1; i=$((i + 1)); done
    SPID=""
    comprueba "aviso de lo cargado por pantalla"    servidor2.out 'Cargadas 3 aeronaves'
    comprueba "registro de la carga"                aeropuerto.log 'Cargadas 3 aeronaves'
    comprueba "registro de las líneas ignoradas"    aeropuerto.log 'L.*nea [0-9]* del fichero ignorada'
fi

"$SERVIDOR" "$((PUERTO + 1))" no_existe.txt > servidor3.out 2> servidor3.err &
SPID=$!
i=0
while kill -0 "$SPID" 2>/dev/null && [ $i -lt 20 ]; do sleep 0.1; i=$((i + 1)); done
if kill -0 "$SPID" 2>/dev/null; then
    fallo "con un fichero inexistente el servidor debe terminar con error"
else
    wait "$SPID"; CODIGO=$?
    [ "$CODIGO" -ne 0 ] && ok "fichero inexistente: el servidor termina con error" || fallo "fichero inexistente: debe salir con código distinto de 0"
fi
SPID=""

# ---------------------------------------------------------------------------------------------
# Argumentos: debe salir con un código distinto de 0 y mostrar el mensaje de uso (nunca arrancar ni colgarse)
usoerror() {    # usoerror "descripción" ejecutable [argumentos...]
    desc=$1; shift
    timeout 3 "$@" > argumentos.out 2> argumentos.err < /dev/null
    codigo=$?
    if [ "$codigo" -eq 124 ]; then
        fallo "$desc: el programa no terminó (debe rechazar los argumentos y salir)"
    elif [ "$codigo" -eq 0 ]; then
        fallo "$desc: debe terminar con un código distinto de 0"
    elif ! grep -q "Uso:" argumentos.err; then
        fallo "$desc: debe mostrar el mensaje de uso (Uso: ...) por la salida de error"
    else
        ok "$desc"
    fi
}

echo "Argumentos del servidor:"
usoerror "sin argumentos"                         "$SERVIDOR"
usoerror "puerto no numérico"                     "$SERVIDOR" abc
usoerror "puerto 0"                               "$SERVIDOR" 0
usoerror "puerto 65536 (fuera de rango)"          "$SERVIDOR" 65536
usoerror "argumentos de más"                      "$SERVIDOR" "$((PUERTO + 2))" inicial.txt otro
if [ -x "$CLIENTE" ]; then
    echo "Argumentos del controlador:"
    usoerror "sin argumentos"                     "$CLIENTE"
    usoerror "falta el puerto"                    "$CLIENTE" 127.0.0.1
    usoerror "IP inválida"                        "$CLIENTE" 999.1.1.1 5000
    usoerror "IP con letras"                      "$CLIENTE" localhost-no 5000
    usoerror "puerto no numérico"                 "$CLIENTE" 127.0.0.1 abc
    usoerror "puerto 0"                           "$CLIENTE" 127.0.0.1 0
    usoerror "argumentos de más"                  "$CLIENTE" 127.0.0.1 5000 extra
    # argumentos correctos pero sin servidor escuchando: avisa y termina (no se cuelga)
    timeout 5 "$CLIENTE" 127.0.0.1 "$((PUERTO + 60))" > sinservidor.out 2> sinservidor.err < /dev/null
    codigo=$?
    if [ "$codigo" -eq 124 ]; then fallo "sin servidor: el controlador se queda colgado"
    elif [ "$codigo" -eq 0 ]; then fallo "sin servidor: debe terminar con un código distinto de 0"
    elif [ ! -s sinservidor.err ]; then fallo "sin servidor: debe avisar por la salida de error"
    else ok "sin servidor: avisa y termina"; fi
else
    echo "  (se omiten los argumentos del controlador: no encuentro $CLIENTE)"
fi

if [ "$FALLOS" -eq 0 ]; then echo "== PRUEBA DE SISTEMA OK =="; exit 0; fi
echo "== PRUEBA DE SISTEMA: $FALLOS fallos =="; exit 1
