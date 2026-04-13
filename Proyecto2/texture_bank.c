/* BMP 24/32 desde carpeta; el orden en el array es el de readdir (no garantizado entre SO). */
#include "texture_bank.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

void vaciar_banco_texturas(BancoTexturas *banco) {
    if (!banco)
        return;
    for (int i = 0; i < banco->cantidad; i++)
        destruir_textura(&banco->texturas[i]);
    banco->cantidad = 0;
}

static int termina_en_bmp(const char *nombre) {
    size_t n = strlen(nombre);
    if (n < 4u)
        return 0;
    return strcasecmp(nombre + n - 4u, ".bmp") == 0;
}

int cargar_texturas_desde_carpeta(BancoTexturas *banco, const char *carpeta) {
    if (!banco || !carpeta)
        return 0;
    vaciar_banco_texturas(banco);

    DIR *d = opendir(carpeta);
    if (!d) {
        fprintf(stderr, "texturas: no se abre la carpeta \"%s\"\n", carpeta);
        return 0;
    }

    struct dirent *ent;
    char ruta[1024];
    while ((ent = readdir(d)) != NULL && banco->cantidad < MAX_TEXTURAS_EN_BANCO) {
        if (ent->d_name[0] == '.')
            continue;
        if (!termina_en_bmp(ent->d_name))
            continue;
        if (snprintf(ruta, sizeof ruta, "%s/%s", carpeta, ent->d_name) >= (int)sizeof ruta)
            continue;
        if (cargar_bmp(&banco->texturas[banco->cantidad], ruta) == 0) {
            banco->cantidad++;
        } else {
            fprintf(stderr, "texturas: no se pudo leer \"%s\"\n", ruta);
        }
    }
    closedir(d);
    return banco->cantidad;
}
