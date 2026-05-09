#define _DEFAULT_SOURCE
#include "escena.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

static int empieza_con(const char *s, const char *clave) {
    size_t n = strlen(clave);
    if (strncmp(s, clave, n) != 0) return 0;
    char sig = s[n];
    return (sig == ' ' || sig == '\t' || sig == '\0' || sig == '\n' || sig == '\r');
}

int cargar_escena(const char *ruta, Escena *esc) {
    memset(esc, 0, sizeof(*esc));
    esc->ancho = 1024; esc->alto = 768;
    snprintf(esc->ruta_salida, sizeof(esc->ruta_salida), "out.ppm");
    esc->ojo = v3(0, 0, 0);
    esc->fov_grados = 55.0f;
    esc->Ia    = v3(0.10f, 0.10f, 0.12f);
    esc->fondo = v3(0.06f, 0.07f, 0.10f);
    esc->modo = MODO_AMBOS;
    esc->sombreado = SOMBREADO_PHONG;

    FILE *f = fopen(ruta, "r");
    if (!f) {
        fprintf(stderr, "No se pudo abrir: %s\n", ruta);
        return 0;
    }

    char linea[1024];
    while (fgets(linea, sizeof linea, f)) {
        char *p = linea;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\r' || *p == '\0') continue;

        if (empieza_con(p, "RES")) {
            int w, h;
            if (sscanf(p + 3, "%d %d", &w, &h) == 2 &&
                w > 0 && h > 0 && w <= 8192 && h <= 8192) {
                esc->ancho = w; esc->alto = h;
            }
        } else if (empieza_con(p, "OUT")) {
            char buf[512];
            if (sscanf(p + 3, "%511s", buf) == 1)
                snprintf(esc->ruta_salida, sizeof(esc->ruta_salida), "%s", buf);
        } else if (empieza_con(p, "MODE")) {
            char buf[16];
            if (sscanf(p + 4, "%15s", buf) == 1) {
                if      (!strcasecmp(buf, "FILE"))   esc->modo = MODO_ARCHIVO;
                else if (!strcasecmp(buf, "WINDOW")) esc->modo = MODO_VENTANA;
                else if (!strcasecmp(buf, "BOTH"))   esc->modo = MODO_AMBOS;
            }
        } else if (empieza_con(p, "SHADING")) {
            char buf[16];
            if (sscanf(p + 7, "%15s", buf) == 1) {
                if      (!strcasecmp(buf, "FLAT"))  esc->sombreado = SOMBREADO_PLANO;
                else if (!strcasecmp(buf, "PHONG")) esc->sombreado = SOMBREADO_PHONG;
            }
        } else if (empieza_con(p, "AMBIENT")) {
            float r, g, b;
            if (sscanf(p + 7, "%f %f %f", &r, &g, &b) == 3) esc->Ia = v3(r, g, b);
        } else if (empieza_con(p, "BG")) {
            float r, g, b;
            if (sscanf(p + 2, "%f %f %f", &r, &g, &b) == 3) esc->fondo = v3(r, g, b);
        } else if (empieza_con(p, "CAM")) {
            float ex, ey, ez, fov;
            if (sscanf(p + 3, "%f %f %f %f", &ex, &ey, &ez, &fov) == 4) {
                esc->ojo = v3(ex, ey, ez);
                esc->fov_grados = fov;
            }
        } else if (empieza_con(p, "LIGHT")) {
            if (esc->num_luces >= MAX_LUCES) continue;
            float px, py, pz, ir, ig, ib;
            if (sscanf(p + 5, "%f %f %f %f %f %f",
                       &px, &py, &pz, &ir, &ig, &ib) == 6) {
                esc->luces[esc->num_luces].posicion   = v3(px, py, pz);
                esc->luces[esc->num_luces].intensidad = v3(ir, ig, ib);
                esc->num_luces++;
            }
        } else if (empieza_con(p, "SPHERE")) {
            if (esc->num_esferas >= MAX_ESFERAS) continue;
            float cx, cy, cz, rad, kr, kg, kb, ks, nn;
            if (sscanf(p + 6, "%f %f %f %f %f %f %f %f %f",
                       &cx, &cy, &cz, &rad, &kr, &kg, &kb, &ks, &nn) == 9
                && rad > 0) {
                Esfera *E = &esc->esferas[esc->num_esferas];
                E->centro = v3(cx, cy, cz);
                E->radio  = rad;
                E->Kd     = v3(kr, kg, kb);
                E->Ks     = ks;
                E->n      = fmaxf(1.0f, nn);
                esc->num_esferas++;
            }
        }
    }
    fclose(f);

    if (esc->num_esferas < 1) {
        fprintf(stderr, "Escena inválida: necesitas al menos 1 esfera.\n");
        return 0;
    }
    if (esc->sombreado == SOMBREADO_PHONG && esc->num_luces < 1) {
        fprintf(stderr, "Escena inválida: SHADING PHONG requiere al menos 1 LIGHT.\n");
        return 0;
    }
    return 1;
}
