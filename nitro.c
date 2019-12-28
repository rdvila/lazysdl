#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

SDL_Surface *load_image(const char *image)
{
    SDL_Surface *loadedimg  = NULL;
    SDL_Surface *optimedimg = NULL;

    loadedimg = IMG_Load(image);

    if (!loadedimg)
    {
        fprintf(stderr, "Cannot load image %s: %s\n", image, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    optimedimg = SDL_DisplayFormat(loadedimg);
    SDL_FreeSurface(loadedimg);

    if (!optimedimg)
    {
        fprintf(stderr, "Cannot optimize image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(optimedimg, SDL_SRCCOLORKEY,
            SDL_MapRGB(optimedimg->format, 0xff, 0x0, 0xff));

    return optimedimg;
}

void apply_surface(int x, int y, SDL_Surface *src,
        SDL_Surface *dest, SDL_Rect *clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;

    if (SDL_BlitSurface(src, clip, dest, &offset) == -1)
    {
        fprintf(stderr, "Cannot blit surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void set_clip(SDL_Rect *rect, int frame)
{
    rect->x = frame * 36;
    rect->y = 0; //frame * 45;
    rect->w = 36;
    rect->h = 45;
}

int main()
{

    SDL_Surface *screen = NULL;
    SDL_Surface *nitro = NULL;

    Uint32 start = 0;

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

    nitro = load_image("nitro.bmp");

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;
    int frame = 0;
    SDL_Rect clip = {0};

    while(!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    if (frame >= 7)
                        frame = 0;

                    frame++;
                }
            }

            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        set_clip(&clip, frame);
        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        printf("Rect = %i, %i, %i, %i | frame = %i\n", clip.x, clip.y,
                clip.w, clip.h, frame);

        apply_surface(100, 100, nitro, screen, &clip);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000 / 20))
            SDL_Delay((1000 / 20) - (currticks - start));

    }

    SDL_FreeSurface(nitro);
    SDL_Quit();
    return 0;
}
