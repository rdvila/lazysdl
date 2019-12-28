#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define FPS 60

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

int main(int argc, char **argv)
{
    atexit(SDL_Quit);

    SDL_Surface *screen;
    SDL_Surface *background;

    int bgX = 0, bgY = 0;

    Uint32 start = 0;

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

    background = load_image("background4.png");

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        bgX -= 2;

        if (bgX <= -background->w)
        {
            bgX = 0;
        }

        apply_surface(bgX, bgY, background, screen, NULL);
        apply_surface(bgX + background->w, bgY, background, screen, NULL);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));
    }

    SDL_FreeSurface(background);

    SDL_Quit();

    return 0;
}
