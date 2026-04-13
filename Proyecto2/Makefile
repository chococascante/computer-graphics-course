CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)
ifneq ($(SDL2_LIBS),)
  CFLAGS  += $(SDL2_CFLAGS)
  LDFLAGS = $(SDL2_LIBS) -lm
else
  CFLAGS  += -I/opt/homebrew/include -I/usr/local/include
  LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lm
endif

TARGET = proyecto2
SRC = main.c \
	framebuffer.c \
	transform.c \
	clipping.c \
	rasterizer.c \
	texture.c \
	texture_bank.c \
	view_control.c \
	scene_geometry.c \
	algoritmos/bresenham.c

SDL2_OK := $(shell pkg-config --exists sdl2 2>/dev/null && echo 1)
ifeq ($(SDL2_OK),)
  SDL2_OK := $(shell test -f /opt/homebrew/include/SDL2/SDL.h 2>/dev/null && echo 1)
endif
ifeq ($(SDL2_OK),)
  SDL2_OK := $(shell test -f /usr/local/include/SDL2/SDL.h 2>/dev/null && echo 1)
endif

.PHONY: all run clean deps

all: $(TARGET)

deps:
	@if [ -n "$(SDL2_OK)" ]; then \
		echo "SDL2 ya está disponible."; \
	else \
		UNAME=$$(uname -s); \
		echo "SDL2 no encontrado. Intentando instalar para $$UNAME..."; \
		if [ "$$UNAME" = "Darwin" ]; then \
			if command -v brew >/dev/null 2>&1; then \
				brew install sdl2 && echo "SDL2 instalado con Homebrew."; \
			else \
				echo "Instala Homebrew (https://brew.sh) y ejecuta: brew install sdl2"; exit 1; \
			fi; \
		elif [ "$$UNAME" = "Linux" ]; then \
			if command -v apt-get >/dev/null 2>&1; then \
				echo "Ejecutando: sudo apt-get install -y libsdl2-dev"; \
				sudo apt-get install -y libsdl2-dev; \
			elif command -v dnf >/dev/null 2>&1; then \
				sudo dnf install -y SDL2-devel; \
			elif command -v pacman >/dev/null 2>&1; then \
				sudo pacman -S --noconfirm sdl2; \
			elif command -v zypper >/dev/null 2>&1; then \
				sudo zypper install -y libSDL2-devel; \
			else \
				echo "Instala libsdl2-dev manualmente."; exit 1; \
			fi; \
		else \
			echo "SO no soportado para auto-instalación."; exit 1; \
		fi; \
	fi

$(TARGET): deps $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
