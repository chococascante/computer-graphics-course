/* BMP top-down o bottom-up según cabecera; muestreo de textura: vecino más cercano (u,v acotados a [0,1]). */
#include "texture.h"
#include <stdio.h>
#include <stdlib.h>

void destruir_textura(Texture *t) {
    if (!t)
        return;
    free(t->rgb);
    t->rgb = NULL;
    t->width = t->height = 0;
}

int textura_tablero_cuadros(Texture *t, int ancho, int alto) {
    if (!t || ancho <= 0 || alto <= 0)
        return -1;
    t->width = ancho;
    t->height = alto;
    t->rgb = (unsigned char *)malloc((size_t)ancho * (size_t)alto * 3u);
    if (!t->rgb)
        return -1;
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            int c = ((x / 8) ^ (y / 8)) & 1;
            unsigned char verde_local = c ? 220u : 40u;
            size_t i = ((size_t)y * (size_t)ancho + (size_t)x) * 3u;
            t->rgb[i] = c ? 255u : 60u;
            t->rgb[i + 1] = verde_local;
            t->rgb[i + 2] = c ? 200u : 180u;
        }
    }
    return 0;
}

int cargar_bmp(Texture *t, const char *ruta) {
    if (!t || !ruta)
        return -1;
    destruir_textura(t);

    FILE *f = fopen(ruta, "rb");
    if (!f)
        return -1;

    unsigned char fh[14];
    if (fread(fh, 1, sizeof fh, f) != sizeof fh) {
        fclose(f);
        return -1;
    }
    if (fh[0] != 'B' || fh[1] != 'M') {
        fclose(f);
        return -1;
    }
    unsigned int off = (unsigned int)fh[10] | ((unsigned int)fh[11] << 8u)
        | ((unsigned int)fh[12] << 16u) | ((unsigned int)fh[13] << 24u);

    unsigned char dib[40];
    if (fread(dib, 1, sizeof dib, f) != sizeof dib) {
        fclose(f);
        return -1;
    }
    unsigned int dib_size = (unsigned int)dib[0] | ((unsigned int)dib[1] << 8u)
        | ((unsigned int)dib[2] << 16u) | ((unsigned int)dib[3] << 24u);
    if (dib_size < 40u) {
        fclose(f);
        return -1;
    }

    int width = (int)(dib[4] | (dib[5] << 8) | (dib[6] << 16) | (dib[7] << 24));
    int height_raw = (int)(dib[8] | (dib[9] << 8) | (dib[10] << 16) | (dib[11] << 24));
    unsigned int planes = (unsigned int)dib[12] | ((unsigned int)dib[13] << 8u);
    unsigned int bpp = (unsigned int)dib[14] | ((unsigned int)dib[15] << 8u);
    unsigned int compression = (unsigned int)dib[16] | ((unsigned int)dib[17] << 8u)
        | ((unsigned int)dib[18] << 16u) | ((unsigned int)dib[19] << 24u);

    if (planes != 1u || compression != 0u) {
        fclose(f);
        return -1;
    }
    if (bpp != 24u && bpp != 32u) {
        fclose(f);
        return -1;
    }
    if (width <= 0 || width > 16384)
        width = 0;
    if (!width) {
        fclose(f);
        return -1;
    }

    int top_down = 0;
    int height = height_raw;
    if (height < 0) {
        top_down = 1;
        height = -height;
    }
    if (height <= 0 || height > 16384) {
        fclose(f);
        return -1;
    }

    if (fseek(f, (long)off, SEEK_SET) != 0) {
        fclose(f);
        return -1;
    }

    int stride = (int)(((unsigned int)width * bpp + 31u) / 32u) * 4;
    size_t row_sz = (size_t)stride;
    unsigned char *row = (unsigned char *)malloc(row_sz);
    if (!row) {
        fclose(f);
        return -1;
    }

    size_t pix = (size_t)width * (size_t)height * 3u;
    unsigned char *rgb = (unsigned char *)malloc(pix);
    if (!rgb) {
        free(row);
        fclose(f);
        return -1;
    }

    for (int fr = 0; fr < height; fr++) {
        if (fread(row, 1, row_sz, f) != row_sz) {
            free(row);
            free(rgb);
            fclose(f);
            return -1;
        }
        int dst_y = top_down ? fr : (height - 1 - fr);
        unsigned char *dst = rgb + (size_t)dst_y * (size_t)width * 3u;

        if (bpp == 24u) {
            for (int x = 0; x < width; x++) {
                dst[x * 3 + 0] = row[x * 3 + 2];
                dst[x * 3 + 1] = row[x * 3 + 1];
                dst[x * 3 + 2] = row[x * 3 + 0];
            }
        } else {
            for (int x = 0; x < width; x++) {
                dst[x * 3 + 0] = row[x * 4 + 2];
                dst[x * 3 + 1] = row[x * 4 + 1];
                dst[x * 3 + 2] = row[x * 4 + 0];
            }
        }
    }

    free(row);
    fclose(f);
    t->width = width;
    t->height = height;
    t->rgb = rgb;
    return 0;
}

static double acotar_01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void muestrear_textura(const Texture *t, double u, double v, unsigned char *r, unsigned char *g,
                       unsigned char *b) {
    if (!t || !t->rgb || t->width < 1 || t->height < 1) {
        *r = *g = *b = 0;
        return;
    }
    u = acotar_01(u);
    v = acotar_01(v);
    /* Curso (S1–S7): vecino más cercano, Textura[round(u·ancho), round(v·alto)] con índices en rango. */
    int xi = (int)(u * (double)t->width + 0.5);
    int yi = (int)(v * (double)t->height + 0.5);
    if (xi < 0) xi = 0;
    if (xi >= t->width) xi = t->width - 1;
    if (yi < 0) yi = 0;
    if (yi >= t->height) yi = t->height - 1;

    size_t i = ((size_t)yi * (size_t)t->width + (size_t)xi) * 3u;
    *r = t->rgb[i];
    *g = t->rgb[i + 1u];
    *b = t->rgb[i + 2u];
}
