#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define FPS 20

int main(int argc, char **argv)
{
    atexit(SDL_Quit);
    SDL_bool fullscreen = SDL_FALSE;

    if (argc > 1 && (strcmp("--fullscreen", argv[1]) == 0))
    {
        fullscreen = SDL_TRUE;
    }

    SDL_Surface *screen = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    Uint32 flags = SDL_SWSURFACE | SDL_RESIZABLE;

    if (fullscreen)
        flags = flags | SDL_FULLSCREEN;

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, flags);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;
    Uint32 start  = 0;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_VIDEORESIZE)
            {
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h,
                        SCREEN_BPP, flags);
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = SDL_TRUE;
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "%s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();
        if ((currticks - start) <= (1000 / FPS))
        {
           SDL_Delay((1000 / FPS) - (currticks - start));
        }
    }

    SDL_Quit();
    return 0;
}

