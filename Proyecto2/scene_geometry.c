/* Geometría en mundo ~[0,1000]x[0,500]: escala el SVG del Mustang y rellena buf_vertices.
 * El orden de MUSTANG_POLYGONS es el de pintado (fondo primero); construir_escena lo conserva. */
#include "scene_geometry.h"
#include "mustang_polygons.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    MAX_POLIGONOS = MUSTANG_POLYGON_COUNT,
    MAX_VERTICES_POLIGONO = 128
};

static PoligonoEscena escena[MAX_POLIGONOS];
static int n_poligonos;
static int escena_lista;

static Vertice buf_vertices[MAX_POLIGONOS][MAX_VERTICES_POLIGONO];

static double x_min_svg, x_max_svg, y_min_svg, y_max_svg;
static double escala, desp_x, desp_y;

static void ajustar_caja_y_escala(void) {
    x_min_svg = y_min_svg = 1e30;
    x_max_svg = y_max_svg = -1e30;
    for (int i = 0; i < MUSTANG_POLYGON_COUNT; i++) {
        const MustangPolygon *p = &MUSTANG_POLYGONS[i];
        for (int j = 0; j < p->num_points; j++) {
            double x = (double)p->points[j].x;
            double y = (double)p->points[j].y;
            if (x < x_min_svg) x_min_svg = x;
            if (x > x_max_svg) x_max_svg = x;
            if (y < y_min_svg) y_min_svg = y;
            if (y > y_max_svg) y_max_svg = y;
        }
    }
    const double margen = 15.0;
    double ancho = x_max_svg - x_min_svg;
    double alto = y_max_svg - y_min_svg;
    if (ancho < 1e-6) ancho = 1.0;
    if (alto < 1e-6) alto = 1.0;
    double sx = (1000.0 - 2.0 * margen) / ancho;
    double sy = (500.0 - 2.0 * margen) / alto;
    escala = sx < sy ? sx : sy;
    double ancho_m = escala * ancho;
    double alto_m = escala * alto;
    desp_x = margen + (1000.0 - 2.0 * margen - ancho_m) * 0.5 - escala * x_min_svg;
    desp_y = margen + (500.0 - 2.0 * margen - alto_m) * 0.5 - escala * y_min_svg;
}

static void a_mundo(double x_svg, double y_svg, double *mx, double *my) {
    *mx = escala * x_svg + desp_x;
    *my = escala * y_svg + desp_y;
}

void construir_escena(void) {
    if (escena_lista)
        return;

    ajustar_caja_y_escala();

    n_poligonos = 0;
    for (int i = 0; i < MUSTANG_POLYGON_COUNT; i++) {
        const MustangPolygon *p = &MUSTANG_POLYGONS[i];
        if (p->num_points < 3 || p->num_points > MAX_VERTICES_POLIGONO)
            continue;
        for (int j = 0; j < p->num_points; j++) {
            double mx, my;
            a_mundo((double)p->points[j].x, (double)p->points[j].y, &mx, &my);
            buf_vertices[n_poligonos][j].x = mx;
            buf_vertices[n_poligonos][j].y = my;
            buf_vertices[n_poligonos][j].u = 0.0;
            buf_vertices[n_poligonos][j].v = 0.0;
        }
        escena[n_poligonos].puntos = buf_vertices[n_poligonos];
        escena[n_poligonos].num_vertices = p->num_points;
        escena[n_poligonos].r = p->r;
        escena[n_poligonos].g = p->g;
        escena[n_poligonos].b = p->b;
        escena[n_poligonos].cerrado = 1;
        escena[n_poligonos].indice_textura = 0;
        n_poligonos++;
    }

    escena_lista = 1;
}

