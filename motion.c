#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define DOT_HEIGHT 50
#define DOT_WIDTH  50

#ifndef FPS60
    #define FPS 20
#else
    #define FPS 60
#endif

typedef struct
{
    int x , y;
    int xVel, yVel;

} dot;

typedef struct
{
    int start_ticks;
    int paused_ticks;

    SDL_bool paused;
    SDL_bool started;

} timer;

void timer_start(timer *t)
{
    t->started = SDL_TRUE;
    t->paused  = SDL_FALSE;
    t->start_ticks = SDL_GetTicks();
}

void timer_stop(timer *t)
{
    t->started = SDL_FALSE;
    t->paused  = SDL_FALSE;
}

void timer_pause(timer *t)
{
    if ((t->started) && (!t->paused))
    {
        t->paused = SDL_TRUE;
        t->paused_ticks = (SDL_GetTicks() - t->start_ticks);
    }
}

void timer_unpause(timer *t)
{
    if (t->paused)
    {
        t->paused = SDL_FALSE;
        t->start_ticks  = (SDL_GetTicks() - t->paused_ticks);
        t->paused_ticks = 0;
    }
}

int timer_get_ticks(timer *t)
{
    if (t->started)
    {
        if (t->paused)
            return t->paused_ticks;
        else
            return (SDL_GetTicks() - t->start_ticks);
    }

    return 0;
}

void dot_move(dot *d)
{
    printf("DOT_MOVE\n");
    d->x += d->xVel;
    d->y += d->yVel;

    if ((d->x < 0) || (d->x + DOT_WIDTH > SCREEN_WIDTH))
        d->x -= d->xVel;

    if ((d->y < 0) || (d->y + DOT_HEIGHT > SCREEN_HEIGHT))
        d->y -= d->yVel;
}


int main(int argc, char **argv)
{
    SDL_Surface *screen;
    dot mydot = {0};
    timer fps = {0};

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything");
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode");
        exit(EXIT_FAILURE);
    }

    SDL_Event event;
    SDL_bool  quit = SDL_FALSE;

    while (!quit)
    {
        timer_start(&fps);

        while  (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot.yVel -= DOT_HEIGHT / 2; break;
                    case SDLK_DOWN:  mydot.yVel += DOT_HEIGHT / 2; break;
                    case SDLK_LEFT:  mydot.xVel -= DOT_WIDTH  / 2; break;
                    case SDLK_RIGHT: mydot.xVel += DOT_WIDTH  / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot.yVel += DOT_HEIGHT / 2; break;
                    case SDLK_DOWN:  mydot.yVel -= DOT_HEIGHT / 2; break;
                    case SDLK_LEFT:  mydot.xVel += DOT_WIDTH  / 2; break;
                    case SDLK_RIGHT: mydot.xVel -= DOT_WIDTH  / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                quit = SDL_TRUE;
            }

            printf("xVel = %i\n", mydot.xVel);
            printf("yVel = %i\n", mydot.yVel);
        }

        dot_move(&mydot);

        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        SDL_Rect rect = {mydot.x, mydot.y, DOT_WIDTH, DOT_HEIGHT};

        SDL_FillRect(screen, &rect,
                SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Chama o suporte!!");
            exit(EXIT_FAILURE);
        }

        if (timer_get_ticks(&fps) < (1000 / FPS))
            SDL_Delay((1000 / FPS) -timer_get_ticks(&fps));
    }

    SDL_Quit();

    return 0;
}
