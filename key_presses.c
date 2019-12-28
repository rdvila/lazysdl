#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

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

    SDL_WM_SetCaption("TTF events test", NULL);
}

void draw_message(SDL_Surface *screen, SDL_Surface *message)
{
    SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    if (message)
    {
        apply_surface((SCREEN_WIDTH - message->w) / 2,
                (SCREEN_HEIGHT - message->h) / 2,
                message, screen);
    }

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;

    SDL_Surface *upMessage    = NULL;
    SDL_Surface *downMessage  = NULL;
    SDL_Surface *leftMessage  = NULL;
    SDL_Surface *rightMessage = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {0, 0, 0};

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

    upMessage = TTF_RenderText_Blended(font,
            "Up was pressed."   , fontColor);

    downMessage = TTF_RenderText_Blended(font,
            "Down was pressed." , fontColor);

    leftMessage = TTF_RenderText_Blended(font,
            "Left was pressed." , fontColor);

    rightMessage = TTF_RenderText_Blended(font,
            "Right was pressed.", fontColor);

    draw_message(screen, NULL);

    SDL_Event event;

    for(;;)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    draw_message(screen, upMessage);    break;
                    case SDLK_DOWN:  draw_message(screen, downMessage);  break;
                    case SDLK_LEFT:  draw_message(screen, leftMessage);  break;
                    case SDLK_RIGHT: draw_message(screen, rightMessage); break;
                    default: break;
                }
            }

            else if (event.type == SDL_QUIT)
                break;
        }
    }

    SDL_FreeSurface(upMessage);
    SDL_FreeSurface(downMessage);
    SDL_FreeSurface(leftMessage);
    SDL_FreeSurface(rightMessage);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
