#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef TILES_H

#define TILES_H

SDL_Texture *loadTileset(SDL_Renderer *renderer, const char *path);
SDL_Point getsize(SDL_Texture *texture);

void drawTile(SDL_Renderer *renderer, SDL_Texture *tileset, 
              int tile_index, int dest_x, int dest_y);
void drawBlock(SDL_Renderer *renderer, SDL_Texture *tileset,
               char *blockset, int block_id, int dest_x, int dest_y);
void drawSprite(SDL_Renderer *renderer, SDL_Texture *tileset,
                int sprite_id, int dest_x, int dest_y, SDL_RendererFlip flip);

#endif
