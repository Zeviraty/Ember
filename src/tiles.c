#include <SDL2/SDL.h>
#include "constants.h"
#include "map.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>

SDL_Texture *loadTileset(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) { printf("IMG_Load error: %s\n", IMG_GetError()); return NULL; }
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));
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

void drawSprite(SDL_Renderer *renderer, SDL_Texture *tileset,
                int sprite_id, int dest_x, int dest_y) {
    for (int y = 0; y < 2; y++)
      for (int x = 0; x < 2; x++)
          drawTile(renderer, tileset, (y+(sprite_id*2))*2+x,
              dest_x + x * TILE_SIZE * SCREEN_SCALE,
              dest_y + y * TILE_SIZE * SCREEN_SCALE);
}
