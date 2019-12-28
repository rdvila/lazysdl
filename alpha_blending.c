#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCRENN_BPP     32

#define FPS 20

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loadedImage    = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load(name);

    if (!loadedImage)
    {
        fprintf(stderr, "Cannot load image %s: %s", name, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (!optimizedImage)
    {
        fprintf(stderr, "Cannot optimize image: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source,
        SDL_Surface *destination, SDL_Rect *clip)
{

    SDL_Rect offset = {0};

    offset.x = x;
    offset.y = y;

    if (SDL_BlitSurface(source, clip, destination, &offset) == -1)
    {
        fprintf(stderr, "Cannot blit surface: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *rect1;
    SDL_Surface *rect2;

    Uint32 start = 0;

    int alpha = SDL_ALPHA_OPAQUE;

    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCRENN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

   rect1 = load_image("rect1.png");
   rect2 = load_image("rect2.png");

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = SDL_TRUE;
            }
        }

        Uint8 *keystates = SDL_GetKeyState(NULL);

        if (keystates[SDLK_UP])
        {
            if (alpha < SDL_ALPHA_OPAQUE)
                alpha += 5;
        }

        if (keystates[SDLK_DOWN])
        {
            if (alpha > SDL_ALPHA_TRANSPARENT)
                alpha -= 5;
        }


        SDL_SetAlpha(rect2, SDL_SRCALPHA, alpha);

        apply_surface(0, 0, rect1, screen, NULL);
        apply_surface(50, 50, rect2, screen, NULL);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000 / FPS))
            SDL_Delay((1000/ FPS) - (currticks - start));
    }

    SDL_FreeSurface(rect1);
    SDL_FreeSurface(rect2);

    SDL_Quit();

    return 0;
}

