#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

SDL_Surface *images[5] = {NULL, NULL, NULL, NULL, NULL};
SDL_Surface *buffer = NULL;

SDL_Thread *producerThread = NULL;
SDL_Thread *consumerThread = NULL;

SDL_mutex *bufferLock = NULL;

SDL_cond *canProduce = NULL;
SDL_cond *canConsume = NULL;

SDL_Surface *screen = NULL;

void clean_up()
{
    SDL_DestroyMutex(bufferLock);

    SDL_DestroyCond(canProduce);
    SDL_DestroyCond(canConsume);

    SDL_Quit();
}

void init()
{
    atexit(clean_up);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Could not initialize everything: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Could not set video mode: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    bufferLock = SDL_CreateMutex();
    canProduce = SDL_CreateCond();
    canConsume = SDL_CreateCond();
}

void apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dest)
{
    SDL_Rect offset = {0};
    offset.x = x;
    offset.y = y;

    if (SDL_BlitSurface(src, NULL, dest, &offset) == -1)
    {
        fprintf(stderr, "Could not blit surface: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

}

void produce(int x, int y);
void consume(int x, int y);

int producer(void *data)
{
    int y = 10, p;
    srand(SDL_GetTicks());
    
    for (p = 0; p < 5; p++)
    {
        SDL_Delay(rand() % 1000);
        produce(10, y);
        y += 90;
    }

    return 0;
}

int consumer(void *data)
{
    int y = 10, p;
    srand(SDL_GetTicks());
    
    for (p = 0; p < 5; p++)
    {
        SDL_Delay(rand() % 1000);
        consume(330, y);
        y += 90;
    }

    return 0;
}

void produce(int x, int y)
{
    SDL_mutexP(bufferLock);

    if (buffer)
    {
        SDL_CondWait(canProduce, bufferLock);
    }

    buffer = images[rand() % 5];
    apply_surface(x, y, buffer, screen);
    
    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr,"Could not flip screen: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_mutexV(bufferLock);
    SDL_CondSignal(canConsume);
}

void consume(int x, int y)
{
    SDL_mutexP(bufferLock);

    if (!buffer)
    {
        SDL_CondWait(canConsume, bufferLock);
    }

    apply_surface(x, y, buffer, screen);
    buffer = NULL;
    
    if (SDL_Flip(screen) == -1)
    {
        fprintf(stderr, "Could not flip screen: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_mutexV(bufferLock);
    SDL_CondSignal(canProduce);
}

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load(name);
    if(!loadedImage)
    {
        fprintf(stderr, "Could not load image %s: %s\n", name, SDL_GetError());
        exit(EXIT_FAILURE);
    } 
    
    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (!optimizedImage)
    {
        fprintf(stderr, "Could not optimize image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY,
            SDL_MapRGB(optimizedImage->format, 0x0, 0xff, 0xff));

    return optimizedImage;
}

int main(int argc, char **argv)
{
    SDL_bool quit = SDL_FALSE;
    init();

    images[0] = load_image("1.png");
    images[1] = load_image("2.png");
    images[2] = load_image("3.png");
    images[3] = load_image("4.png");
    images[4] = load_image("5.png");

    SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0x80, 0x80, 0x80));

    producerThread = SDL_CreateThread(producer, NULL);
    consumerThread = SDL_CreateThread(consumer, NULL);

    SDL_Event event;

    while (!quit)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }
    }

    clean_up();
    return 0;
}

