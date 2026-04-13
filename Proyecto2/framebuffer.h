#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stddef.h>

typedef struct Framebuffer {
    int width;
    int height;
    unsigned char *pixels; /* RGB fila a fila */
} Framebuffer;

int crear_framebuffer(Framebuffer *fb, int ancho, int alto);
void liberar_framebuffer(Framebuffer *fb);
void limpiar_framebuffer(Framebuffer *fb, unsigned char r, unsigned char g, unsigned char b);
void pintar_pixel(Framebuffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b);

#endif
