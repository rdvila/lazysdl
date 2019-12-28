#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32
#define FPS 20

typedef struct
{
    int cursor;
    char *str;
    SDL_Surface *text;

} string_input;

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
    SDL_Surface *message;
    SDL_Surface *name = NULL;

    TTF_Font *font;
    SDL_Color fontcolor = {0, 0, 0};

    Uint32 start = 0;

    string_input strin = {0};
    strin.str = calloc(16,sizeof(char));
    memset(strin.str, 0, 16);

    printf("%s\n", strin.str);

    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Cannot init ttf: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_EnableUNICODE(SDL_ENABLE);

    font = TTF_OpenFont("LinLibertine.ttf", 30);

    if (!font)
    {
        fprintf(stderr, "Cannot load font: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    message = TTF_RenderText_Blended(font,
            "New high score! Enter name:", fontcolor);

    name = TTF_RenderText_Blended(font,
            strin.str, fontcolor);

    SDL_Event event;
    SDL_bool  quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            int len = strlen(strin.str);

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE)
                {
                    if (len > 0)
                        memset(strin.str + len -1, 0, 1);

                }

                else if (len < 16)
                {
                    char c = (char)event.key.keysym.unicode;
                    memcpy(strin.str + len, &c, 1);
                }


                if (strin.str)
                {
                    SDL_FreeSurface(name);
                    name = TTF_RenderText_Blended(font, strin.str, fontcolor);
                }
            }

            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }


        SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        apply_surface((SCREEN_WIDTH - message->w) / 2, 20, message, screen);

        if (name)
        {
            apply_surface((SCREEN_WIDTH - name->w) / 2,
                        (SCREEN_HEIGHT - name->h) / 2, name, screen);
        }

       if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();
        if ((currticks - start) < 1000 / FPS)
            SDL_Delay((1000 / FPS) - (currticks - start));
    }

    free(strin.str);

    SDL_FreeSurface(message);
    SDL_FreeSurface(name);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