/* Centro del rectángulo que rodea todo el coche (mundo); main lo usa al rotar con []. */
void centro_geometrico_escena_mundo(double *cx, double *cy) {
    construir_escena();
    if (!cx || !cy)
        return;
    if (n_poligonos <= 0) {
        *cx = 500.0;
        *cy = 250.0;
        return;
    }
    double xmin = 1e30, xmax = -1e30, ymin = 1e30, ymax = -1e30;
    for (int i = 0; i < n_poligonos; i++) {
        for (int j = 0; j < escena[i].num_vertices; j++) {
            double x = escena[i].puntos[j].x;
            double y = escena[i].puntos[j].y;
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
        }
    }
    *cx = 0.5 * (xmin + xmax);
    *cy = 0.5 * (ymin + ymax);
}

int num_poligonos_escena(void) {
    return n_poligonos;
}

const PoligonoEscena *poligono_escena_en(int i) {
    if (i < 0 || i >= n_poligonos)
        return NULL;
    return &escena[i];
}

void asignar_texturas_aleatorias(int num_texturas) {
    construir_escena();
    if (num_texturas <= 0)
        return;
    for (int i = 0; i < n_poligonos; i++)
        escena[i].indice_textura = rand() % num_texturas;
}

/* Una sola caja para todo el Mustang; modo 3 sin BMP sirve para ver el mapeo continuo. */
void actualizar_mapa_uv_global(void) {
    construir_escena();
    double xmin = 1e30, xmax = -1e30, ymin = 1e30, ymax = -1e30;
    for (int i = 0; i < n_poligonos; i++) {
        for (int j = 0; j < escena[i].num_vertices; j++) {
            double x = buf_vertices[i][j].x;
            double y = buf_vertices[i][j].y;
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
        }
    }
    double w = xmax - xmin;
    double h = ymax - ymin;
    if (w < 1e-9) w = 1.0;
    if (h < 1e-9) h = 1.0;
    for (int i = 0; i < n_poligonos; i++) {
        for (int j = 0; j < escena[i].num_vertices; j++) {
            buf_vertices[i][j].u = (buf_vertices[i][j].x - xmin) / w;
            buf_vertices[i][j].v = (buf_vertices[i][j].y - ymin) / h;
        }
    }
}

/* Cada polígono con su propio 0..1 en u,v (útiles con texturas distintas por pieza). */
void actualizar_mapa_uv_local(void) {
    construir_escena();
    for (int i = 0; i < n_poligonos; i++) {
        double xmin = 1e30, xmax = -1e30, ymin = 1e30, ymax = -1e30;
        for (int j = 0; j < escena[i].num_vertices; j++) {
            double x = buf_vertices[i][j].x;
            double y = buf_vertices[i][j].y;
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
        }
        double w = xmax - xmin;
        double h = ymax - ymin;
        if (w < 1e-9) w = 1.0;
        if (h < 1e-9) h = 1.0;
        for (int j = 0; j < escena[i].num_vertices; j++) {
            buf_vertices[i][j].u = (buf_vertices[i][j].x - xmin) / w;
            buf_vertices[i][j].v = (buf_vertices[i][j].y - ymin) / h;
        }
    }
}

void imprimir_diagnosticos_escena(void) {
    construir_escena();
    printf("Polígonos en escena: %d (definidos en cabecera: %d)\n", n_poligonos, MUSTANG_POLYGON_COUNT);
    printf("Caja SVG x[%.1f,%.1f] y[%.1f,%.1f]  escala=%.6f despl=(%.1f,%.1f)\n", x_min_svg, x_max_svg, y_min_svg,
           y_max_svg, escala, desp_x, desp_y);
    for (int i = 0; i < n_poligonos && i < 5; i++) {
        printf("  polígono %d: %d vértices, RGB (%u,%u,%u)\n", i, escena[i].num_vertices, escena[i].r, escena[i].g,
               escena[i].b);
        int lim = escena[i].num_vertices < 4 ? escena[i].num_vertices : 4;
        for (int j = 0; j < lim; j++)
            printf("    (%.3f, %.3f)\n", escena[i].puntos[j].x, escena[i].puntos[j].y);
    }
}
