CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -O2 -MMD -MP -Iincludes
LDFLAGS = -lSDL2 -lSDL2_image

WIN_CC = x86_64-w64-mingw32-gcc
WIN_SDL = libs/SDL2/x86_64-w64-mingw32
WIN_SDL_IMAGE = libs/SDL2_image/x86_64-w64-mingw32
WIN_OBJ = $(patsubst src/%.c, build/win_%.o, $(SRC))

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

TARGET = build/ember

all: $(TARGET)


win: $(WIN_OBJ)
	$(WIN_CC) $(WIN_OBJ) \
		-L$(WIN_SDL)/lib \
		-L$(WIN_SDL_IMAGE)/lib \
		-lmingw32 -lSDL2main $(LDFLAGS) \
		-mwindows \
		-o build/ember.exe
	cp $(WIN_SDL)/bin/SDL2.dll build/
	cp $(WIN_SDL_IMAGE)/bin/SDL2_image.dll build/

build/win_%.o: src/%.c
	mkdir -p build
	$(WIN_CC) -Wall -Wextra -std=gnu11 -O2 -Iincludes \
		-I$(WIN_SDL)/include \
		-I$(WIN_SDL)/include/SDL2 \
		-I$(WIN_SDL_IMAGE)/include \
		-I$(WIN_SDL_IMAGE)/include/SDL2 \
		-c $< -o $@

$(TARGET): $(OBJ)
	mkdir -p build
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
