#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include "lineas.h"

#define CARPETA_PPM "salida"

// Resolución de la imagen
static int g_resolucion;
// Buffer de la imagen
static unsigned char *g_framebuffer;
// Renderer de SDL
static SDL_Renderer *g_renderer;
// Char porque int desperdiciaría 3 bytes
static unsigned char g_color_r = 255, g_color_g = 255, g_color_b = 255;
static int g_ppm3 = 0;

// Función para obtener el tiempo en segundos + microsegundos
static double get_time_sec(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// No hace nada
void plot_vacia(int x, int y) {
    (void)x;
    (void)y;
}

// Plotea un pixel en el buffer de la imagen en formato PPM
void plot_ppm(int x, int y) {
    if (g_framebuffer == NULL || x < 0 || x >= g_resolucion || y < 0 || y >= g_resolucion)
        return;
    int idx = (y * g_resolucion + x) * 3;
    g_framebuffer[idx] = g_color_r;
    g_framebuffer[idx + 1] = g_color_g;
    g_framebuffer[idx + 2] = g_color_b;
}

// Plotea un pixel en la ventana de SDL
void plot_sdl(int x, int y) {
    if (g_renderer == NULL) return;
    SDL_SetRenderDrawColor(g_renderer, g_color_r, g_color_g, g_color_b, 255);
    SDL_RenderDrawPoint(g_renderer, x, y);
}

// Limpia el buffer de la imagen en formato PPM
static void clear_ppm(void) {
    if (g_framebuffer)
        memset(g_framebuffer, 0, (size_t)g_resolucion * g_resolucion * 3);
}

static void write_ppm(const char *filename) {
    FILE *f = fopen(filename, g_ppm3 ? "w" : "wb");
    if (!f) return;
    // Si envío la bandera --ppm3, se usa el formato P3, sino P6
    if (g_ppm3) {
        fprintf(f, "P3\n%d %d\n255\n", g_resolucion, g_resolucion);
        for (int y = 0; y < g_resolucion; y++) {
            for (int x = 0; x < g_resolucion; x++) {
                int idx = (y * g_resolucion + x) * 3;
                fprintf(f, "%u %u %u  ", g_framebuffer[idx], g_framebuffer[idx + 1], g_framebuffer[idx + 2]);
            }
            fprintf(f, "\n");
        }
    } else {
        fprintf(f, "P6\n%d %d\n255\n", g_resolucion, g_resolucion);
        fwrite(g_framebuffer, 1, (size_t)g_resolucion * g_resolucion * 3, f);
    }
    fclose(f);
}

// Espera a que se presione una tecla o se cierre la ventana
static void wait_key_sdl(void) {
    SDL_Event e;
    for (;;) {
        while (SDL_PollEvent(&e)) {
            // Si se cierra la ventana, sale
            if (e.type == SDL_QUIT) exit(0);
            // si se presiona una tecla, continua
            if (e.type == SDL_KEYDOWN) return;
        }
        SDL_Delay(20);
    }
}

// Abstracción de qué algoritmo se está usando
typedef void (*AlgoFunc)(Linea, PlotFunc);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: %s <resolucion> <num_lineas> <num_veces> [--entero] [--ppm3]\n", argv[0]);
        return 1;
    }

    // Lee los argumentos y los convierte a enteros
    int resolucion = atoi(argv[1]);
    int num_lineas = atoi(argv[2]);
    int num_veces = atoi(argv[3]);
    int con_entero = 0;
    for (int f = 4; f < argc; f++) {
        if (strcmp(argv[f], "--entero") == 0) con_entero = 1;
        if (strcmp(argv[f], "--ppm3") == 0) g_ppm3 = 1;
    }

    // Valida que los argumentos sean mayores a 0
    if (resolucion <= 0 || num_lineas <= 0 || num_veces <= 0) {
        printf("Error: Los argumentos deben ser mayores a 0\n");
        return 1;
    }

    // Reserva memoria para las lineas
    Linea *lineas = malloc((size_t)num_lineas * sizeof(Linea));
    if (!lineas) {
        printf("Error: malloc lineas\n");
        return 1;
    }

    // Inicializa el generador de números aleatorios
    srand((unsigned)time(NULL));
    int i = 0;
    // Genera las lineas
    while (i < num_lineas) {
        lineas[i].x0 = rand() % resolucion;
        lineas[i].y0 = rand() % resolucion;
        lineas[i].x1 = rand() % resolucion;
        lineas[i].y1 = rand() % resolucion;
        // Si los puntos no son iguales, incrementa el contador
        if (lineas[i].x0 != lineas[i].x1 || lineas[i].y0 != lineas[i].y1)
            i++;
    }

    // Si envío la bandera --entero, se usan 5 algoritmos, sino 4
    int num_algos = con_entero ? 5 : 4;
    printf("Resolucion: %d, Lineas: %d, Veces: %d\n", resolucion, num_lineas, num_veces);

    // Reserva memoria para los tiempos
    double tiempos[3][5];
    // Reserva memoria para los algoritmos
    AlgoFunc algos[5];
    // Reserva memoria para los nombres
    const char *nombres[5];
    // Reserva memoria para los archivos ppm
    const char *archivos_ppm[5];

    algos[0] = brute_force;
    nombres[0] = "Brute Force";
    archivos_ppm[0] = "brute_force";
    algos[1] = incremental;
    nombres[1] = "Incremental";
    archivos_ppm[1] = "incremental";
    algos[2] = incremental_v2;
    nombres[2] = "Incremental V2";
    archivos_ppm[2] = "incremental_v2";

    // Si envío la bandera --entero, se usan 5 algoritmos, sino 4
    if (con_entero) {
        algos[3] = incremental_v2_entero;
        nombres[3] = "Incremental V2 Entero";
        archivos_ppm[3] = "incremental_v2_entero";
        algos[4] = bresenham;
        nombres[4] = "Bresenham";
        archivos_ppm[4] = "bresenham";
    } else {
        algos[3] = bresenham;
        nombres[3] = "Bresenham";
        archivos_ppm[3] = "bresenham";
    }

    g_resolucion = resolucion;

    /* ----- Versión 1: plot vacía ----- */
    for (int a = 0; a < num_algos; a++) {
        double t0 = get_time_sec();
        for (int rep = 0; rep < num_veces; rep++)
            for (int k = 0; k < num_lineas; k++)
                algos[a](lineas[k], plot_vacia);
        double t1 = get_time_sec();
        tiempos[0][a] = (t1 - t0) / num_veces;
    }

    /* ----- Colores fijos por algoritmo ----- */
    unsigned char colores_algo[5][3];
    colores_algo[0][0] = 255; colores_algo[0][1] =  50; colores_algo[0][2] =  50;  /* rojo */
    colores_algo[1][0] =  50; colores_algo[1][1] = 255; colores_algo[1][2] =  50;  /* verde */
    colores_algo[2][0] =  50; colores_algo[2][1] = 100; colores_algo[2][2] = 255;  /* azul */
    if (con_entero) {
        colores_algo[3][0] = 255; colores_algo[3][1] =  50; colores_algo[3][2] = 255; /* magenta */
        colores_algo[4][0] = 255; colores_algo[4][1] = 255; colores_algo[4][2] =  50; /* amarillo */
    } else {
        colores_algo[3][0] = 255; colores_algo[3][1] = 255; colores_algo[3][2] =  50; /* amarillo */
    }

    /* ----- Versión 2: PPM ----- */
    mkdir(CARPETA_PPM, 0755); /* ignora error si ya existe */
    g_framebuffer = malloc((size_t)resolucion * resolucion * 3);
    if (!g_framebuffer) {
        printf("No se pudo reservar memoria para el buffer de la imagen\n");
        free(lineas);
        return 1;
    }
    // Itera sobre los algoritmos
    for (int a = 0; a < num_algos; a++) {
        // Limpia el buffer de la imagen
        clear_ppm();
        g_color_r = colores_algo[a][0];
        g_color_g = colores_algo[a][1];
        g_color_b = colores_algo[a][2];
        double t0 = get_time_sec();
        for (int rep = 0; rep < num_veces; rep++)
            for (int k = 0; k < num_lineas; k++)
                // Plotea la linea en el buffer de la imagen
                algos[a](lineas[k], plot_ppm);
        double t1 = get_time_sec();
        tiempos[1][a] = (t1 - t0) / num_veces;
        {
            char fn[128];
            snprintf(fn, sizeof(fn), "%s/%s.ppm", CARPETA_PPM, archivos_ppm[a]);
            // Escribe el ppm
            write_ppm(fn);
        }
    }
    // Libera el buffer de la imagen
    free(g_framebuffer);
    g_framebuffer = NULL;

    /* ----- SDL: ventana y renderer ----- */
    // Inicializa SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("No se pudo inicializar SDL: %s\n", SDL_GetError());
        free(lineas);
        return 1;
    }

    // Crea la ventana
    SDL_Window *window = SDL_CreateWindow("Proyecto 1 - Lineas",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        resolucion, resolucion, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("No se pudo crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        free(lineas);
        return 1;
    }

    // Crea el renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("No se pudo crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        free(lineas);
        return 1;
    }
    g_renderer = renderer;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* ----- Versión 3: SDL (dibujar, medir, pausa) ----- */
    for (int a = 0; a < num_algos; a++) {
        g_color_r = colores_algo[a][0];
        g_color_g = colores_algo[a][1];
        g_color_b = colores_algo[a][2];
        printf("Algoritmo: %s\n", nombres[a]);
        double t0 = get_time_sec();
        for (int rep = 0; rep < num_veces; rep++)
            for (int k = 0; k < num_lineas; k++)
                algos[a](lineas[k], plot_sdl);
        double t1 = get_time_sec();
        tiempos[2][a] = (t1 - t0) / num_veces;
        SDL_RenderPresent(renderer);
        printf("Presione una tecla para continuar...\n");
        wait_key_sdl();
    }

    /* ----- Tabla final de tiempos ----- */
    printf("\n");
    printf("+------------------+------------------+------------------+------------------+\n");
    printf("| %-16s | %12s | %12s | %12s |\n", "Algoritmo", "Vacia (s)", "PPM (s)", "SDL (s)");
    printf("+------------------+------------------+------------------+------------------+\n");
    for (int a = 0; a < num_algos; a++) {
        printf("| %-16s | %12.6f | %12.6f | %12.6f |\n",
            nombres[a], tiempos[0][a], tiempos[1][a], tiempos[2][a]);
    }
    printf("+------------------+------------------+------------------+------------------+\n");

    printf("\nPresione una tecla o cierre la ventana para salir.\n");
    wait_key_sdl();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(lineas);
    return 0;
}
