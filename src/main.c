#include <SDL2/SDL.h>
#include <stdio.h>
#include "constants.h"
#include "map.h"
#include <SDL2/SDL_image.h>
#include "tiles.h"
#include "player.h"
#include "stdbool.h"

#ifdef WINDOWS
#include <math.h>
#endif

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

int sign(int x)
{
    return (x>>31) | ((unsigned)-x >> 31);
}

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
    SDL_Texture *player_sheet = loadTileset(renderer, "data/sprites/red.png");

    struct map *map = get_map("PalletTown");
    struct map *north = map->north.name ? get_map(map->north.name) : NULL;
    struct map *east  = map->east.name  ? get_map(map->east.name)  : NULL;
    struct map *south = map->south.name ? get_map(map->south.name) : NULL;
    struct map *west  = map->west.name  ? get_map(map->west.name)  : NULL;

    struct Player player = {
        (map->width  / 2) * METABLOCK_SIZE,
        (map->height / 2) * METABLOCK_SIZE,
        0, 0
    };

    int cycle = 0;
    bool alt_sprite = 0;
    bool facing_flip = 0;
    int facing = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch( event.type ){
              case SDL_QUIT:
                running = 0;
                break;

              case SDL_KEYDOWN:
                switch( event.key.keysym.sym ) {
                    case SDLK_LEFT:
                        if (player.xvel == 0 && player.yvel == 0) player.xvel = -1 * (TILE_SIZE*2);
                        break;
                    case SDLK_RIGHT:
                        if (player.xvel == 0 && player.yvel == 0) player.xvel =  1 * (TILE_SIZE*2);
                        break;
                    case SDLK_UP:
                        if (player.xvel == 0 && player.yvel == 0) player.yvel = -1 * (TILE_SIZE*2);
                        break;
                    case SDLK_DOWN:
                        if (player.xvel == 0 && player.yvel == 0) player.yvel =  1 * (TILE_SIZE*2);
                        break;
                    default:
                        break;
                };
                break;

              default:
                break;
            }
        }

        if (player.xvel != 0) {
          player.x += sign(player.xvel)*2;
          player.xvel -= sign(player.xvel)*2;
        };
        if (player.yvel != 0) {
          player.y += sign(player.yvel)*2;
          player.yvel -= sign(player.yvel)*2;
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        int cam_x = player.x - CAM_OFFSET_X;
        int cam_y = player.y - CAM_OFFSET_Y;

        int block_x = (int)floor((double)cam_x / METABLOCK_SIZE);
        int block_y = (int)floor((double)cam_y / METABLOCK_SIZE);

        int pixel_off_x = cam_x - block_x * METABLOCK_SIZE;
        int pixel_off_y = cam_y - block_y * METABLOCK_SIZE;

        for (int y = 0; y < SCREEN_BLOCKS_H+1; y++)
          for (int x = 0; x < SCREEN_BLOCKS_W+1; x++) {
              int bx = x + block_x;
              int by = y + block_y;
              if (bx < 0 || by < 0 || bx >= map->width || by >= map->height) continue; // IMPLEMETN GETTING BLOCKS OF CONNECTED MAPS
              drawBlock(renderer, tileset, map->bst->name,
                  (unsigned char)map->map_data[by * map->width + bx],
                  (x * METABLOCK_SIZE - pixel_off_x) * SCREEN_SCALE,
                  (y * METABLOCK_SIZE - pixel_off_y) * SCREEN_SCALE);
          }

        if (player.xvel != 0 || player.yvel != 0) cycle++;
        else alt_sprite = 0;

        if (player.xvel > 0) { facing = 2; facing_flip = 1; }
        if (player.xvel < 0) { facing = 2; facing_flip = 0; }
        if (player.yvel > 0) { facing = 0; }
        if (player.yvel < 0) { facing = 1; }

        int step = (cycle / 4) % 4;
        alt_sprite = (step & 1) != 0;

        bool flip = 0;

        if (facing == 2)
            flip = facing_flip;
        else
            flip = (step & 2) != 0;

        int sprite = facing;
        if (alt_sprite) sprite += 3;

        if (flip)
          drawSprite(renderer, player_sheet, sprite,
                  CAM_OFFSET_X * SCREEN_SCALE,
                  CAM_OFFSET_Y * SCREEN_SCALE,
                  SDL_FLIP_HORIZONTAL);
        else
          drawSprite(renderer, player_sheet, sprite,
                  CAM_OFFSET_X * SCREEN_SCALE,
                  CAM_OFFSET_Y * SCREEN_SCALE,
                  SDL_FLIP_NONE);

        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
