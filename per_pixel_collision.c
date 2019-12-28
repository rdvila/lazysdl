#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#define MAX_VECTOR_ITEMS 255

#define DOT_WIDTH  20
#define DOT_HEIGHT 20

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#ifndef FPS60
    #define FPS 23
#else
    #define FPS 60
#endif

#ifdef DEBUG
    #define PRINT_DEBUG(s) printf("%s\n", s);
#else
    #define PRINT_DEBUG(s)
#endif

typedef struct
{
    int count;
    SDL_Rect rects[MAX_VECTOR_ITEMS];

} vector;

typedef struct
{
    int x, y;
    int xVel, yVel;
    vector box;

} dot;

SDL_bool  check_collision(vector *a, vector *b);
SDL_Rect *vector_get_rect(vector *v, int index);
void vector_add(vector *v, SDL_Rect rect);
void vector_add_geometry(vector *v, int w, int h);
void dot_init(dot *d, int x , int y);
void dot_move(dot *d1, dot *d2);
void dot_shift_boxes(dot *d);

void vector_add(vector *v, SDL_Rect rect)
{
    if (v->count >= MAX_VECTOR_ITEMS -1)
    {
        fprintf(stderr, "vector out of range.");
        exit(EXIT_FAILURE);
    }

    v->rects[v->count] = rect;
    v->count += 1;
}

void vector_add_geometry(vector *v, int w, int h)
{
    SDL_Rect rect = {0, 0, w, h};
    vector_add(v, rect);
}

SDL_Rect *vector_get_rect(vector *v, int index)
{
    if (index >= v->count)
    {
        fprintf(stderr, "Index out of bound.");
        exit(EXIT_FAILURE);
    }

    return &v->rects[index];
}

void dot_shift_boxes(dot *d)
{
    PRINT_DEBUG("Entering dot_shift_box")

    int r = 0;
    int set;

    for (set = 0; set < d->box.count; set++)
    {
        SDL_Rect *rect = vector_get_rect(&d->box, set);

        rect->x = d->x + ((DOT_WIDTH - rect->w) / 2);
        rect->y = d->y + r;

        r += rect->h;
    }

    PRINT_DEBUG("Leaving dot_shift_box")
}

void dot_init(dot *d, int x , int y)
{
    d->x = x;
    d->y = y;

    d->yVel = 0;
    d->xVel = 0;

    vector_add_geometry(&d->box, 4 , 1);
    vector_add_geometry(&d->box, 10, 1);
    vector_add_geometry(&d->box, 12, 1);
    vector_add_geometry(&d->box, 14, 1);
    vector_add_geometry(&d->box, 16, 1);
    vector_add_geometry(&d->box, 18, 1);
    vector_add_geometry(&d->box, 18, 2);
    vector_add_geometry(&d->box, 20, 4);
    vector_add_geometry(&d->box, 18, 2);
    vector_add_geometry(&d->box, 18, 1);
    vector_add_geometry(&d->box, 16, 1);
    vector_add_geometry(&d->box, 14, 1);
    vector_add_geometry(&d->box, 12, 1);
    vector_add_geometry(&d->box, 10, 1);
    vector_add_geometry(&d->box, 4 , 1);

    dot_shift_boxes(d);
}

void dot_move(dot *d1, dot *d2)
{
    PRINT_DEBUG("Entering dot_move")

    d1->x += d1->xVel;
    d1->y += d1->yVel;

    int dW = DOT_WIDTH;
    int dH = DOT_HEIGHT;

    int sW = SCREEN_WIDTH;
    int sH = SCREEN_HEIGHT;

    dot_shift_boxes(d1);

    SDL_bool has_collision = check_collision(&d1->box, &d2->box);

    if ((d1->x < 0) || ((d1->x + dW) > sW) || has_collision)
    {
        d1->x -= d1->xVel;
        dot_shift_boxes(d1);
    }

    if ((d1->y < 0) || ((d1->y + dH) > sH) || has_collision)
    {
        d1->y -= d1->yVel;
        dot_shift_boxes(d1);
    }

    PRINT_DEBUG("Leaving dot_move")
}

SDL_bool check_collision(vector *a, vector *b)
{
    int lA, lB, rA, rB;
    int tA, tB, bA, bB;

    int aBox, bBox;

    for (aBox = 0; aBox < a->count; aBox++)
    {
        SDL_Rect *rect1 =  vector_get_rect(a, aBox);

        lA = rect1->x;
        rA = rect1->x + rect1->w;
        tA = rect1->y;
        bA = rect1->y + rect1->h;

        for (bBox = 0; bBox < b->count; bBox++)
        {
            SDL_Rect *rect2 =  vector_get_rect(b, bBox);

            lB = rect2->x;
            rB = rect2->x + rect2->w;
            tB = rect2->y;
            bB = rect2->y + rect2->h;

            if (!(bA <= tB || tA >= bB || rA <= lB || lA >= rB))
            {
                return SDL_TRUE;
            }
        }
    }

    return SDL_FALSE;
}

/*-------*/

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

    optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
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

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Surface *circle1;

    dot dot1 = {0};
    dot dot2 = {0};

    dot_init(&dot1, 50, 50);
    dot_init(&dot2, 10, 10);

    Uint32 start = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        fprintf(stderr, "Cannot init everything");
        exit(EXIT_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_SWSURFACE);

    if (!screen)
    {
        fprintf(stderr, "Cannot set video mode");
        exit(EXIT_FAILURE);
    }

    circle1 = load_image("circle.png");

    SDL_Event event = {0};
    SDL_bool  quit  = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {

                PRINT_DEBUG("Entering event.type == SDL_KEYDOWN");

                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    dot1.yVel -= 1; break;
                    case SDLK_DOWN:  dot1.yVel += 1; break;
                    case SDLK_LEFT:  dot1.xVel -= 1; break;
                    case SDLK_RIGHT: dot1.xVel += 1; break;
                    default: break;
                }

                PRINT_DEBUG("Leaving event.type == SDL_KEYDOWN");
            }
            else if (event.type == SDL_KEYUP)
            {

                PRINT_DEBUG("Entering event.type == SDL_KEYUP");

                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    dot1.yVel += 1; break;
                    case SDLK_DOWN:  dot1.yVel -= 1; break;
                    case SDLK_LEFT:  dot1.xVel += 1; break;
                    case SDLK_RIGHT: dot1.xVel -= 1; break;
                    default: break;
                }

                PRINT_DEBUG("Entering event.type == SDL_KEYUP");
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;

        }

        dot_move(&dot1, &dot2);

        SDL_FillRect(screen, &screen->clip_rect,
                SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

        apply_surface(dot1.x, dot1.y, circle1, screen);
        apply_surface(dot2.x, dot2.y, circle1, screen);

        if (SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Cannot flip screen.");
            exit(EXIT_FAILURE);
        }

        Uint32 currticks = SDL_GetTicks();

        if ((currticks - start) < (1000/ FPS))
            SDL_Delay((1000 / FPS) - (currticks - start));
    }

    SDL_FreeSurface(circle1);
    SDL_Quit();

    return 0;
}

