CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)

ifneq ($(SDL2_LIBS),)
  CFLAGS  += -DHAVE_SDL2 $(SDL2_CFLAGS)
  LDFLAGS := $(SDL2_LIBS) $(LDFLAGS)
endif

TARGET = raytrace_poc

SRC = main.c \
      escena.c \
      geometria.c \
      sombreado.c \
      camara.c \
      ppm.c \
      sdl_vista.c \
      render.c

OBJ = $(SRC:.c=.o)
HDR = vec3.h tipos.h escena.h geometria.h sombreado.h camara.h ppm.h sdl_vista.h render.h

.PHONY: all run run-snowman run-kirby clean rebuild deps

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

run: run-snowman

run-snowman: $(TARGET)
	./$(TARGET) scene_snowman.txt

run-kirby: $(TARGET)
	./$(TARGET) scene_kirby.txt

rebuild: clean all

deps:
	@if pkg-config --exists sdl2; then \
		echo "SDL2 ya está disponible."; \
	else \
		echo "SDL2 no encontrado. Instalando libsdl2-dev..."; \
		sudo apt-get update && sudo apt-get install -y libsdl2-dev; \
	fi

clean:
	rm -f $(TARGET) $(OBJ) *.ppm
