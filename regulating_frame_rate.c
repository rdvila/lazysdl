#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32
#define FRAMERATE      20

void apply_surface(int x, int y, SDL_Surface *source,
        SDL_Surface *destination)
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(source, NULL, destination, &offset);
}

void init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything.");
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Cannot init ttf.");
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("Frames", NULL);
}

void draw_message(SDL_Surface *screen, SDL_Surface *message1)
{
    SDL_FillRect(screen, &screen->clip_rect,
        SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    apply_surface((SCREEN_WIDTH - message1->w) / 2,
            ((SCREEN_HEIGHT - message1->h) / 2),
            message1, screen);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;

    SDL_Surface *message1  = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {0, 0, 0};

    Uint32 start = 0;

    init();

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set screen.");
        exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont("LinLibertine.ttf", 30);

    if (!font)
    {
        fprintf(stderr, "Cannot load font.");
        exit(EXIT_FAILURE);
    }

    message1 = TTF_RenderText_Blended(font,
            "Testing frame rate.", fontColor);

    SDL_Event event;

    for(;;)
    {
        start = SDL_GetTicks();

        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                break;
        }

        draw_message(screen, message1);

        if ((SDL_GetTicks() - start) < (1000 / FRAMERATE))
            SDL_Delay((1000 / FRAMERATE) - (SDL_GetTicks() - start));

    }

    SDL_FreeSurface(message1);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
