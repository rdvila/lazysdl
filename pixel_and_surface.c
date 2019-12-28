#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define FLIP_VERTICAL   1
#define FLIP_HORIZONTAL 2

Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[y * surface->w + x];
}

void put_pixel32(SDL_Surface *surface,  int x, int y, Uint32 pixel)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[y * surface->w + x] =  pixel;
}

SDL_Surface *flip_surface(SDL_Surface *surface, int flags)
{
    SDL_Surface *flipped = NULL;

    Uint8 alpha = 0;

    if (surface->flags & SDL_SRCCOLORKEY)
    {
        alpha = surface->format->Amask;
    }
    
    flipped = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h,
            surface->format->BitsPerPixel, surface->format->Rmask,
            surface->format->Gmask, surface->format->Bmask, alpha);

    if (SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }

    int x, y, rx, ry;
    for (x=0, rx = flipped->w -1; x < flipped->w; x++, rx--)
    {
        for (y=0, ry = flipped->h -1; y < flipped->h; y++, ry--)
        {
            Uint32 pixel =  get_pixel32(surface, x, y);

            if ((flags & FLIP_VERTICAL) && (flags & FLIP_HORIZONTAL))
                put_pixel32(flipped, rx, ry, pixel);

            else if (flags & FLIP_HORIZONTAL)
                put_pixel32(flipped, rx, y, pixel);

            else if (flags & FLIP_VERTICAL)
                put_pixel32(flipped, x, ry, pixel);
        } 
    }

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }

    if (surface->flags & SDL_SRCCOLORKEY)
    {
        SDL_SetColorKey(flipped,  SDL_RLEACCEL | SDL_SRCCOLORKEY,
                surface->format->colorkey);   
    }

    return flipped;
}

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loaded_image    = NULL;
    SDL_Surface *optimized_image = NULL;

    loaded_image = IMG_Load(name);

    if (!loaded_image)
    {
        fprintf(stderr, "Cannot load image %s: %s\n", name, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    optimized_image = SDL_DisplayFormat(loaded_image);
    SDL_FreeSurface(loaded_image);

    if (!optimized_image)
    {
        fprintf(stderr, "Cannot optimize image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(optimized_image, SDL_SRCCOLORKEY,
            SDL_MapRGB(optimized_image->format, 0x0, 0xff, 0xff));
    
    return optimized_image;
}

void apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dest,
        SDL_Rect *clip)
{
    SDL_Rect offset = {0};
    offset.x = x;
    offset.y = y;

    if (SDL_BlitSurface(src, clip, dest, &offset) == -1)
    {
        fprintf(stderr, "Cannot blit surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;

    SDL_Surface *top_left  = NULL;
    SDL_Surface *top_right = NULL;

    SDL_Surface *bottom_left  = NULL;
    SDL_Surface *bottom_right = NULL;

    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    top_left     = load_image("corner.png");
    top_right    = flip_surface(top_left, FLIP_HORIZONTAL); 
    bottom_left  = flip_surface(top_left, FLIP_VERTICAL); 
    bottom_right = flip_surface(top_left, FLIP_HORIZONTAL | FLIP_VERTICAL);

    apply_surface(  0,   0, top_left, screen, NULL);
    apply_surface(320,   0, top_right, screen, NULL);
    apply_surface(  0, 240, bottom_left, screen, NULL);
    apply_surface(320, 240, bottom_right, screen, NULL);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Event event;
    SDL_bool  quit = SDL_FALSE;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }
    }
   
    SDL_FreeSurface(top_left);
    SDL_FreeSurface(top_right);
    SDL_FreeSurface(bottom_left);
    SDL_FreeSurface(bottom_right);

    SDL_Quit();
 
    return 0;
}
