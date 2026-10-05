# PracticaAeropuerto.cmake - Funciones del proyecto.
#
# ----- Para quien hace la práctica (cuando se trae este repositorio con FetchContent) --------------------------
#
#   include(FetchContent)
#   FetchContent_Declare(Aeropuerto
#     GIT_REPOSITORY https://github.com/jmguerreroh/AirportSimulator.git GIT_TAG main)
#   FetchContent_MakeAvailable(Aeropuerto)
#   aeropuerto_practica()
#
# aeropuerto_practica():
#   CMake SOLO descarga y prepara; el programa se compila con el Makefile (que completa el estudiantado):
#   1. Descarga el proyecto base y, si no esta instalado, SDL2 (lo compila e instala en build/sdl2-install).
#   2. La primera vez CREA en la carpeta de trabajo (si no existen; NUNCA pisa tu trabajo):
#        cliente/include, cliente/src       plantillas del controlador
#        servidor/include, servidor/src     plantillas del aeropuerto
#        Makefile                           con la infraestructura hecha y TODO para cliente y servidor
#        tests/                             tests y pruebas de sistema
#        ENUNCIADO.md, docs/, ejemplos/     documentacion y datos de ejemplo
#   3. Regenera config.mk (lo usa el Makefile): ruta de lo descargado y flags de SDL2.
#   Despues:  make

set(AERO_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")   # visible desde el CMakeLists de quien hace la práctica

# Copia 'origen' a 'destino' solo si 'destino' no existe.
function(aeropuerto_copiar_si_falta origen destino)
  if(NOT EXISTS "${destino}")
    get_filename_component(carpeta "${destino}" DIRECTORY)
    file(COPY "${origen}" DESTINATION "${carpeta}")
    get_filename_component(nombre_origen "${origen}" NAME)
    get_filename_component(nombre_destino "${destino}" NAME)
    if(NOT nombre_origen STREQUAL nombre_destino)
      file(RENAME "${carpeta}/${nombre_origen}" "${destino}")
    endif()
    file(RELATIVE_PATH rel "${CMAKE_SOURCE_DIR}" "${destino}")
    message(STATUS "Plantilla creada en tu carpeta: ${rel}")
  endif()
endfunction()

# Funcion para el CMakeLists.txt de quien hace la práctica.
function(_aeropuerto_practica)
  get_filename_component(raiz "${AERO_CMAKE_DIR}/.." ABSOLUTE)
  set(dir "${CMAKE_SOURCE_DIR}")      # carpeta del proyecto de quien hace la práctica

  # 1) Plantillas: todo lo que hay en estudiante/ (menos config.mk.in) se copia si falta
  file(GLOB_RECURSE plantillas RELATIVE "${raiz}/estudiante" "${raiz}/estudiante/*")
  foreach(rel ${plantillas})
    if(NOT rel STREQUAL "config.mk.in")
      aeropuerto_copiar_si_falta("${raiz}/estudiante/${rel}" "${dir}/${rel}")
    endif()
  endforeach()
  aeropuerto_copiar_si_falta("${raiz}/docs/ENUNCIADO.md" "${dir}/ENUNCIADO.md")
  aeropuerto_copiar_si_falta("${raiz}/docs/PROTOCOLO.md" "${dir}/docs/PROTOCOLO.md")
  aeropuerto_copiar_si_falta("${raiz}/docs/API.md" "${dir}/docs/API.md")
  aeropuerto_copiar_si_falta("${raiz}/examples/aeropuerto_inicial.txt"
                             "${dir}/ejemplos/aeropuerto_inicial.txt")
  aeropuerto_copiar_si_falta("${raiz}/examples/rellenar.sh"
                             "${dir}/ejemplos/rellenar.sh")
  if(NOT EXISTS "${dir}/.gitignore")
    file(WRITE "${dir}/.gitignore" "build/\naeropuerto\ncontrolador\n*.log\n*.o\n")
  endif()

  # 2) config.mk: lo que el Makefile necesita saber de lo descargado (se regenera siempre)
  set(AERO_BASE_DIR "${raiz}/base")
  configure_file("${raiz}/estudiante/config.mk.in" "${dir}/config.mk" @ONLY)

  message(STATUS "")
  message(STATUS "Todo descargado y preparado. Completa el Makefile (busca TODO) y compila con:  make")
endfunction()

macro(aeropuerto_practica)
  _aeropuerto_practica()
endmacro()
