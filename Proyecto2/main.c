#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "framebuffer.h"
#include "rasterizer.h"
#include "scene_geometry.h"
#include "texture.h"
#include "texture_bank.h"
#include "transform.h"
#include "view_control.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Modos de dibujo
enum {
    MODO_ALAMBRE = 0,
    MODO_RELLENO = 1,
    MODO_TEXTURAS_VARIAS = 2,
    MODO_UNA_TEXTURA = 3
};

// Ventana inicial
static const VentanaMundo ventana_inicial = {0.0, 1000.0, 0.0, 500.0};

/* Normaliza el ángulo para evitar pérdida de precisión en cos/sin con argumentos enormes. (-pi, pi]. */
static double normalizar_angulo_vista(double a) {
    const double tpi = 2.0 * M_PI;
    a -= tpi * floor((a + M_PI) / tpi);
    return a;
}

// Actualiza la pantalla con el framebuffer
static void actualizar_pantalla(const Framebuffer *fb, SDL_Renderer *ren, SDL_Texture *tex_sdl) {
    SDL_UpdateTexture(tex_sdl, NULL, fb->pixels, fb->width * 3);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex_sdl, NULL, NULL);
    SDL_RenderPresent(ren);
}

static void dibujar_escena(Framebuffer *fb, const VentanaMundo *ventana, const RectVista *vista, int modo,
                           const BancoTexturas *banco, int cantidad_texturas, int indice_textura_unica,
                           const Texture *textura_respaldo, double angulo_vista_rad, int bordes_negros) {
    // Agregué un pivote porque al hacer pan la escena giraba alrededor del centro de la pantalla.
    double piv_x, piv_y;
    centro_geometrico_escena_mundo(&piv_x, &piv_y);

    Vertice buf_rot[TAM_BUF_VERTICES];

    for (int i = 0; i < num_poligonos_escena(); i++) {
        const PoligonoEscena *pol = poligono_escena_en(i);
        if (!pol || pol->num_vertices < 2)
            continue;
        if (pol->num_vertices > TAM_BUF_VERTICES)
            continue;

        const Vertice *dibujo = pol->puntos;
        int n = pol->num_vertices;

        if (angulo_vista_rad != 0.0) {
            copiar_poligono_rotado(pol->puntos, pol->num_vertices, piv_x, piv_y, angulo_vista_rad, buf_rot);
            dibujo = buf_rot;
            n = pol->num_vertices;
        }

        if (pol->cerrado) {
            if (pol->num_vertices < 3)
                continue;
            if (modo == MODO_ALAMBRE) {
                if (bordes_negros)
                    dibujar_contorno_poligono(fb, dibujo, n, ventana, vista, 0, 0, 0);
                else
                    dibujar_contorno_poligono(fb, dibujo, n, ventana, vista, pol->r, pol->g, pol->b);
            } else if (modo == MODO_RELLENO) {
                rellenar_poligono_recorte(fb, dibujo, n, ventana, vista, pol->r, pol->g, pol->b, NULL, 0);
                if (bordes_negros)
                    dibujar_contorno_poligono(fb, dibujo, n, ventana, vista, 0, 0, 0);
            } else {
                const Texture *tex = textura_respaldo;
                if (cantidad_texturas > 0 && banco) {
                    if (modo == MODO_TEXTURAS_VARIAS)
                        tex = &banco->texturas[pol->indice_textura % cantidad_texturas];
                    else
                        tex = &banco->texturas[indice_textura_unica % cantidad_texturas];
                }
                rellenar_poligono_recorte(fb, dibujo, n, ventana, vista, 255, 255, 255, tex, 1);
                if (bordes_negros)
                    dibujar_contorno_poligono(fb, dibujo, n, ventana, vista, 0, 0, 0);
            }
        } else {
            if (bordes_negros)
                dibujar_polilinea(fb, dibujo, n, ventana, vista, 0, 0, 0);
            else
                dibujar_polilinea(fb, dibujo, n, ventana, vista, pol->r, pol->g, pol->b);
        }
    }
}

