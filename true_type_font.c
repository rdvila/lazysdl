#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

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

    optimizedImage = SDL_DisplayFormat(loadedImage);
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

    SDL_WM_SetCaption("TTF teste", NULL);
}

int main(int argc, char **argv)
{
    SDL_Surface *background = NULL;
    SDL_Surface *message    = NULL;
    SDL_Surface *screen     = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {255, 255, 255};

    init();

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set screen.");
        exit(EXIT_FAILURE);
    }

    background = load_image("background2.png");
    font = TTF_OpenFont("LinLibertine.ttf", 30);

    if (!font)
    {
        fprintf(stderr, "Cannot load font.");
        exit(EXIT_FAILURE);
    }

    message = TTF_RenderText_Blended(font,
            "The quick brown fox jumps over the lazy dog", fontColor);

    apply_surface(0, 0, background, screen);
    apply_surface(10, 228, message, screen);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }

    SDL_Delay(2000);

    SDL_FreeSurface(background);
    SDL_FreeSurface(message);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
