# Makefile de la práctica Aeropuerto
#
#   make                 compila ./aeropuerto y ./controlador
#   make tests           comprueba tus funciones de envío y recepción (compila y ejecuta)
#   make prueba          prueba de sistema: tu servidor real + nc (necesita ./aeropuerto)
#   make memoria         tu servidor real bajo valgrind: no debe dejar fugas
#   make valgrind        las comprobaciones de make tests bajo valgrind
#   make SIN_SDL=1       sin ventana gráfica
#   make clean
#
# PARTE DADA (casi todo el fichero): el simulador gráfico que descargó CMake, las pruebas y los objetivos.
# PARTE TUYA (AL FINAL del fichero, busca TODO): cómo se compilan y enlazan tus carpetas servidor/ y cliente/.
# Si todavía no has ejecutado 'cmake -S . -B build', hazlo antes: descarga lo que falta.

include config.mk          # lo genera CMake: BASE, SDL_PREFIX, HAY_SDL

ifeq ($(wildcard $(BASE)/simulator/ventana.c),)
$(error No encuentro lo descargado en $(BASE). Ejecuta primero: cmake -S . -B build)
endif

CC      ?= cc
CSTD     = -std=c11 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
WARN     = -Wall -Wextra -Wpedantic
OPT     ?= -O2 -g
CFLAGS_BASE = $(CSTD) $(WARN) $(OPT)
DEPFLAGS = -MMD -MP
LDLIBS   = -pthread -lm
BUILD    = build/make

.DEFAULT_GOAL := all      # "make" a secas construye los dos ejecutables

# ======================================================================================
#  PARTE DADA: simulador gráfico
# ======================================================================================

ifeq ($(SIN_SDL),1)
  HAY_SDL = no
endif
ifeq ($(HAY_SDL),si)
  ifeq ($(SDL_PREFIX),)
    PKGCONF    = pkg-config
    SDL_STATIC =
  else
    PKGCONF    = PKG_CONFIG_PATH=$(SDL_PREFIX)/lib/pkgconfig:$(SDL_PREFIX)/lib64/pkgconfig pkg-config
    SDL_STATIC = --static
  endif
  GRAF_SRC   = $(BASE)/src/simulador.c $(BASE)/simulator/ventana.c $(BASE)/simulator/renderer.c
  SDL_CFLAGS = $(shell $(PKGCONF) --cflags sdl2)
  SDL_LIBS   = $(shell $(PKGCONF) $(SDL_STATIC) --libs sdl2)
else
  GRAF_SRC   = $(BASE)/src/simulador_nulo.c
  $(info [Makefile] Sin SDL2 (o SIN_SDL=1): se compila SIN ventana gráfica.)
endif
GRAF_OBJ = $(addprefix $(BUILD)/base/,$(notdir $(GRAF_SRC:.c=.o)))

# El simulador usa tus estructuras (servidor/include/estructuras.h y simulador.h)
$(BUILD)/base/%.o: $(BASE)/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) $(DEPFLAGS) $(CFLAGS) -I$(BASE)/simulator -Iservidor/include $(SDL_CFLAGS) -c $< -o $@

$(BUILD)/base/%.o: $(BASE)/simulator/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) $(DEPFLAGS) $(CFLAGS) -I$(BASE)/simulator -Iservidor/include $(SDL_CFLAGS) -c $< -o $@

# ======================================================================================
#  PARTE DADA: objetivos, comprobaciones y pruebas
# ======================================================================================

all: aeropuerto controlador

# Si falta una regla (porque todavía no has completado la parte TUYA, al final de este fichero),
# make avisa de lo que falta en lugar de dar un error difícil de entender.
.DEFAULT:
	@echo "Falta completar el Makefile: no sé cómo construir '$@'. Completa la parte TUYA (al final del fichero, busca TODO)."; exit 1

