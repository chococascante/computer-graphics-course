#include "view_control.h"
#include <math.h>

/* factor < 1 acerca (ventana más chica = más zoom), > 1 aleja. El centro queda fijo. */
void acercar_o_alejar(VentanaMundo *ventana, double factor) {
    if (!ventana || factor <= 0.0)
        return;
    double cx = (ventana->wxmin + ventana->wxmax) * 0.5;
    double cy = (ventana->wymin + ventana->wymax) * 0.5;
    double mitad_ancho = (ventana->wxmax - ventana->wxmin) * 0.5 * factor;
    double mitad_alto = (ventana->wymax - ventana->wymin) * 0.5 * factor;
    if (mitad_ancho < 5.0) mitad_ancho = 5.0;
    if (mitad_alto < 5.0) mitad_alto = 5.0;
    if (mitad_ancho > 1e6) mitad_ancho = 1e6;
    if (mitad_alto > 1e6) mitad_alto = 1e6;
    ventana->wxmin = cx - mitad_ancho;
    ventana->wxmax = cx + mitad_ancho;
    ventana->wymin = cy - mitad_alto;
    ventana->wymax = cy + mitad_alto;
}

void desplazar_ventana(VentanaMundo *ventana, double dx, double dy) {
    if (!ventana)
        return;
    ventana->wxmin += dx;
    ventana->wxmax += dx;
    ventana->wymin += dy;
    ventana->wymax += dy;
}

void rotar_punto(double x, double y, double cx, double cy, double rad, double *ox, double *oy) {
    double c = cos(rad);
    double s = sin(rad);
    double dx = x - cx;
    double dy = y - cy;
    *ox = cx + dx * c - dy * s;
    *oy = cy + dx * s + dy * c;
}

void copiar_poligono_rotado(const Vertice *origen, int n, double cx, double cy, double rad, Vertice *destino) {
    if (!origen || !destino || n < 0)
        return;
    for (int i = 0; i < n; i++) {
        rotar_punto(origen[i].x, origen[i].y, cx, cy, rad, &destino[i].x, &destino[i].y);
        destino[i].u = origen[i].u;
        destino[i].v = origen[i].v;
    }
}
