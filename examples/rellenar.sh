#!/bin/sh
# rellenar.sh - Añade aeronaves de ejemplo al servidor que ya está en marcha (útil para ver el simulador).
#
#   ejemplos/rellenar.sh PUERTO [CANTIDAD]       (por defecto 8; máximo 100)
#
# Hace una petición ANADIR por conexión, igual que el controlador. Necesita 'nc' (netcat).
set -eu

PUERTO=${1:?"Uso: $0 PUERTO [CANTIDAD]"}
CANTIDAD=${2:-8}
command -v nc >/dev/null 2>&1 || { echo "Falta 'nc' (netcat)" >&2; exit 1; }

CIUDADES="Madrid Barcelona Sevilla Valencia Bilbao Paris Londres Roma Lisboa Oslo"
MODELOS="A320 B737 A350 A380 B777 E190"

i=0
while [ "$i" -lt "$CANTIDAD" ]; do
    # los ID salen desordenados a propósito para que se vea el efecto de ORDENAR
    id=$(( (i * 37 + 101) % 400 + 100 ))
    origen=$(echo $CIUDADES | cut -d' ' -f$(( i % 10 + 1 )))
    destino=$(echo $CIUDADES | cut -d" " -f$(( (i * 3 + 5) % 10 + 1 )))
    modelo=$(echo $MODELOS | cut -d' ' -f$(( i % 6 + 1 )))
    capacidad=$(( (i * 23 + 15) % 101 ))
    combustible=$(( (i * 31 + 20) % 100 )).5
    printf 'ANADIR %s %s %s %s %s %s\n' "$id" "$origen" "$destino" "$modelo" "$capacidad" "$combustible" |
        nc 127.0.0.1 "$PUERTO" | head -1
    i=$((i + 1))
done
