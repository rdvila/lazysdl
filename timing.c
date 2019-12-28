#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

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

    SDL_WM_SetCaption("Timing", NULL);
}

void draw_message( SDL_Surface *screen, SDL_Surface *message1,
        SDL_Surface *message2)
{
    SDL_FillRect(screen, &screen->clip_rect,
        SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    apply_surface((SCREEN_WIDTH - message1->w) / 2,
            ((SCREEN_HEIGHT - message1->h) / 2) + 30,
            message1, screen);

    if (message2)
    {
        apply_surface((SCREEN_WIDTH - message2->w) / 2,
                ((SCREEN_HEIGHT - message2->h) / 2) - 30,
                message2, screen);
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

    SDL_Surface *message1  = NULL;
    SDL_Surface *message2  = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {0, 0, 0};

    SDL_bool running = SDL_TRUE;
    Uint32 start = 0;

    init();

    start = SDL_GetTicks();

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
            "Press s to start or stop the timer.", fontColor);

    draw_message(screen, message1, NULL);

    SDL_Event event;

    for(;;)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_s)
                {
                    if (running)
                    {
                        #if 0
                        char msgticks[30] = "timer = ";
                        char buf[20];

                        itoa((SDL_GetTicks - start), buf, 10);
                        strcat(msgticks, buf);
                        #endif

                        char msgticks[30];
                        snprintf(msgticks, 10, "Timer = %i",
                                (SDL_GetTicks() - start));

                        message2 = TTF_RenderText_Blended(font,
                                msgticks , fontColor);

                        draw_message(screen, message1, message2);

                        running = SDL_FALSE;
                        start = 0;
                    }
                    else
                    {
                        draw_message(screen, message1, NULL);

                        running = SDL_TRUE;
                        start = SDL_GetTicks();
                    }
                }
            }
            else if (event.type == SDL_QUIT)
                break;

        }

    }

    SDL_FreeSurface(message1);
    SDL_FreeSurface(message2);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
