/* Relleno: SH en mundo → coords enteras → scan-line (ET/AET). Alámbrico: CS + Bresenham por arista. */
#include "rasterizer.h"
#include "lineas.h"
#include <math.h>
#include <stdio.h>

static int n_avisos_acotar_x;
static int n_avisos_filas_scanline;
static int n_avisos_aet_llena;
static int n_avisos_recorte;
static int n_avisos_scanline;

typedef struct {
    int y_bot;
    int y_top;
    double xa, ya, ua, va;
    double dx_dy, du_dy, dv_dy;
} AristaOrden;

typedef struct {
    AristaOrden d;
    double x, u, v;
} AristaActiva;

/* Ignora aristas horizontales en espacio píxel (dy~0); no aportan al relleno por filas. */
static int preparar_aristas(const Vertice *v, int n, AristaOrden *out, int max_out) {
    int ne = 0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        double x0 = v[i].x, y0 = v[i].y;
        double x1 = v[j].x, y1 = v[j].y;
        double u0 = v[i].u, v0 = v[i].v;
        double u1 = v[j].u, v1 = v[j].v;

        double xa, ya, ua, va, xb, yb, ub, vb;
        if (y0 < y1) {
            xa = x0;
            ya = y0;
            ua = u0;
            va = v0;
            xb = x1;
            yb = y1;
            ub = u1;
            vb = v1;
        } else if (y1 < y0) {
            xa = x1;
            ya = y1;
            ua = u1;
            va = v1;
            xb = x0;
            yb = y0;
            ub = u0;
            vb = v0;
        } else {
            continue;
        }

        double dy = yb - ya;
        if (dy < 1e-9)
            continue;

        if (ne >= max_out)
            return -1;

        AristaOrden *e = &out[ne];
        e->dx_dy = (xb - xa) / dy;
        e->du_dy = (ub - ua) / dy;
        e->dv_dy = (vb - va) / dy;
        e->xa = xa;
        e->ya = ya;
        e->ua = ua;
        e->va = va;
        e->y_bot = (int)floor(ya + 0.5);
        e->y_top = (int)floor(yb + 0.5);
        if (e->y_top <= e->y_bot)
            continue;
        ne++;
    }
    return ne;
}

static void ordenar_por_x(AristaActiva *a, int n) {
    for (int i = 1; i < n; i++) {
        AristaActiva key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j].x > key.x) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

static void pintar_franja(Framebuffer *fb, int y, double x0, double u0, double v0, double x1, double u1,
                          double v1, unsigned char cr, unsigned char cg, unsigned char cb, const Texture *tex,
                          int con_textura) {
    if (!fb || y < 0 || y >= fb->height)
        return;

    if (x0 > x1) {
        double tx = x0;
        x0 = x1;
        x1 = tx;
        double tu = u0;
        u0 = u1;
        u1 = tu;
        double tv = v0;
        v0 = v1;
        v1 = tv;
    }

    int W = fb->width;
    double lim = (double)W * 6.0 + 512.0;
    if (x0 < -lim || x0 > lim || x1 < -lim || x1 > lim) {
        if (n_avisos_acotar_x < 10) {
            fprintf(stderr, "raster: span horizontal acotado (fila y=%d)\n", y);
            n_avisos_acotar_x++;
        }
        if (x0 < -lim) x0 = -lim;
        if (x0 > lim) x0 = lim;
        if (x1 < -lim) x1 = -lim;
        if (x1 > lim) x1 = lim;
    }

    int xs = (int)ceil(x0 - 0.5);
    int xe = (int)floor(x1 + 0.5);
    if (xs < 0) xs = 0;
    if (xe >= W) xe = W - 1;
    if (xs > xe)
        return;

    double denom = x1 - x0;
    for (int x = xs; x <= xe; x++) {
        unsigned char r, g, b;
        if (con_textura && tex && tex->rgb) {
            double t = denom > 1e-9 ? ((double)x - x0) / denom : 0.0;
            double u = u0 + t * (u1 - u0);
            double v = v0 + t * (v1 - v0);
            muestrear_textura(tex, u, v, &r, &g, &b);
        } else {
            r = cr;
            g = cg;
            b = cb;
        }
        pintar_pixel(fb, x, y, r, g, b);
    }
}

