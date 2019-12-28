#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_opengl.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     32

#define FPS 60

#define SQUARE_WIDTH  20
#define SQUARE_HEIGHT 20

typedef struct
{
    int x, y;
    int xVel, yVel;

} square;

void init_GL()
{
    glClearColor(0, 0, 0, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (glGetError() != GL_NO_ERROR)
        exit(EXIT_FAILURE);
}

void square_move(square *s)
{
    s->x += s->xVel;
    s->y += s->yVel;

    if ((s->x < 0) || (s->x + SQUARE_WIDTH > SCREEN_WIDTH))
        s->x -= s->xVel;
    
    if ((s->y < 0) || (s->y + SQUARE_HEIGHT > SCREEN_HEIGHT))
        s->y -= s->yVel;
}

void square_show(square *s)
{
    glTranslatef(s->x, s->y, 0);

    glBegin(GL_QUADS);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glVertex3f(0           ,             0, 0);
        glVertex3f(SQUARE_WIDTH,             0, 0);
        glVertex3f(SQUARE_WIDTH, SQUARE_HEIGHT, 0);
        glVertex3f(0           , SQUARE_HEIGHT, 0);
    glEnd();

    glLoadIdentity();
}


int main(int argc, char **argv)
{
    atexit(SDL_Quit);

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Could not initialize everything: %s", SDL_GetError());
        //exit(EXIT_FAILURE);
    }

    if (SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
            SDL_OPENGL) == NULL)
    {
        fprintf(stderr, "Could not set video mode: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    init_GL();

    SDL_WM_SetCaption("OpenGL Teste.", NULL);

    square mysquare = {0};
    Uint32 start = 0;

    SDL_Event event;
    SDL_bool  quit = SDL_FALSE;

    while (!quit)
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mysquare.yVel -= SQUARE_HEIGHT / 2;break;
                    case SDLK_DOWN:  mysquare.yVel += SQUARE_HEIGHT / 2;break;
                    case SDLK_LEFT:  mysquare.xVel -= SQUARE_WIDTH  / 2;break;
                    case SDLK_RIGHT: mysquare.xVel += SQUARE_WIDTH  / 2;break;
                    default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:    mysquare.yVel += SQUARE_HEIGHT / 2;break;
                    case SDLK_DOWN:  mysquare.yVel -= SQUARE_HEIGHT / 2;break;
                    case SDLK_LEFT:  mysquare.xVel += SQUARE_WIDTH  / 2;break;
                    case SDLK_RIGHT: mysquare.xVel -= SQUARE_WIDTH  / 2;break;
                    default: break;
                }
            }
            else if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
        }

        square_move(&mysquare);
 
        glClear(GL_COLOR_BUFFER_BIT);

        square_show(&mysquare);

        SDL_GL_SwapBuffers();

        Uint32 currticks = SDL_GetTicks();
        if ((currticks - start) < (1000 / FPS))
        {
            SDL_Delay((1000 / FPS) - (currticks - start));
        }
    }

    SDL_Quit();
    return 0;
}

