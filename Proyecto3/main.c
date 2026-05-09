/*
 * Proyecto 3 — Ray tracer de esferas (C, Linux).
 * Profesor: Ernesto Rivera Alvarado.
 *
 * Punto de entrada. Carga la escena desde el archivo indicado por argv
 * (por defecto scene_snowman.txt) y delega el render a renderizar().
 *
 * Estructura del proyecto:
 *   vec3.h                 Primitivas vectoriales 3D.
 *   tipos.h                Tipos de dominio (Esfera, Luz, Escena, ...).
 *   escena.{h,c}           Carga de la escena desde archivo de texto.
 *   geometria.{h,c}        Intersección rayo-esfera y shadow rays.
 *   sombreado.{h,c}        Modelo Phong (difuso + especular + ambiente).
 *   camara.{h,c}           Cámara fija mirando -Z.
 *   ppm.{h,c}              Salida a PPM P6 sin pérdida.
 *   sdl_vista.{h,c}        Encapsula SDL2 (solo ventana + pintar píxeles).
 *   render.{h,c}           Loop principal que integra todo.
 *   main.c                 (este archivo).
 */
#include "escena.h"
#include "render.h"

#include <stdio.h>

int main(int argc, char **argv) {
    const char *ruta_escena = "scene_snowman.txt";
    if (argc >= 2) ruta_escena = argv[1];

    Escena esc;
    if (!cargar_escena(ruta_escena, &esc)) return 1;

    fprintf(stderr,
            "Render: %dx%d, esferas=%d, luces=%d, modo=%s, sombreado=%s\n",
            esc.ancho, esc.alto, esc.num_esferas, esc.num_luces,
            esc.modo == MODO_ARCHIVO ? "FILE" :
            esc.modo == MODO_VENTANA ? "WINDOW" : "BOTH",
            esc.sombreado == SOMBREADO_PHONG ? "PHONG" : "FLAT");

    if (!renderizar(&esc)) return 1;
    return 0;
}
