#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct Texture {
    int width;
    int height;
    unsigned char *rgb;
} Texture;

void destruir_textura(Texture *t);
int textura_tablero_cuadros(Texture *t, int ancho, int alto);
int cargar_bmp(Texture *t, const char *ruta);
/* u,v en [0,1]; mapeo pizarra round(u·ancho), round(v·alto). La interpolación lineal de (u,v) está en rasterizer.c. */
void muestrear_textura(const Texture *t, double u, double v, unsigned char *r, unsigned char *g,
                       unsigned char *b);

#endif
