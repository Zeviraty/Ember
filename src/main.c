#include <SDL2/SDL.h>
#include <stdio.h>
#include "constants.h"
#include "map.h"
#include <SDL2/SDL_image.h>
#include "tiles.h"

int init_rendering(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    *window = SDL_CreateWindow(
        "Ember",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH*SCREEN_SCALE,
        SCREEN_HEIGHT*SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );

    if (!*window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowResizable(*window,0);

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    if (!*renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }

    return 0;
};

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;

    loadBlockSets();
    loadMaps();

    int render = init_rendering(&window, &renderer);
    if (render != 0) return render;

    int running = 1;

    SDL_Event event;

    SDL_Texture *tileset = loadTileset(renderer, "data/tilesets/overworld.png");
    struct map *map = get_map("PalletTown");

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch( event.type ){
              case SDL_QUIT:
                running = 0;
                break;

              case SDL_KEYDOWN:
                printf( "Key press detected\n" );
                break;

              case SDL_KEYUP:
                printf( "Key release detected\n" );
                break;

              default:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < map->height; y++)
          for (int x = 0; x < map->width; x++)
              drawBlock(renderer, tileset, map->bst->name,
                  (unsigned char)map->map_data[y * map->width + x],
                  x * TILE_SIZE * 4 * SCREEN_SCALE,
                  y * TILE_SIZE * 4 * SCREEN_SCALE);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
