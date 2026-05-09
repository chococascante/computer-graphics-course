#include "ppm.h"
#include <stdio.h>

unsigned char a_byte(float v) {
    if (v <= 0.0f) return 0;
    if (v >= 1.0f) return 255;
    return (unsigned char)(v * 255.0f + 0.5f);
}

int escribir_ppm(const char *ruta, int ancho, int alto, const unsigned char *pixeles) {
    FILE *f = fopen(ruta, "wb");
    if (!f) { perror(ruta); return 0; }
    if (fprintf(f, "P6\n%d %d\n255\n", ancho, alto) < 0) { fclose(f); return 0; }
    size_t n = (size_t)ancho * (size_t)alto * 3u;
    int ok = (fwrite(pixeles, 1, n, f) == n);
    fclose(f);
    return ok;
}
