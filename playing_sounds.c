#include <stdio.h>
#include <stdlib.h>

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

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        fprintf(stderr, "Cannot init mixer.");
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("Playing Sounds", NULL);
}

void draw_message(int y, SDL_Surface *screen, SDL_Surface *message)
{
    apply_surface((SCREEN_WIDTH - message->w) / 2,
            ((SCREEN_HEIGHT - message->h) / 2) + y,
            message, screen);

}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;

    SDL_Surface *message1  = NULL;
    SDL_Surface *message2  = NULL;
    SDL_Surface *message3  = NULL;

    TTF_Font *font = NULL;
    SDL_Color fontColor = {0, 0, 0};

    Mix_Music *music = NULL;

    Mix_Chunk *scratch = NULL;
    Mix_Chunk *high    = NULL;
    Mix_Chunk *med     = NULL;
    Mix_Chunk *low     = NULL;

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
            "Press 1, 2, 3 or 4 to play a sound effect.", fontColor);

    message2 = TTF_RenderText_Blended(font,
            "Press 9 to play or pause the music." , fontColor);

    message3 = TTF_RenderText_Blended(font,
            "Press 0 to stop the music." , fontColor);

    music = Mix_LoadMUS("beat.wav");

    if (!music)
    {
        fprintf(stderr, "Cannot load music.");
        exit(EXIT_FAILURE);
    }

    scratch = Mix_LoadWAV("scratch.wav");
    high    = Mix_LoadWAV("high.wav");
    med     = Mix_LoadWAV("medium.wav");
    low     = Mix_LoadWAV("low.wav");

    if (!scratch || !high || !med || !low)
    {
        fprintf(stderr, "Cannot load chunks.");
        exit(EXIT_FAILURE);
    }

    SDL_FillRect(screen, &screen->clip_rect,
        SDL_MapRGB(screen->format, 0xff, 0xff, 0xff ));

    draw_message(-30, screen, message1);
    draw_message(0  , screen, message2);
    draw_message(30 , screen, message3);

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen.");
        exit(EXIT_FAILURE);
    }

    SDL_Event event;

    for(;;)
    {
        if (SDL_PollEvent(&event))
        {
            int error = 0;

            if (event.key.keysym.sym == SDLK_1)
                error = Mix_PlayChannel(-1, scratch, 0);

            else if (event.key.keysym.sym == SDLK_2)
                error = Mix_PlayChannel(-1, high, 0);

            else if (event.key.keysym.sym == SDLK_3)
                error = Mix_PlayChannel(-1, med, 0);

            else if (event.key.keysym.sym == SDLK_4)
                error = Mix_PlayChannel(-1, low, 0);

            else if (event.key.keysym.sym == SDLK_9)
            {
                if (Mix_PlayingMusic() == 0)
                    error = Mix_PlayMusic(music, -1);
                else
                {
                    if (Mix_PausedMusic() == 1)
                        Mix_ResumeMusic();
                    else
                        Mix_PauseMusic();
                }
            }
            else if (event.key.keysym.sym == SDLK_0)
                Mix_HaltMusic();


            else if (event.type == SDL_QUIT)
                break;

            if (error == -1)
            {
                fprintf(stderr, "Cannot do anything =(.");
                exit(EXIT_FAILURE);
            }
        }

    }

    SDL_FreeSurface(message1);
    SDL_FreeSurface(message2);
    SDL_FreeSurface(message3);

    Mix_FreeChunk(scratch);
    Mix_FreeChunk(high);
    Mix_FreeChunk(med);
    Mix_FreeChunk(low);

    Mix_FreeMusic(music);

    TTF_CloseFont(font);

    Mix_CloseAudio();

    TTF_Quit();
    SDL_Quit();

    return 0;
}