static int barrido_scanline(Framebuffer *fb, const Vertice *verts_pix, int n, unsigned char cr,
                            unsigned char cg, unsigned char cb, const Texture *tex, int con_textura) {
    enum { MAX_ARISTAS = 256, MAX_AET = 256 };
    AristaOrden pool[MAX_ARISTAS];
    int ne = preparar_aristas(verts_pix, n, pool, MAX_ARISTAS);
    if (ne <= 0)
        return 0;

    int gmin = pool[0].y_bot;
    int gmax = pool[0].y_top;
    for (int i = 1; i < ne; i++) {
        if (pool[i].y_bot < gmin) gmin = pool[i].y_bot;
        if (pool[i].y_top > gmax) gmax = pool[i].y_top;
    }

    AristaActiva aet[MAX_AET];
    int naet = 0;

    const int max_filas = fb->height * 12 + 4000;
    int filas = 0;

    for (int y = gmin; y < gmax; y++) {
        if (++filas > max_filas) {
            if (n_avisos_filas_scanline < 10) {
                fprintf(stderr, "raster: demasiadas filas en scanline (y %d..%d)\n", gmin, gmax);
                n_avisos_filas_scanline++;
            }
            return -1;
        }

        int w = 0;
        for (int i = 0; i < naet; i++) {
            if (aet[i].d.y_top > y)
                aet[w++] = aet[i];
        }
        naet = w;

        for (int i = 0; i < ne; i++) {
            if (pool[i].y_bot != y || pool[i].y_top <= pool[i].y_bot)
                continue;
            if (naet >= MAX_AET) {
                if (n_avisos_aet_llena < 10)
                    fprintf(stderr, "raster: aristas activas llenas (%d)\n", MAX_AET);
                n_avisos_aet_llena++;
                return -1;
            }
            AristaActiva *e = &aet[naet++];
            e->d = pool[i];
            double yy = (double)y;
            e->x = e->d.xa + (yy - e->d.ya) * e->d.dx_dy;
            e->u = e->d.ua + (yy - e->d.ya) * e->d.du_dy;
            e->v = e->d.va + (yy - e->d.ya) * e->d.dv_dy;
        }

        ordenar_por_x(aet, naet);

        for (int k = 0; k + 1 < naet; k += 2) {
            pintar_franja(fb, y, aet[k].x, aet[k].u, aet[k].v, aet[k + 1].x, aet[k + 1].u, aet[k + 1].v, cr,
                          cg, cb, tex, con_textura);
        }

        for (int i = 0; i < naet; i++) {
            aet[i].x += aet[i].d.dx_dy;
            aet[i].u += aet[i].d.du_dy;
            aet[i].v += aet[i].d.dv_dy;
        }
    }
    return 0;
}

/* con_textura: interpola u,v en los spans; si tex es NULL el raster usa color plano aunque el flag sea 1. */
void rellenar_poligono_recorte(Framebuffer *fb, const Vertice *poligono_mundo, int n,
                               const VentanaMundo *ventana, const RectVista *vista, unsigned char r,
                               unsigned char g, unsigned char b, const Texture *tex, int con_textura) {
    Vertice recortado[MAX_VERTICES_RECORTE];
    int n_rec = recortar_poligono_sh(poligono_mundo, n, recortado, MAX_VERTICES_RECORTE, ventana);
    if (n_rec < 0) {
        if (n_avisos_recorte < 10) {
            fprintf(stderr, "raster: recorte SH falló (buffer o entrada)\n");
            n_avisos_recorte++;
        }
        return;
    }
    if (n_rec < 3)
        return;

    Vertice en_pixeles[MAX_VERTICES_RECORTE];
    for (int i = 0; i < n_rec; i++) {
        int xi, yi;
        mundo_a_rectvista(recortado[i].x, recortado[i].y, ventana, vista, &xi, &yi);
        en_pixeles[i].x = (double)xi;
        en_pixeles[i].y = (double)yi;
        en_pixeles[i].u = recortado[i].u;
        en_pixeles[i].v = recortado[i].v;
    }

    if (barrido_scanline(fb, en_pixeles, n_rec, r, g, b, tex, con_textura) != 0) {
        if (n_avisos_scanline < 10) {
            fprintf(stderr, "raster: scanline abortó un polígono\n");
            n_avisos_scanline++;
        }
    }
}

static Framebuffer *fb_alambrico;
static unsigned char rojo_al, verde_al, azul_al;

static void poner_pixel_alambrico(int x, int y) {
    pintar_pixel(fb_alambrico, x, y, rojo_al, verde_al, azul_al);
}

void dibujar_contorno_poligono(Framebuffer *fb, const Vertice *poligono_mundo, int n,
                               const VentanaMundo *ventana, const RectVista *vista, unsigned char r,
                               unsigned char g, unsigned char b) {
    fb_alambrico = fb;
    rojo_al = r;
    verde_al = g;
    azul_al = b;

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        Vertice a = poligono_mundo[i];
        Vertice b = poligono_mundo[j];
        if (!recortar_arista_cs(&a, &b, ventana))
            continue;
        int ax, ay, bx, by;
        mundo_a_rectvista(a.x, a.y, ventana, vista, &ax, &ay);
        mundo_a_rectvista(b.x, b.y, ventana, vista, &bx, &by);
        Linea ln = {ax, ay, bx, by};
        bresenham(ln, poner_pixel_alambrico);
    }
}

void dibujar_polilinea(Framebuffer *fb, const Vertice *puntos_mundo, int n, const VentanaMundo *ventana,
                       const RectVista *vista, unsigned char r, unsigned char g, unsigned char b) {
    if (n < 2)
        return;
    fb_alambrico = fb;
    rojo_al = r;
    verde_al = g;
    azul_al = b;

    for (int i = 0; i < n - 1; i++) {
        Vertice a = puntos_mundo[i];
        Vertice b = puntos_mundo[i + 1];
        if (!recortar_arista_cs(&a, &b, ventana))
            continue;
        int ax, ay, bx, by;
        mundo_a_rectvista(a.x, a.y, ventana, vista, &ax, &ay);
        mundo_a_rectvista(b.x, b.y, ventana, vista, &bx, &by);
        Linea ln = {ax, ay, bx, by};
        bresenham(ln, poner_pixel_alambrico);
    }
}
