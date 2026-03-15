CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# SDL2: pkg-config en Linux; fallback Homebrew en Mac
SDL2_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL2_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)
ifneq ($(SDL2_LIBS),)
  CFLAGS  += $(SDL2_CFLAGS)
  LDFLAGS = $(SDL2_LIBS) -lm
else
  CFLAGS  += -I/opt/homebrew/include -I/usr/local/include
  LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lm
endif

TARGET = lineas
SRC = main.c \
	algoritmos/brute_force.c \
	algoritmos/incremental.c \
	algoritmos/incremental_v2.c \
	algoritmos/bresenham.c

# Objetivo por defecto: compilar ejecutable (deps se ejecuta al construir $(TARGET))
all: $(TARGET)

# Comprueba si SDL2 está disponible (pkg-config o cabecera en rutas típicas)
SDL2_OK := $(shell pkg-config --exists sdl2 2>/dev/null && echo 1)
ifeq ($(SDL2_OK),)
  SDL2_OK := $(shell test -f /opt/homebrew/include/SDL2/SDL.h 2>/dev/null && echo 1)
endif
ifeq ($(SDL2_OK),)
  SDL2_OK := $(shell test -f /usr/local/include/SDL2/SDL.h 2>/dev/null && echo 1)
endif

# Instalar dependencias según el SO si faltan
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
				echo "Ejecutando: sudo dnf install -y SDL2-devel"; \
				sudo dnf install -y SDL2-devel; \
			elif command -v pacman >/dev/null 2>&1; then \
				echo "Ejecutando: sudo pacman -S --noconfirm sdl2"; \
				sudo pacman -S --noconfirm sdl2; \
			elif command -v zypper >/dev/null 2>&1; then \
				echo "Ejecutando: sudo zypper install -y libSDL2-devel"; \
				sudo zypper install -y libSDL2-devel; \
			else \
				echo "No se detectó gestor de paquetes. Instala libsdl2-dev (o equivalente) manualmente."; exit 1; \
			fi; \
		else \
			echo "SO no soportado para auto-instalación. Instala SDL2 manualmente."; exit 1; \
		fi; \
	fi

$(TARGET): deps $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) 800 100 10

clean:
	rm -f $(TARGET)

.PHONY: all run clean deps
