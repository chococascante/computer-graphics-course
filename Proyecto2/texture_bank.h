#ifndef TEXTURE_BANK_H
#define TEXTURE_BANK_H

/* Texturas de disco para modos 3 y 4; si falla todo, main sigue con tablero procedural. */
#include "texture.h"

#define MAX_TEXTURAS_EN_BANCO 64

typedef struct BancoTexturas {
    Texture texturas[MAX_TEXTURAS_EN_BANCO];
    int cantidad;
} BancoTexturas;

void vaciar_banco_texturas(BancoTexturas *banco);
int cargar_texturas_desde_carpeta(BancoTexturas *banco, const char *carpeta);

#endif
