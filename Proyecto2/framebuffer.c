/* RGB24 interleaved, fila mayor = y creciente (igual que SDL para este demo). */
#include "framebuffer.h"
#include <stdlib.h>
#include <string.h>

int crear_framebuffer(Framebuffer *fb, int ancho, int alto) {
    if (!fb || ancho <= 0 || alto <= 0)
        return -1;
    fb->width = ancho;
    fb->height = alto;
    size_t n = (size_t)ancho * (size_t)alto * 3u;
    fb->pixels = (unsigned char *)malloc(n);
    if (!fb->pixels)
        return -1;
    memset(fb->pixels, 0, n);
    return 0;
}

void liberar_framebuffer(Framebuffer *fb) {
    if (!fb)
        return;
    free(fb->pixels);
    fb->pixels = NULL;
    fb->width = fb->height = 0;
}

void limpiar_framebuffer(Framebuffer *fb, unsigned char r, unsigned char g, unsigned char b) {
    if (!fb || !fb->pixels)
        return;
    for (int y = 0; y < fb->height; y++) {
        for (int x = 0; x < fb->width; x++) {
            size_t i = ((size_t)y * (size_t)fb->width + (size_t)x) * 3u;
            fb->pixels[i] = r;
            fb->pixels[i + 1] = g;
            fb->pixels[i + 2] = b;
        }
    }
}

void pintar_pixel(Framebuffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (!fb || !fb->pixels)
        return;
    if (x < 0 || x >= fb->width || y < 0 || y >= fb->height)
        return;
    size_t i = ((size_t)y * (size_t)fb->width + (size_t)x) * 3u;
    fb->pixels[i] = r;
    fb->pixels[i + 1] = g;
    fb->pixels[i + 2] = b;
}
