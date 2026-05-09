/*
 * ppm.h — Salida a archivo PPM P6 (sin pérdida).
 */
#ifndef PPM_H
#define PPM_H

unsigned char a_byte(float v);
int escribir_ppm(const char *ruta, int ancho, int alto, const unsigned char *pixeles);

#endif
