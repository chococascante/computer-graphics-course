#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "clipping.h"
#include "framebuffer.h"
#include "texture.h"
#include "transform.h"

void rellenar_poligono_recorte(Framebuffer *fb, const Vertice *poligono_mundo, int n,
                               const VentanaMundo *ventana, const RectVista *vista, unsigned char r,
                               unsigned char g, unsigned char b, const Texture *tex, int con_textura);

void dibujar_contorno_poligono(Framebuffer *fb, const Vertice *poligono_mundo, int n,
                               const VentanaMundo *ventana, const RectVista *vista, unsigned char r,
                               unsigned char g, unsigned char b);

void dibujar_polilinea(Framebuffer *fb, const Vertice *puntos_mundo, int n,
                       const VentanaMundo *ventana, const RectVista *vista, unsigned char r,
                       unsigned char g, unsigned char b);

#endif
