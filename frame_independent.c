#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define DOT_WIDTH  20
#define DOT_HEIGHT 20
#define DOT_VEL    200

#define FPS 20

typedef struct
{
    float x, y;
    float xVel, yVel;

} dot;

void dot_handle_input(dot *d, SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:    d->yVel -= DOT_VEL; break;
            case SDLK_DOWN:  d->yVel += DOT_VEL; break;
            case SDLK_LEFT:  d->xVel -= DOT_VEL; break;
            case SDLK_RIGHT: d->xVel += DOT_VEL; break;
            default: break;
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_UP:    d->yVel += DOT_VEL; break;
            case SDLK_DOWN:  d->yVel -= DOT_VEL; break;
            case SDLK_LEFT:  d->xVel += DOT_VEL; break;
            case SDLK_RIGHT: d->xVel -= DOT_VEL; break;
            default: break;
        }
    }
}

void dot_move(dot *d, Uint32 deltaTicks)
{
    d->x += d->xVel * (deltaTicks / 1000.0f);
    d->y += d->yVel * (deltaTicks / 1000.0f);

    if (d->x < 0)
        d->x = 0;

    else if(d->x + DOT_WIDTH > SCREEN_WIDTH)
        d->x = SCREEN_WIDTH - DOT_WIDTH;
    
    if (d->y < 0)
        d->y = 0;

    else if(d->y + DOT_HEIGHT > SCREEN_HEIGHT)
        d->y = SCREEN_HEIGHT - DOT_HEIGHT;
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;
    Uint32 start = 0;   

    dot mydot = {0};

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
   
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {   
            dot_handle_input(&mydot, &event);

            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        SDL_Rect d = {mydot.x, mydot.y, DOT_WIDTH, DOT_HEIGHT};
        SDL_FillRect(screen, &d,
            SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 ticks = SDL_GetTicks() - start;
        dot_move(&mydot, ticks);

        Uint32 currTicks = SDL_GetTicks();

        if ((currTicks - start) < (1000 / FPS))
        {
            SDL_Delay((1000 / FPS) - (currTicks - start));
        }

    }

    SDL_Quit();
    return 0;   
     
}
