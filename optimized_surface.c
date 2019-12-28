#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loadedImage    = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = SDL_LoadBMP(name);

    if (!loadedImage)
    {
        fprintf(stderr, "Cannot load image \"%s\".", name);
        exit(EXIT_FAILURE);
    }

    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

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

int main(int argc, char **argv)
{
    SDL_Surface *message    = NULL;
    SDL_Surface *background = NULL;
    SDL_Surface *screen     = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init EVERYTHING.");
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT,
            SCREEN_BPP, SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode.");
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("Hello World!!", NULL);

    message    = SDL_LoadBMP("hello.bmp");
    background = SDL_LoadBMP("background.bmp");

    apply_surface(0  ,   0, background, screen);
    apply_surface(320,   0, background, screen);
    apply_surface(0  , 240, background, screen);
    apply_surface(320, 240, background, screen);

    apply_surface(120, 40, message, screen);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }

    SDL_Delay(2000);

    SDL_FreeSurface(message);
    SDL_FreeSurface(background);

    SDL_Quit();

    return 0;
}
