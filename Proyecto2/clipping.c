/* CS = aristas (alámbrico, contornos). SH = polígonos rellenos, contra el mismo rect VentanaMundo. */
#include "clipping.h"
#include <math.h>

#define CS_LEFT   1
#define CS_RIGHT  2
#define CS_BOTTOM 4
#define CS_TOP    8

static int codigo_cohen(double x, double y, const VentanaMundo *w) {
    int code = 0;
    if (x < w->wxmin) code |= CS_LEFT;
    else if (x > w->wxmax) code |= CS_RIGHT;
    if (y < w->wymin) code |= CS_TOP;
    else if (y > w->wymax) code |= CS_BOTTOM;
    return code;
}

static void interpolar_vertice(const Vertice *a, const Vertice *b, double t, Vertice *out) {
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
    out->u = a->u + t * (b->u - a->u);
    out->v = a->v + t * (b->v - a->v);
}

/* Devuelve 0 si la arista queda fuera o el algoritmo no converge (tope + aristas casi paralelas al borde). */
int recortar_arista_cs(Vertice *a, Vertice *b, const VentanaMundo *ventana) {
    Vertice p0 = *a;
    Vertice p1 = *b;
    int iter = 0;
    for (;;) {
        if (++iter > 128)
            return 0;
        int c0 = codigo_cohen(p0.x, p0.y, ventana);
        int c1 = codigo_cohen(p1.x, p1.y, ventana);
        if ((c0 | c1) == 0) {
            *a = p0;
            *b = p1;
            return 1;
        }
        if ((c0 & c1) != 0)
            return 0;
        int c = c0 ? c0 : c1;
        Vertice inter;
        double t;
        if (c & CS_TOP) {
            double den = p1.y - p0.y;
            if (fabs(den) < 1e-15)
                return 0;
            t = (ventana->wymin - p0.y) / den;
            interpolar_vertice(&p0, &p1, t, &inter);
        } else if (c & CS_BOTTOM) {
            double den = p1.y - p0.y;
            if (fabs(den) < 1e-15)
                return 0;
            t = (ventana->wymax - p0.y) / den;
            interpolar_vertice(&p0, &p1, t, &inter);
        } else if (c & CS_RIGHT) {
            double den = p1.x - p0.x;
            if (fabs(den) < 1e-15)
                return 0;
            t = (ventana->wxmax - p0.x) / den;
            interpolar_vertice(&p0, &p1, t, &inter);
        } else {
            double den = p1.x - p0.x;
            if (fabs(den) < 1e-15)
                return 0;
            t = (ventana->wxmin - p0.x) / den;
            interpolar_vertice(&p0, &p1, t, &inter);
        }
        if (c == c0)
            p0 = inter;
        else
            p1 = inter;
    }
}

static int intersect_left(const Vertice *s, const Vertice *e, Vertice *out, const VentanaMundo *w) {
    double dx = e->x - s->x;
    if (fabs(dx) < 1e-12)
        return 0;
    double t = (w->wxmin - s->x) / dx;
    interpolar_vertice(s, e, t, out);
    return 1;
}

static int intersect_right(const Vertice *s, const Vertice *e, Vertice *out, const VentanaMundo *w) {
    double dx = e->x - s->x;
    if (fabs(dx) < 1e-12)
        return 0;
    double t = (w->wxmax - s->x) / dx;
    interpolar_vertice(s, e, t, out);
    return 1;
}

static int intersect_bottom(const Vertice *s, const Vertice *e, Vertice *out, const VentanaMundo *w) {
    double dy = e->y - s->y;
    if (fabs(dy) < 1e-12)
        return 0;
    double t = (w->wymax - s->y) / dy;
    interpolar_vertice(s, e, t, out);
    return 1;
}

static int intersect_top(const Vertice *s, const Vertice *e, Vertice *out, const VentanaMundo *w) {
    double dy = e->y - s->y;
    if (fabs(dy) < 1e-12)
        return 0;
    double t = (w->wymin - s->y) / dy;
    interpolar_vertice(s, e, t, out);
    return 1;
}

typedef int (*DentroFn)(const Vertice *p, const VentanaMundo *w);
typedef int (*CruceFn)(const Vertice *s, const Vertice *e, Vertice *out, const VentanaMundo *w);

static int inside_left(const Vertice *p, const VentanaMundo *w) {
    return p->x >= w->wxmin;
}
static int inside_right(const Vertice *p, const VentanaMundo *w) {
    return p->x <= w->wxmax;
}
static int inside_bottom(const Vertice *p, const VentanaMundo *w) {
    return p->y <= w->wymax;
}
static int inside_top(const Vertice *p, const VentanaMundo *w) {
    return p->y >= w->wymin;
}

static int pasada_sh(const Vertice *in, int n_in, Vertice *out, int max_out, DentroFn dentro,
                     CruceFn cruce, const VentanaMundo *w) {
    if (n_in < 2)
        return 0;
    int n_out = 0;
    Vertice s = in[n_in - 1];
    for (int i = 0; i < n_in; i++) {
        Vertice e = in[i];
        int Sin = dentro(&s, w);
        int Ein = dentro(&e, w);
        Vertice inter;
        if (Sin && Ein) {
            if (n_out >= max_out) return -1;
            out[n_out++] = e;
        } else if (Sin && !Ein) {
            if (cruce(&s, &e, &inter, w)) {
                if (n_out >= max_out) return -1;
                out[n_out++] = inter;
            }
        } else if (!Sin && Ein) {
            if (!cruce(&s, &e, &inter, w))
                inter = e;
            if (n_out >= max_out) return -1;
            out[n_out++] = inter;
            if (n_out >= max_out) return -1;
            out[n_out++] = e;
        }
        s = e;
    }
    return n_out;
}

/* Cuatro cortes: izq, der, abajo (y grande), arriba (y pequeña) — nombres “top/bottom” al revés del canvas. */
int recortar_poligono_sh(const Vertice *entrada, int n_entrada, Vertice *salida, int max_salida,
                         const VentanaMundo *ventana) {
    if (n_entrada < 3 || max_salida < 3)
        return -1;
    Vertice buf_a[MAX_VERTICES_RECORTE];
    Vertice buf_b[MAX_VERTICES_RECORTE];
    if (n_entrada > MAX_VERTICES_RECORTE)
        return -1;
    for (int i = 0; i < n_entrada; i++)
        buf_a[i] = entrada[i];

    int n = n_entrada;
    n = pasada_sh(buf_a, n, buf_b, MAX_VERTICES_RECORTE, inside_left, intersect_left, ventana);
    if (n < 0) return -1;

    n = pasada_sh(buf_b, n, buf_a, MAX_VERTICES_RECORTE, inside_right, intersect_right, ventana);
    if (n < 0) return -1;

    n = pasada_sh(buf_a, n, buf_b, MAX_VERTICES_RECORTE, inside_bottom, intersect_bottom, ventana);
    if (n < 0) return -1;

    n = pasada_sh(buf_b, n, salida, max_salida, inside_top, intersect_top, ventana);
    if (n < 0) return -1;
    return n;
}