# make tests: comprueba tus funciones de comunicacion.c (envío y recepción por el socket).
# Cada comprobación se compila directamente con el comunicacion.c de su lado.
CLI_TESTS = $(basename $(notdir $(wildcard tests/test_*cliente*.c)))
SRV_TESTS = $(basename $(notdir $(wildcard tests/test_*servidor*.c)))
TEST_BIN  = $(addprefix $(BUILD)/tests/,$(SRV_TESTS) $(CLI_TESTS))

$(addprefix $(BUILD)/tests/,$(SRV_TESTS)): $(BUILD)/tests/%: tests/%.c servidor/src/comunicacion.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) $(CFLAGS) -Iservidor/include -Itests $^ -o $@ $(LDLIBS)

$(addprefix $(BUILD)/tests/,$(CLI_TESTS)): $(BUILD)/tests/%: tests/%.c cliente/src/comunicacion.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BASE) $(CFLAGS) -Icliente/include -Itests $^ -o $@ $(LDLIBS)

tests: $(TEST_BIN)
	@fallos=0; for t in $(TEST_BIN); do ./$$t || fallos=1; done; \
	if [ $$fallos -eq 0 ]; then echo "== TODAS LAS COMPROBACIONES OK =="; else echo "== HAY COMPROBACIONES FALLIDAS =="; exit 1; fi

prueba: aeropuerto controlador
	sh tests/prueba_sistema.sh ./aeropuerto ./controlador

memoria: aeropuerto
	sh tests/prueba_memoria.sh ./aeropuerto

valgrind: $(TEST_BIN)
	@for t in $(TEST_BIN); do valgrind --leak-check=full --error-exitcode=1 -q ./$$t || exit 1; done; \
	echo "== valgrind: sin errores ni fugas =="

.PHONY: all tests prueba memoria valgrind clean
.SECONDARY:

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)

# ======================================================================================
#  PARTE TUYA: servidor/ y cliente/
# ======================================================================================

#=================== INICIO DE TU CODIGO ====================
# TODO (estudiante): variables con los ficheros de tu servidor y de tu cliente
# PISTA: SRV_FUENTES: todos los .c de servidor/src/ (usa $(wildcard servidor/src/*.c))
# PISTA: SRV_OBJ: lo mismo pero como objetos en $(BUILD)/servidor/NOMBRE.o (usa $(patsubst ...))
# PISTA: haz igual con CLI_FUENTES y CLI_OBJ para cliente/src/
# PISTA: usa esos mismos nombres (SRV_OBJ y CLI_OBJ) en las reglas de más abajo
#==================== FIN DE TU CODIGO =====================


#=================== INICIO DE TU CODIGO ====================
# TODO (estudiante): reglas para compilar los .c del servidor y los del cliente (un objeto por fichero)
# PISTA: regla de patrón:  $(BUILD)/servidor/%.o: servidor/src/%.c
# PISTA: receta: @mkdir -p $(dir $@)   y   $(CC) $(CFLAGS_BASE) $(DEPFLAGS) $(CFLAGS) -Iservidor/include -c $< -o $@
# PISTA: para el cliente igual, pero con -Icliente/include (¡el servidor y el cliente tienen cada uno su comunicacion.h!)
# PISTA: recuerda que las recetas empiezan con un TABULADOR, no con espacios
#==================== FIN DE TU CODIGO =====================


#=================== INICIO DE TU CODIGO ====================
# TODO (estudiante): enlazar el ejecutable ./aeropuerto (servidor) y ./controlador (cliente)
# PISTA: aeropuerto necesita: $(SRV_OBJ), $(GRAF_OBJ) (el simulador) y las librerías $(SDL_LIBS) $(LDLIBS)
# PISTA: controlador necesita: $(CLI_OBJ) y $(LDLIBS) (el cliente no usa el simulador)
# PISTA: receta de enlazado:  $(CC) $(LDFLAGS) -o $@ $^ ...librerías...   ($^ = todos los prerrequisitos)
#==================== FIN DE TU CODIGO =====================


#=================== INICIO DE TU CODIGO ====================
# TODO (estudiante): objetivo clean
# PISTA: borra la carpeta $(BUILD) y los ejecutables aeropuerto y controlador (rm -rf ...)
#==================== FIN DE TU CODIGO =====================

