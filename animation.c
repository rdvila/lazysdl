#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  649
#define SCREEN_HEIGHT 480
#define SCREEN_BPP      32

#ifndef FPS60
    #define FPS 20
#else
    #define FPS 60
#endif

#define FOO_WIDTH   64
#define FOO_HEIGHT 205
#define FOO_RIGHT    0
#define FOO_LEFT     1

typedef struct
{
    int offset;
    int velocity;
    int frame;
    int status;

} foo;

typedef struct
{
    int count;
    SDL_Rect items[16];

} vector;

void vector_add(vector *v, SDL_Rect rect)
{
    v->items[v->count] = rect;
    v->count += 1;
}

SDL_Rect create_rectangle(int x, int y, int w, int h)
{
    SDL_Rect rect = {x, y, w, h};
    return rect;
}

void set_clips(vector *left, vector *right)
{
    vector_add(right, create_rectangle(0, 0, FOO_WIDTH, FOO_HEIGHT));
    vector_add(right, create_rectangle(FOO_WIDTH, 0, FOO_WIDTH, FOO_HEIGHT));
    vector_add(right, create_rectangle(FOO_WIDTH *2, 0, FOO_WIDTH, FOO_HEIGHT));
    vector_add(right, create_rectangle(FOO_WIDTH *3, 0, FOO_WIDTH, FOO_HEIGHT));

    vector_add(left, create_rectangle(0, FOO_HEIGHT, FOO_WIDTH, FOO_HEIGHT));
    vector_add(left, create_rectangle(FOO_WIDTH, FOO_HEIGHT,
        FOO_WIDTH, FOO_HEIGHT));
    vector_add(left, create_rectangle(FOO_WIDTH *2, FOO_HEIGHT,
        FOO_WIDTH, FOO_HEIGHT));
    vector_add(left, create_rectangle(FOO_WIDTH *3, FOO_HEIGHT,
        FOO_WIDTH, FOO_HEIGHT));
}

void foo_move(foo *f)
{
    f->offset += f->velocity;

    if ((f->offset < 0) || (f->offset + FOO_WIDTH > SCREEN_WIDTH))
    {
        f->offset -= f->velocity;
    }
}

void foo_show(foo *walk)
{
    if (walk->velocity < 0)
    {
        walk->status = FOO_LEFT;
        walk->frame++;
    }
    else if (walk->velocity > 0)
    {
        walk->status = FOO_RIGHT;
        walk->frame++;
    }
    else
        walk->frame = 0;

    if (walk->frame >= 4)
        walk->frame = 0;
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
        SDL_Surface *destination, SDL_Rect *clip)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, clip, destination, &offset);
}

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *ffoo;

    vector clipsright = {0};
    vector clipsleft  = {0};

    foo walk = {0, 0, 0, FOO_RIGHT};

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

    ffoo = load_image("foo.png");
    set_clips(&clipsleft, &clipsright);

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
                    case SDLK_RIGHT: walk.velocity += FOO_WIDTH / 4; break;
                    case SDLK_LEFT:  walk.velocity -= FOO_WIDTH / 4; break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_RIGHT: walk.velocity -= FOO_WIDTH / 4; break;
                    case SDLK_LEFT:  walk.velocity += FOO_WIDTH / 4; break;
                    default: break;
                }
            }

            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        foo_move(&walk);

        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 255, 255, 255));

        if (walk.status == FOO_RIGHT)
            apply_surface(walk.offset, SCREEN_HEIGHT - FOO_HEIGHT,
                    ffoo, screen, &clipsright.items[walk.frame]);
        else
            apply_surface(walk.offset, SCREEN_HEIGHT - FOO_HEIGHT,
                    ffoo, screen, &clipsleft.items[walk.frame]);

        foo_show(&walk);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen.");
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));

    }

    SDL_FreeSurface(ffoo);
    SDL_Quit();

    return 0;
}
