#include "render.h"
#include "camara.h"
#include "ppm.h"
#include "sdl_vista.h"
#include "sombreado.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int renderizar(const Escena *esc) {
    float aspecto    = (float)esc->ancho / (float)esc->alto;
    float fov_rad    = esc->fov_grados * (float)M_PI / 180.0f;
    float alto_plano = 2.0f * tanf(fov_rad * 0.5f);
    float ancho_plano = aspecto * alto_plano;

    size_t total_px = (size_t)esc->ancho * (size_t)esc->alto;
    unsigned char *pixeles = (unsigned char *)malloc(total_px * 3u);
    if (!pixeles) { perror("malloc"); return 0; }
    memset(pixeles, 0, total_px * 3u);

    int quiere_ventana = (esc->modo & MODO_VENTANA) ? 1 : 0;
    int quiere_archivo = (esc->modo & MODO_ARCHIVO) ? 1 : 0;

    VistaSDL *vista = NULL;
    if (quiere_ventana) {
        if (!sdl_disponible()) {
            fprintf(stderr,
                    "Aviso: compilado sin SDL2; MODE %s degradado a FILE.\n",
                    esc->modo == MODO_VENTANA ? "WINDOW" : "BOTH");
            quiere_ventana = 0;
            if (!quiere_archivo) quiere_archivo = 1;
        } else {
            vista = sdl_iniciar(esc->ancho, esc->alto, "Proyecto 3 - Ray Tracer");
            if (!vista) {
                fprintf(stderr, "No se pudo iniciar la ventana SDL; usando solo archivo.\n");
                quiere_ventana = 0;
            }
        }
    }

    int abortado = 0;

    for (int j = 0; j < esc->alto && !abortado; j++) {
        for (int i = 0; i < esc->ancho; i++) {
            Vec3 D = direccion_rayo(esc, i, j, ancho_plano, alto_plano);
            Vec3 col = trazar_color(esc, esc->ojo, D);

            unsigned char r = a_byte(col.x);
            unsigned char g = a_byte(col.y);
            unsigned char b = a_byte(col.z);

            size_t k = ((size_t)j * (size_t)esc->ancho + (size_t)i) * 3u;
            pixeles[k + 0] = r;
            pixeles[k + 1] = g;
            pixeles[k + 2] = b;

            if (vista) sdl_pintar_pixel(vista, i, j, r, g, b);
        }

        if (vista) {
            if ((j & 7) == 0 || j == esc->alto - 1) sdl_actualizar(vista);
            if (sdl_revisar_salida(vista)) { abortado = 1; break; }
        }
    }

    if (quiere_archivo) {
        if (!escribir_ppm(esc->ruta_salida, esc->ancho, esc->alto, pixeles))
            fprintf(stderr, "Error al escribir %s\n", esc->ruta_salida);
        else
            fprintf(stderr, "Escrito %s (%dx%d)\n", esc->ruta_salida, esc->ancho, esc->alto);
    }

    if (vista) {
        sdl_actualizar(vista);
        fprintf(stderr, "Render completo. Cierra la ventana o presiona ESC.\n");
        if (!abortado) sdl_esperar_cierre(vista);
        sdl_finalizar(vista);
    }

    free(pixeles);
    return 1;
}
