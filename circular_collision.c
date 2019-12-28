#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL.h>
#include <SDL_image.h>

#define DOT_WIDTH  20
#define DOT_HEIGHT 20

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#ifndef FPS60
    #define FPS 20
#else
    #define FPS 60
#endif

typedef struct
{
    int x, y;
    int r;

} circle;

typedef struct
{
    circle circ;
    int xVel, yVel;

} dot;

typedef union
{
    circle circ;
    SDL_Rect rect;

} type;

typedef struct
{
    int count;
    type items[255];

} vector;

void vector_add_rectangle(vector *v, SDL_Rect rect)
{
    v->items[v->count].rect = rect;
    v->count += 1;
}

SDL_Rect *vector_get_rectangle(vector *v, int index)
{
    return &v->items[index].rect;
}

double distance(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x2 - x1, 2) +  pow(y2 - y1, 2));
}

SDL_bool check_collision_circle(circle *a, circle *b)
{
    return (distance(a->x, a->y, b->x, b->y) < (a->r + b->r));
}

SDL_bool check_collision_rectangles(circle *a, vector *v)
{
    int cX = 0, cY = 0;
    int bBox;

    for (bBox = 0; bBox < v->count; bBox++)
    {
        SDL_Rect *rect = vector_get_rectangle(v, bBox);

        if (a->x < rect->x)
            cX = rect->x;

        else if (a->x > (rect->x + rect->w))
            cX = rect->x + rect->w;

        else
            cX = a->x;

        if (a->y < rect->y)
            cY = rect->y;

        else if (a->y > (rect->y + rect->h))
            cY = rect->y + rect->h;

        else
            cY = a->y;

        if (distance(a->x, a->y, cX, cY) < a->r)
        {
            return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}

void dot_move(dot *d, vector *rects, circle *c)
{
    d->circ.x += d->xVel;
    d->circ.y += d->yVel;

    SDL_bool has_collision =
            (check_collision_rectangles(&d->circ, rects)
            || check_collision_circle(&d->circ, c));

    if (d->circ.x < (d->circ.r) || (d->circ.x + d->circ.r) > SCREEN_WIDTH
            || has_collision)
    {
        d->circ.x -= d->xVel;
    }

    if (d->circ.y < (d->circ.r) || (d->circ.y + d->circ.r) > SCREEN_HEIGHT
            || has_collision)
    {
        d->circ.y -= d->yVel;
    }
}

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
        SDL_Surface *destination)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, NULL, destination, &offset);
}

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *circle1;

    vector box;
    circle otherdot = {80, 80, DOT_WIDTH / 2};
    dot mydot = {{10, 10, DOT_WIDTH / 2}, 0, 0};

    Uint32 start = 0;

    {
        SDL_Rect r = {100, 100, 40, 40};
        vector_add_rectangle(&box, r);
    }

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

    circle1 = load_image("circle.png");

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot.yVel -= 1; break;
                    case SDLK_DOWN:  mydot.yVel += 1; break;
                    case SDLK_LEFT:  mydot.xVel -= 1; break;
                    case SDLK_RIGHT: mydot.xVel += 1; break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot.yVel += 1; break;
                    case SDLK_DOWN:  mydot.yVel -= 1; break;
                    case SDLK_LEFT:  mydot.xVel += 1; break;
                    case SDLK_RIGHT: mydot.xVel -= 1; break;
                    default: break;
                }
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        dot_move(&mydot, &box, &otherdot);

        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        int i;
        for (i = 0; i < box.count; i++)
        {
            SDL_FillRect(screen, vector_get_rectangle(&box, i),
                    SDL_MapRGB(screen->format, 0, 0, 0));
        }

        apply_surface(otherdot.x - otherdot.r , otherdot.y - otherdot.r,
                 circle1, screen);

        apply_surface(mydot.circ.x - mydot.circ.r ,
                mydot.circ.y - mydot.circ.r, circle1, screen);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen.");
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));

    }

    SDL_FreeSurface(circle1);
    SDL_Quit();
    return 0;
}
