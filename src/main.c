#include <SDL2/SDL.h>
#include <stdio.h>
#include "constants.h"
#include "map.h"
#include <SDL2/SDL_image.h>
#include "stb_ds.h"

SDL_Texture *loadTileset(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) { printf("IMG_Load error: %s\n", IMG_GetError()); return NULL; }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

SDL_Point getsize(SDL_Texture *texture) {
    SDL_Point size;
    SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    return size;
}

void drawTile(SDL_Renderer *renderer, SDL_Texture *tileset, 
              int tile_index, int dest_x, int dest_y) {
    int tileset_width_in_tiles = getsize(tileset).x / TILE_SIZE;
    int tile_col = tile_index % tileset_width_in_tiles;
    int tile_row = tile_index / tileset_width_in_tiles;

    SDL_Rect src = {
        tile_col * TILE_SIZE,
        tile_row * TILE_SIZE,
        TILE_SIZE, TILE_SIZE
    };
    SDL_Rect dst = {
        dest_x, dest_y,
        TILE_SIZE * SCREEN_SCALE, TILE_SIZE * SCREEN_SCALE  // scale up if needed
    };

    SDL_RenderCopy(renderer, tileset, &src, &dst);
}

void drawBlock(SDL_Renderer *renderer, SDL_Texture *tileset,
               char *blockset, int block_id, int dest_x, int dest_y) {
    struct block bl = get_block(blockset, block_id);
    for (int y = 0; y < 4; y++)
      for (int x = 0; x < 4; x++)
          drawTile(renderer, tileset, bl.tiles[y][x],
              dest_x + x * TILE_SIZE * SCREEN_SCALE,
              dest_y + y * TILE_SIZE * SCREEN_SCALE);
}
int main(void) {
    SDL_Window* window = NULL;

    loadBlockSets();
    loadMaps();

    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow(
        "Ember",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH*4,
        SCREEN_HEIGHT*4,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Rect rect = { 0, 0, 70, 40 };

    int running = 1;
    SDL_Event event;
    SDL_Texture *tileset = loadTileset(renderer, "data/tilesets/overworld.png");
    struct map *map = get_map("PalletTown");
    int c = 0;
    int shift = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderFillRect(renderer,&rect);
        for (int y = 0; y < map->height; y++)
          for (int x = 0; x < map->width; x++)
              drawBlock(renderer, tileset, map->bst->name,
                  (unsigned char)map->map_data[y * map->width + x],
                  x * TILE_SIZE * 4 * SCREEN_SCALE,
                  y * TILE_SIZE * 4 * SCREEN_SCALE);
        SDL_RenderPresent(renderer);
        rect.x++;
        c++;
        if (c % 25 == 0) shift++;

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
