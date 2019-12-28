#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#define TILE_WIDTH   80
#define TILE_HEIGHT  80
#define TILE_SPRITES 12

#define TOTAL_TILES 192

typedef enum
{
    TILE_RED,
    TILE_GREEN,
    TILE_BLUE,
    TILE_CENTER,
    TILE_TOP,
    TILE_TOPRIGHT,
    TILE_RIGHT,
    TILE_BOTTOMRIGHT,
    TILE_BOTTON,
    TILE_BOTTONLEFT,
    TILE_LEFT,
    TILE_TOPLRFT
} tile_type;

typedef struct
{
    SDL_Rect box;
    tile_type type;
} tile;

typedef struct
{
    SDL_Rect box;
    int xvel, yvel;
} dot;

tile *tile_create(int x, int y, tile_type type)
{
    tile *t = malloc(sizeof(tile));
    t->box.x = x;
    t->box.y = y;

    t->box.w = TILE_WIDTH;
    t->box.h = TILE_HEIGHT;

    t->type = type;
}

void map_set_tiles(tile **tiles)
{
    int x = 0, y = 0;

    FILE *map = fopen("lazy.map", "r");
    
}
