#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_thread.h>

SDL_bool quit;

int my_thread(void *data)
{
    while (!quit)
    {
        SDL_WM_SetCaption("Thread is running", NULL);
        SDL_Delay(250);

        SDL_WM_SetCaption("Thread is running.", NULL);
        SDL_Delay(250);

        SDL_WM_SetCaption("Thread is running..", NULL);
        SDL_Delay(250);

        SDL_WM_SetCaption("Thread is running...", NULL);
        SDL_Delay(250);
    }

    return 0;
}

int main(int argc, char **argv)
{
    quit = SDL_FALSE;

    SDL_Surface *screen = NULL;
    SDL_Thread  *thread = NULL;

    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    thread = SDL_CreateThread(my_thread, NULL);

    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;   
        }
    }

    SDL_KillThread(thread);
    SDL_Quit();

    return 0;
}
