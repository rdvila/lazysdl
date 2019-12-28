#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define SQUARE_WIDTH  20
#define SQUARE_HEIGHT 20

#ifndef FPS60
    #define FPS 20
#else
    #define FPS 60
#endif

typedef struct
{
    SDL_Rect *box;
    int xVel, yVel;

} square;

SDL_bool check_collision(SDL_Rect *a, SDL_Rect *b)
{

    int lA, lB;
    int rA, rB;
    int tA, tB;
    int bA, bB;

    lA = a->x;
    rA = a->x + a->w;
    tA = a->y;
    bA = a->y + a->h;

    lB = b->x;
    rB = b->x + b->w;
    tB = b->y;
    bB = b->y + b->h;

    return !(bA <= tB || tA >= bB || rA <= lB || lA >= rB);
}

void square_move(square *s, SDL_Rect *rect)
{
    s->box->x += s->xVel;
    s->box->y += s->yVel;

    SDL_bool has_collision = check_collision(s->box, rect);

    if ((s->box->x < 0) || ((s->box->x + SQUARE_WIDTH) > SCREEN_WIDTH)
            || has_collision)
    {
        s->box->x -= s->xVel;
    }

    if ((s->box->y < 0) || ((s->box->y + SQUARE_HEIGHT) > SCREEN_HEIGHT)
            || has_collision)
    {
        s->box->y -= s->yVel;
    }
}

int main(int argc, char **argv)
{
    SDL_Rect wall = {300, 40, 40,400};
    SDL_Rect sqr  = {0, 0, SQUARE_WIDTH, SQUARE_HEIGHT};
    SDL_Surface *screen;

    square mysquare = {0};
    mysquare.box = &sqr;

    Uint32 start = 0;

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
        start = SDL_GetTicks();

        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mysquare.yVel -= SQUARE_HEIGHT / 2; break;
                    case SDLK_DOWN:  mysquare.yVel += SQUARE_HEIGHT / 2; break;
                    case SDLK_LEFT:  mysquare.xVel -= SQUARE_WIDTH  / 2; break;
                    case SDLK_RIGHT: mysquare.xVel += SQUARE_WIDTH  / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mysquare.yVel += SQUARE_HEIGHT / 2; break;
                    case SDLK_DOWN:  mysquare.yVel -= SQUARE_HEIGHT / 2; break;
                    case SDLK_LEFT:  mysquare.xVel += SQUARE_WIDTH  / 2; break;
                    case SDLK_RIGHT: mysquare.xVel -= SQUARE_WIDTH  / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;

        }

        square_move(&mysquare, &wall);

        //SDL_FillRect(screen, &screen->clip_rect,
        //        SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        SDL_FillRect(screen, &wall,
                SDL_MapRGB(screen->format, 0x77, 0x77, 0x77));

        SDL_FillRect(screen, mysquare.box,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen.");
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));
    }

    SDL_Quit();

    return 0;
}



