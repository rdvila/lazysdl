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

void draw_message(SDL_Surface *screen, SDL_Surface *message, SDL_Rect *rect)
{


    SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    SDL_FillRect(screen, rect,
            SDL_MapRGB(screen->format, 0xff, 0x0, 0x0 ));

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

unsigned int is_over(int x, int y, SDL_Rect *rect)
{
    return ((x > rect->x) && (x < (rect->x + rect->w))
            && (y > rect->y) && (y < (rect->y + rect->h)));
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;

    SDL_Surface *overMessage = NULL;
    SDL_Surface *outMessage  = NULL;
    SDL_Surface *upMessage   = NULL;
    SDL_Surface *downMessage = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {0, 0, 0};

    SDL_Rect rect = {80, 80, 480, 320};

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

    overMessage = TTF_RenderText_Blended(font,
            "Mouse over."   , fontColor);

    outMessage = TTF_RenderText_Blended(font,
            "Mouse out." , fontColor);

    upMessage = TTF_RenderText_Blended(font,
            "Mouse up." , fontColor);

    downMessage = TTF_RenderText_Blended(font,
            "Mouse down.", fontColor);

    draw_message(screen, NULL, &rect);

    SDL_Event event;

    for(;;)
    {
        if (SDL_PollEvent(&event))
        {
            int x = 0;
            int y = 0;

            if (event.type == SDL_MOUSEMOTION)
            {
                x = event.motion.x;
                y = event.motion.y;

                #ifdef VERBOSE
                    printf("x = %i, y = %i\n", x, y);
                    printf("\n");
                #endif

                if (is_over(x, y, &rect))
                    draw_message(screen, overMessage, &rect);
                else
                    draw_message(screen, outMessage, &rect);
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                draw_message(screen, downMessage, &rect);
            }

            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                draw_message(screen, upMessage, &rect);
            }

            else if (event.type == SDL_QUIT)
                break;
        }
    }

    SDL_FreeSurface(upMessage);
    SDL_FreeSurface(downMessage);
    SDL_FreeSurface(overMessage);
    SDL_FreeSurface(outMessage);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
