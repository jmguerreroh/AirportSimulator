cmake_minimum_required(VERSION 3.16)
project(AirportSimulator VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Tipo de compilacion" FORCE)
endif()

# Este proyecto NO compila el programa: el estudiantado lo trae con FetchContent y
# aeropuerto_practica() solo DESCARGA y PREPARA (ver README.md). Se compila con make.

option(AEROPUERTO_SIN_SDL "Compilar sin simulador grafico (sin SDL2)" OFF)
option(AEROPUERTO_FORZAR_DESCARGA_SDL2 "Descargar y compilar SDL2 aunque este instalado" OFF)


# ---- SDL2 (simulador grafico): el del sistema si existe; si no, se DESCARGA, se compila y se
#      instala en una carpeta local (build/sdl2-install). Lo usa el Makefile de quien hace la práctica (config.mk).
set(AERO_SDL_TARGET "")
set(AERO_SDL_PREFIX "" CACHE INTERNAL "")     # vacio = SDL2 del sistema
set(AERO_HAY_SDL "no" CACHE INTERNAL "")
if(NOT AEROPUERTO_SIN_SDL)
  if(NOT AEROPUERTO_FORZAR_DESCARGA_SDL2)
    find_package(SDL2 QUIET)
    if(TARGET SDL2::SDL2)
      set(AERO_SDL_TARGET SDL2::SDL2)
    else()
      find_package(PkgConfig QUIET)
      if(PkgConfig_FOUND)
        pkg_check_modules(AERO_SDL2 QUIET IMPORTED_TARGET sdl2)
        if(AERO_SDL2_FOUND)
          set(AERO_SDL_TARGET PkgConfig::AERO_SDL2)
        endif()
      endif()
    endif()
  endif()

  if(NOT AERO_SDL_TARGET)
    message(STATUS "SDL2 no encontrado en el sistema: descargandolo y compilandolo (solo la primera vez)")
    include(FetchContent)
    if(POLICY CMP0169)
      cmake_policy(SET CMP0169 OLD)           # FetchContent_Populate directo
    endif()
    FetchContent_Declare(SDL2
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-2.30.9
      GIT_SHALLOW TRUE)
    FetchContent_GetProperties(SDL2)
    if(NOT sdl2_POPULATED)
      FetchContent_Populate(SDL2)
    endif()
    set(sdl_prefix "${CMAKE_BINARY_DIR}/sdl2-install")
    if(NOT EXISTS "${sdl_prefix}/lib/libSDL2.a")
      set(sdl_build "${CMAKE_BINARY_DIR}/sdl2-build")
      execute_process(COMMAND ${CMAKE_COMMAND} -S "${sdl2_SOURCE_DIR}" -B "${sdl_build}"
                              -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST=OFF
                              -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${sdl_prefix}
                      RESULT_VARIABLE r1 OUTPUT_QUIET)
      execute_process(COMMAND ${CMAKE_COMMAND} --build "${sdl_build}" --parallel
                      RESULT_VARIABLE r2 OUTPUT_QUIET)
      execute_process(COMMAND ${CMAKE_COMMAND} --install "${sdl_build}"
                      RESULT_VARIABLE r3 OUTPUT_QUIET)
      if(NOT r1 EQUAL 0 OR NOT r2 EQUAL 0 OR NOT r3 EQUAL 0)
        message(WARNING "No se pudo compilar SDL2: se continua SIN simulador grafico")
      endif()
    endif()
    if(EXISTS "${sdl_prefix}/lib/libSDL2.a")
      find_package(SDL2 CONFIG QUIET PATHS "${sdl_prefix}" NO_DEFAULT_PATH)
      if(TARGET SDL2::SDL2)
        set(AERO_SDL_TARGET SDL2::SDL2)
      elseif(TARGET SDL2::SDL2-static)
        set(AERO_SDL_TARGET SDL2::SDL2-static)
      endif()
      set(AERO_SDL_PREFIX "${sdl_prefix}" CACHE INTERNAL "")
    endif()
  endif()
endif()
if(AERO_SDL_TARGET)
  set(AERO_HAY_SDL "si" CACHE INTERNAL "")
  message(STATUS "Simulador grafico: SDL2")
else()
  message(STATUS "Simulador grafico: DESACTIVADO (sin SDL2)")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PracticaAeropuerto.cmake)
