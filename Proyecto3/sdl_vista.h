/*
 * sdl_vista.h — Encapsula el uso de SDL2 (limitado a crear ventana y
 * pintar píxeles, conforme a la Sección V del enunciado).
 *
 * Si el proyecto se compila sin SDL2 (HAVE_SDL2 no definido), todas las
 * funciones devuelven valores neutros y sdl_disponible() retorna 0.
 */
#ifndef SDL_VISTA_H
#define SDL_VISTA_H

typedef struct VistaSDL VistaSDL;

int      sdl_disponible(void);
VistaSDL *sdl_iniciar(int ancho, int alto, const char *titulo);
void     sdl_pintar_pixel(VistaSDL *v, int i, int j,
                          unsigned char r, unsigned char g, unsigned char b);
void     sdl_actualizar(VistaSDL *v);
int      sdl_revisar_salida(VistaSDL *v);
void     sdl_esperar_cierre(VistaSDL *v);
void     sdl_finalizar(VistaSDL *v);

#endif
