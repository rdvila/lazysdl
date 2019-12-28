#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#ifndef FPS60
    #define FPS 20
#else
    #define FPS 60
#endif

#define DOT_WIDTH  20
#define DOT_HEIGHT 20

#define LEVEL_WIDTH  1280
#define LEVEL_HEIGHT  960

typedef struct
{
    int x, y;
    int xVel, yVel;

} dot;

void dot_move(dot *d)
{
    d->x += d->xVel;
    d->y += d->yVel;

    if ((d->x < 0) || (d->x + DOT_WIDTH > LEVEL_WIDTH))
    {
        d->x -= d->xVel;
    }

    if ((d->y < 0) || (d->y + DOT_HEIGHT > LEVEL_HEIGHT))
    {
        d->y -= d->yVel;
    }
}

void dot_set_camera(dot *d, SDL_Rect *camera)
{
    camera->x = (d->x + DOT_WIDTH  / 2) - (SCREEN_WIDTH  / 2);
    camera->y = (d->y + DOT_HEIGHT / 2) - (SCREEN_HEIGHT / 2);

    if (camera->x < 0)
        camera->x = 0;

    if (camera->y < 0)
        camera->y = 0;

    if (camera->x  > (LEVEL_WIDTH -camera->w))
        camera->x = (LEVEL_WIDTH -camera->w);

    if (camera->y  > (LEVEL_HEIGHT -camera->h))
        camera->y = (LEVEL_HEIGHT -camera->h);
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
        SDL_Surface *destination, SDL_Rect *camera)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, camera, destination, &offset);
}

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *background;
    SDL_Surface *circle;

    Uint32 start = 0;

    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    dot mydot = {0, 0, 0, 0};

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything.");
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode.");
        exit(EXIT_FAILURE);
    }

    background = load_image("background3.png");
    circle     = load_image("circle.png");


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
                quit = SDL_TRUE;
        }

        dot_move(&mydot);
        dot_set_camera(&mydot, &camera);

        apply_surface(0, 0, background, screen, &camera);
        apply_surface(mydot.x -camera.x, mydot.y -camera.y,
                circle, screen, NULL);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen.");
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));

    }


    SDL_FreeSurface(circle);
    SDL_FreeSurface(background);

    SDL_Quit();
    return 0;
}