int main(int argc, char *argv[]) {
    int solo_diagnosticos = 0;
    int bordes_negros = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0)
            solo_diagnosticos = 1;
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bordes") == 0)
            bordes_negros = 1;
        else {
            fprintf(stderr, "Opción desconocida: %s\n", argv[i]);
            fprintf(stderr, "Uso: %s [-d] [-b|--bordes]\n", argv[0]);
            fprintf(stderr, "  -d            imprime datos de la escena y termina\n");
            fprintf(stderr, "  -b, --bordes  trazo negro ~1 px en aristas (todos los modos)\n");
            return 1;
        }
    }
    if (solo_diagnosticos) {
        imprimir_diagnosticos_escena();
        return 0;
    }

    if (bordes_negros)
        fprintf(stderr, "Bordes negros activados (-b).\n");

    srand((unsigned int)time(NULL));

    const int ancho = 1000;
    const int alto = 500;

    Framebuffer fb;
    if (crear_framebuffer(&fb, ancho, alto) != 0) {
        fprintf(stderr, "No se pudo reservar el framebuffer.\n");
        return 1;
    }

    Texture textura_tablero;
    if (textura_tablero_cuadros(&textura_tablero, 256, 256) != 0) {
        liberar_framebuffer(&fb);
        fprintf(stderr, "No se pudo generar la textura de respaldo.\n");
        return 1;
    }

    BancoTexturas banco;
    memset(&banco, 0, sizeof banco);
    int cantidad_texturas = cargar_texturas_desde_carpeta(&banco, "textures");
    if (cantidad_texturas == 0)
        fprintf(stderr, "Aviso: carpeta textures/ vacía o sin BMP; modos 3 y 4 usan el tablero.\n");
    else
        fprintf(stderr, "Texturas cargadas: %d\n", cantidad_texturas);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        vaciar_banco_texturas(&banco);
        destruir_textura(&textura_tablero);
        liberar_framebuffer(&fb);
        return 1;
    }

    SDL_Window *ventana_sdl = SDL_CreateWindow(
        "Proyecto 2 — 1 alambre 2 relleno 3 texturas 4 una textura | +/- zoom flechas pan [] rotar | Inicio/R "
        "reinicio | Q salir",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ancho, alto, SDL_WINDOW_SHOWN);
    if (!ventana_sdl) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        vaciar_banco_texturas(&banco);
        destruir_textura(&textura_tablero);
        liberar_framebuffer(&fb);
        return 1;
    }

    SDL_Renderer *render = SDL_CreateRenderer(ventana_sdl, -1, SDL_RENDERER_ACCELERATED);
    if (!render) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana_sdl);
        SDL_Quit();
        vaciar_banco_texturas(&banco);
        destruir_textura(&textura_tablero);
        liberar_framebuffer(&fb);
        return 1;
    }

    SDL_Texture *tex_sdl =
        SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, ancho, alto);
    if (!tex_sdl) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(ventana_sdl);
        SDL_Quit();
        vaciar_banco_texturas(&banco);
        destruir_textura(&textura_tablero);
        liberar_framebuffer(&fb);
        return 1;
    }

    VentanaMundo ventana = ventana_inicial;
    RectVista vista = {0, ancho - 1, 0, alto - 1};

    construir_escena();

    int modo = MODO_RELLENO;
    int indice_textura_unica = 0;
    double angulo_vista_rad = 0.0;
    int ejecutando = 1;

    // ~60 FPS; todo el pipeline es CPU (no OpenGL de escena).
    while (ejecutando) {
        limpiar_framebuffer(&fb, 40, 44, 52);
        dibujar_escena(&fb, &ventana, &vista, modo, &banco, cantidad_texturas, indice_textura_unica,
                       &textura_tablero, angulo_vista_rad, bordes_negros);
        actualizar_pantalla(&fb, render, tex_sdl);

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                ejecutando = 0;

            // Rueda: mismo centro de ventana mundo, se encoge o agranda el rectángulo visible.
            if (ev.type == SDL_MOUSEWHEEL) {
                int rapido = (SDL_GetModState() & KMOD_SHIFT) != 0;
                double z = rapido ? 0.88 : 0.95;
                if (ev.wheel.y > 0)
                    acercar_o_alejar(&ventana, z);
                else if (ev.wheel.y < 0)
                    acercar_o_alejar(&ventana, 1.0 / z);
            }

            if (ev.type == SDL_KEYDOWN) {
                int rapido = (ev.key.keysym.mod & KMOD_SHIFT) != 0;
                double ancho_m = ventana.wxmax - ventana.wxmin;
                double alto_m = ventana.wymax - ventana.wymin;
                double fraccion_pan = rapido ? 0.06 : 0.02;
                double paso_x = ancho_m * fraccion_pan;
                double paso_y = alto_m * fraccion_pan;
                double z = rapido ? 0.88 : 0.95;
                double paso_rot = rapido ? (30.0 * M_PI / 180.0) : (15.0 * M_PI / 180.0);

                switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    ejecutando = 0;
                    break;
                case SDLK_1:
                    modo = MODO_ALAMBRE;
                    break;
                case SDLK_2:
                    modo = MODO_RELLENO;
                    break;
                case SDLK_3:
                    modo = MODO_TEXTURAS_VARIAS;
                    if (cantidad_texturas > 0) {
                        asignar_texturas_aleatorias(cantidad_texturas);
                        actualizar_mapa_uv_local();
                    } else {
                        actualizar_mapa_uv_global();
                    }
                    break;
                case SDLK_4:
                    modo = MODO_UNA_TEXTURA;
                    if (cantidad_texturas > 0) {
                        indice_textura_unica = rand() % cantidad_texturas;
                        actualizar_mapa_uv_global();
                    } else {
                        actualizar_mapa_uv_global();
                    }
                    break;

                case SDLK_EQUALS:
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    acercar_o_alejar(&ventana, z);
                    break;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    acercar_o_alejar(&ventana, 1.0 / z);
                    break;

                /* Pan en ejes de mundo = ejes de pantalla; angulo_vista_rad solo rota la geometría. */
                case SDLK_LEFT:
                    desplazar_ventana(&ventana, -paso_x, 0.0);
                    break;
                case SDLK_RIGHT:
                    desplazar_ventana(&ventana, paso_x, 0.0);
                    break;
                case SDLK_UP:
                    desplazar_ventana(&ventana, 0.0, -paso_y);
                    break;
                case SDLK_DOWN:
                    desplazar_ventana(&ventana, 0.0, paso_y);
                    break;

                case SDLK_LEFTBRACKET:
                    angulo_vista_rad = normalizar_angulo_vista(angulo_vista_rad - paso_rot);
                    break;
                case SDLK_RIGHTBRACKET:
                    angulo_vista_rad = normalizar_angulo_vista(angulo_vista_rad + paso_rot);
                    break;

                case SDLK_HOME:
                case SDLK_r:
                    ventana = ventana_inicial;
                    angulo_vista_rad = 0.0;
                    break;

                default:
                    break;
                }
            }
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tex_sdl);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(ventana_sdl);
    SDL_Quit();
    vaciar_banco_texturas(&banco);
    destruir_textura(&textura_tablero);
    liberar_framebuffer(&fb);
    return 0;
}
