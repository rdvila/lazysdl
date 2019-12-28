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

    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (!optimizedImage)
    {
        fprintf(stderr, "Cannot optimize image.");
        exit(EXIT_FAILURE);
    }

    Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0xFF, 0, 0xFF);
    SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);

    return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source,
        SDL_Surface *destination)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, NULL, destination, &offset);
}

void init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Cannot init everything. %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *image       = NULL;
    SDL_Surface *background  = NULL;
    SDL_Surface *screen      = NULL;

    init();

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
            SCREEN_BPP, SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set screen.");
        exit(EXIT_FAILURE);
    }

    background = load_image("everest.jpg");
    image = load_image("spaceship.bmp");

    apply_surface(0, 0, background, screen);
    apply_surface(320, 20, image, screen);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }

    SDL_Delay(2000);

    SDL_FreeSurface(background);
    SDL_FreeSurface(image);
    SDL_Quit();

    return 0;
}
