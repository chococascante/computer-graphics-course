#include "sdl_vista.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>

struct VistaSDL {
    SDL_Window *ventana;
    SDL_Surface *superficie;
    SDL_PixelFormat *formato;
};

int sdl_disponible(void) { return 1; }

VistaSDL *sdl_iniciar(int ancho, int alto, const char *titulo) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init falló: %s\n", SDL_GetError());
        return NULL;
    }
    VistaSDL *v = (VistaSDL *)calloc(1, sizeof(*v));
    if (!v) { SDL_Quit(); return NULL; }
    v->ventana = SDL_CreateWindow(titulo,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  ancho, alto, SDL_WINDOW_SHOWN);
    if (!v->ventana) {
        fprintf(stderr, "SDL_CreateWindow falló: %s\n", SDL_GetError());
        free(v); SDL_Quit(); return NULL;
    }
    v->superficie = SDL_GetWindowSurface(v->ventana);
    if (!v->superficie) {
        fprintf(stderr, "SDL_GetWindowSurface falló: %s\n", SDL_GetError());
        SDL_DestroyWindow(v->ventana); free(v); SDL_Quit(); return NULL;
    }
    v->formato = v->superficie->format;
    return v;
}

void sdl_pintar_pixel(VistaSDL *v, int i, int j,
                      unsigned char r, unsigned char g, unsigned char b) {
    if (!v || !v->superficie) return;
    if (SDL_MUSTLOCK(v->superficie)) SDL_LockSurface(v->superficie);
    Uint8 *fila = (Uint8 *)v->superficie->pixels + j * v->superficie->pitch;
    Uint32 empacado = SDL_MapRGB(v->formato, r, g, b);
    Uint32 *fila32 = (Uint32 *)fila;
    fila32[i] = empacado;
    if (SDL_MUSTLOCK(v->superficie)) SDL_UnlockSurface(v->superficie);
}

void sdl_actualizar(VistaSDL *v) {
    if (v && v->ventana) SDL_UpdateWindowSurface(v->ventana);
}

int sdl_revisar_salida(VistaSDL *v) {
    (void)v;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) return 1;
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) return 1;
    }
    return 0;
}

void sdl_esperar_cierre(VistaSDL *v) {
    while (!sdl_revisar_salida(v)) SDL_Delay(16);
}

void sdl_finalizar(VistaSDL *v) {
    if (!v) return;
    if (v->ventana) SDL_DestroyWindow(v->ventana);
    SDL_Quit();
    free(v);
}

#else /* sin SDL2 */

struct VistaSDL { int dummy; };

int      sdl_disponible(void)                 { return 0; }
VistaSDL *sdl_iniciar(int a, int al, const char *t) { (void)a; (void)al; (void)t; return NULL; }
void     sdl_pintar_pixel(VistaSDL *v, int i, int j,
                          unsigned char r, unsigned char g, unsigned char b) {
    (void)v; (void)i; (void)j; (void)r; (void)g; (void)b;
}
void     sdl_actualizar(VistaSDL *v)          { (void)v; }
int      sdl_revisar_salida(VistaSDL *v)      { (void)v; return 0; }
void     sdl_esperar_cierre(VistaSDL *v)      { (void)v; }
void     sdl_finalizar(VistaSDL *v)           { (void)v; }

#endif
