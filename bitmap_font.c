#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

typedef struct
{
    SDL_Surface *bitmap;
    SDL_Rect chars[256];

    int new_line, space;

} bitmap_font;

UInt32 get_pixel32(int x, int y, SDL_Surface *surface)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[(y * surface->w) + x];
}

void bitmap_font_build_font(bitmap_font *f, SDL_Surface *surface)
{
    f->bitmap = surface;
    Uint32 bgcolor = SDL_MapRGB(f->bitmap->format, 0xff, 0x0, 0xff);

    int cellw = f->bitmap->w / 16;
    int cellh = f->bitmap->h / 16;

    int top   = cellh;
    int basea = cellh;

    
    
}
