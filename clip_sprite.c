#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loadedImage    = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load(name);

    if (!loadedImage)
    {
        fprintf(stderr, "Cannot load image \"%s\".", name);
        exit(EXIT_FAILURE);
    }

    optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (!optimizedImage)
    {
        fprintf(stderr, "Cannot optimize image.");
        exit(EXIT_FAILURE);
    }

    return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source,
        SDL_Surface *destination, SDL_Rect *clip)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, clip, destination, &offset);
}

void init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *dots   = NULL;
    SDL_Surface *screen = NULL;

    SDL_Rect clip[4];

    clip[0].x = 0;
    clip[0].y = 0;
    clip[0].w = 200;
    clip[0].h = 200;

    clip[1].x = 200;
    clip[1].y = 0;
    clip[1].w = 200;
    clip[1].h = 200;

    clip[2].x = 0;
    clip[2].y = 200;
    clip[2].w = 200;
    clip[2].h = 200;

    clip[3].x = 200;
    clip[3].y = 200;
    clip[3].w = 200;
    clip[3].h = 200;

    init();

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
            SCREEN_BPP, SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set screen.");
        exit(EXIT_FAILURE);
    }

    dots = load_image("dots.png");

    SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    apply_surface(0  , 0  , dots, screen, &clip[0]);
    apply_surface(440, 0  , dots, screen, &clip[1]);
    apply_surface(0  , 280, dots, screen, &clip[2]);
    apply_surface(440, 280, dots, screen, &clip[3]);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }

    SDL_Delay(2000);

    SDL_FreeSurface(dots);
    SDL_Quit();

    return 0;
}
