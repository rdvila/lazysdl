#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define TOTAL_PARTICLES 20

#define DOT_WIDTH  20
#define DOT_HEIGHT 20

#define FPS 20

#define LIFE_PARTICLES 10

#define QTD_TYPES 3

void apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dest,
     SDL_Rect *clip);

typedef struct
{
    int x, y;
    int frame;
    SDL_Surface *type;

} particle;

typedef struct
{
    int x, y;
    int xVel, yVel;
    particle *particles[TOTAL_PARTICLES];

} dot;

void init()
{

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("Particle Test", NULL);

    srand(SDL_GetTicks());
}

particle *particle_create(int x, int y, SDL_Surface *type)
{
    particle *p = malloc(sizeof(particle));
    p->x = x - 5 + (rand() % 25);
    p->y = y - 5 + (rand() % 25);

    p->frame = rand() % 5;
    p->type = type;

    return p;
}

void particle_repair(particle *p, int x, int y, SDL_Surface *type)
{
    p->x = x - 5 + (rand() % 25);
    p->y = y - 5 + (rand() % 25);

    p->frame = rand() % 5;
    p->type = type;
}

void particle_show(particle *p, SDL_Surface *screen, SDL_Surface *white)
{
    if ((p->frame % 2) == 0)
        apply_surface(p->x, p->y, white, screen, NULL);
    else
        apply_surface(p->x, p->y, p->type, screen, NULL);

    p->frame+=1;

}

dot *dot_create(SDL_Surface **types)
{
    dot *d = malloc(sizeof(dot));

    d->x = 0;
    d->y = 0;

    d->xVel = 0;
    d->yVel = 0;

    int p;
    for (p = 0; p < TOTAL_PARTICLES; p++)
    {
        d->particles[p] = particle_create(d->x, d-> y,
                types[rand() % QTD_TYPES]);
    }

    return d;
}

void dot_destroy(dot *d)
{
    int p;
    for (p=0; p < TOTAL_PARTICLES; p++)
    {
        printf("free particle numeber %i, at x=%i, y=%i\n", p,
            d->particles[p]->x, d->particles[p]->y);
        free(d->particles[p]);
    }

    free(d);
}

void dot_show_particles(dot *d, SDL_Surface *screen,SDL_Surface **types,
        SDL_Surface *white)
{
    int p;
    for (p = 0; p < TOTAL_PARTICLES; p++)
    {
        if (d->particles[p]->frame >= LIFE_PARTICLES)
        {
            /*free(d->particles[p]);
            d->particles[p] = particle_create(d->x, d->y,
                    types[rand() % QTD_TYPES]);*/
            particle_repair(d->particles[p], d->x, d->y,
                    types[rand() % QTD_TYPES]);

        }
    }

    for (p = 0; p < TOTAL_PARTICLES; p++)
    {
        particle_show(d->particles[p], screen, white);
    }
}

void dot_move(dot *d)
{
    d->x += d->xVel;
    d->y += d->yVel;

    if ((d->x < 0) || (d->x + DOT_WIDTH > SCREEN_WIDTH))
        d->x -= d->xVel;

    if ((d->y < 0) || (d->y + DOT_HEIGHT > SCREEN_HEIGHT))
        d->y -= d->yVel;
}

SDL_Surface *load_image(const char *name)
{
    SDL_Surface *loadedImage    = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load(name);

    if (!loadedImage)
    {
        fprintf(stderr, "Cannot load image %s: %s", name, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (!optimizedImage)
    {
        fprintf(stderr, "Cannot optimize image: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY,
            SDL_MapRGB(optimizedImage->format, 0xff, 0x0, 0xff));

    return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dest,
        SDL_Rect *clip)
{
    SDL_Rect offset = {0};

    offset.x = x;
    offset.y = y;

    SDL_BlitSurface(src, clip, dest, &offset);
}

int main(int argc, char **argv)
{

    SDL_Surface *screen = NULL;
    SDL_Surface *circle = NULL;
    SDL_Surface *red    = NULL;
    SDL_Surface *green  = NULL;
    SDL_Surface *blue   = NULL;
    SDL_Surface *white  = NULL;

    dot *mydot = NULL;

    Uint32 start = 0;

    atexit(SDL_Quit);
    init();

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    circle = load_image("circle2.bmp");
    red    = load_image("red.bmp");
    green  = load_image("green.bmp");
    blue   = load_image("blue.bmp");
    white  = load_image("white.bmp");

    SDL_SetAlpha(red  , SDL_SRCALPHA | SDL_RLEACCEL, 192);
    SDL_SetAlpha(green, SDL_SRCALPHA | SDL_RLEACCEL, 192);
    SDL_SetAlpha(blue , SDL_SRCALPHA | SDL_RLEACCEL, 192);
    SDL_SetAlpha(white, SDL_SRCALPHA | SDL_RLEACCEL, 192);

    SDL_Surface *types[QTD_TYPES] = {red, green, blue};

    mydot = dot_create(types);

    SDL_Event event;
    SDL_bool  quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot->yVel -= DOT_HEIGHT / 2; break;
                    case SDLK_DOWN:  mydot->yVel += DOT_HEIGHT / 2; break;
                    case SDLK_LEFT:  mydot->xVel -= DOT_WIDTH / 2; break;
                    case SDLK_RIGHT: mydot->xVel += DOT_WIDTH / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:    mydot->yVel += DOT_HEIGHT / 2; break;
                    case SDLK_DOWN:  mydot->yVel -= DOT_HEIGHT / 2; break;
                    case SDLK_LEFT:  mydot->xVel += DOT_WIDTH / 2; break;
                    case SDLK_RIGHT: mydot->xVel -= DOT_WIDTH / 2; break;
                    default: break;
                }
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        SDL_FillRect(screen, &screen->clip_rect,
            SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

        dot_move(mydot);
        apply_surface(mydot->x, mydot->y, circle, screen, NULL);

        dot_show_particles(mydot, screen, types, white);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen: %s", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        Uint32 currTicks = SDL_GetTicks();

        if ((currTicks - start) < (1000 / FPS))
        {
            SDL_Delay((1000 / FPS) - (currTicks - start));
        }
    }

    dot_destroy(mydot);

    SDL_FreeSurface(circle);
    SDL_FreeSurface(red);
    SDL_FreeSurface(green);
    SDL_FreeSurface(blue);
    SDL_FreeSurface(white);

    SDL_Quit();
    return 0;
}
