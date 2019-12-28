#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

SDL_Surface *screen = NULL;
SDL_Surface *text[5] = {NULL, NULL, NULL, NULL, NULL};

struct control
{
    SDL_Thread *thread_a;
    SDL_Thread *thread_b;
    SDL_sem  *video_lock;

} mycontrol = {NULL, NULL, NULL};

void show_surface(int x, int y, SDL_Surface *src)
{
    /*LOCK*/
    SDL_SemWait(mycontrol.video_lock);

    SDL_Rect offset;
    offset.x = x;
    offset.y = y;

    if (SDL_BlitSurface(src, NULL, screen, &offset) == -1)
    {
        fprintf(stderr, "Cannot blit surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Cannot flip screen: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /*UNLOCK*/
    SDL_SemPost(mycontrol.video_lock);
    
}

int blitter_a(void *data)
{
    int i, y = 10;

    for (i=0; i < 5; i++)
    {
        SDL_Delay(200);
        show_surface(((SCREEN_WIDTH / 2) - text[i]->w) / 2, y, text[i]);
        y += 100;
    }

    return 0;
}

int blitter_b(void *data)
{
    int i, y = 10;

    for (i=0; i < 5; i++)
    {
        SDL_Delay(200);
        show_surface((SCREEN_WIDTH / 2) + (((SCREEN_WIDTH / 2) - text[i]->w)
                / 2), y, text[i]);

        y += 100;
    }

    return 0;
}

void safe_exit()
{
    SDL_DestroySemaphore(mycontrol.video_lock);

    /*SDL_FreeSurface(text[0]);
    SDL_FreeSurface(text[1]);
    SDL_FreeSurface(text[2]);
    SDL_FreeSurface(text[3]);
    SDL_FreeSurface(text[4]);*/

    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char **argv)
{
    TTF_Font  *font = NULL;
    SDL_Color color = {0xff, 0xff, 0xff};

    atexit(safe_exit);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)    
    {
        fprintf(stderr, "Cannot init everything: %s\n", SDL_GetError());
        exit(EXIT_FAILURE); 
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Cannot init ttf: %s\n", SDL_GetError());
        exit(EXIT_FAILURE); 
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE); 
    }
    SDL_WM_SetCaption("Testing Threads.", NULL);

    font = TTF_OpenFont("LinLibertine.ttf", 72);
    if (!font)
    {
        fprintf(stderr, "Cannot open font: %s\n", SDL_GetError());
        exit(EXIT_FAILURE); 
    }

    text[0] = TTF_RenderText_Solid(font, "1", color);
    text[1] = TTF_RenderText_Solid(font, "2", color);
    text[2] = TTF_RenderText_Solid(font, "3", color);
    text[3] = TTF_RenderText_Solid(font, "4", color);
    text[4] = TTF_RenderText_Solid(font, "5", color);
    TTF_CloseFont(font);
    
    mycontrol.video_lock = SDL_CreateSemaphore(1);

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    mycontrol.thread_a = SDL_CreateThread(blitter_a, NULL);
    mycontrol.thread_b = SDL_CreateThread(blitter_b, NULL);

    SDL_WaitThread(mycontrol.thread_a, NULL);
    SDL_WaitThread(mycontrol.thread_b, NULL);

    while (!quit)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        SDL_Delay(1000 / 20); 
    }
    
    safe_exit();
    return 0;
}


