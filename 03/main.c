#include <err.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

float *samples;
size_t numsamples;

void
read_samples()
{
    size_t numallocs;
    float s;

    numallocs = 0;
    numsamples = 0;
    samples = NULL;

    for (;;)
    {
        if (EOF == scanf("%f\n", &s))
            break;

        ++numsamples;

        if (numallocs < numsamples)
        {
            if (!(samples = realloc(samples,
                                    (numallocs + 1024) * sizeof(samples[0]))))
                err(EXIT_FAILURE, "realloc");
        }

        samples[numsamples - 1] = s;
    }
}

int
main()
{
    size_t numpoints;
    SDL_Event e;
    SDL_Point *points;
    SDL_Renderer *renderer;
    SDL_Window *window;
    int i, sw, sh;

    read_samples();

    numpoints = numsamples;

    if (!(points = calloc(numpoints, sizeof(SDL_Point))))
        err(EXIT_FAILURE, "calloc");

    if (SDL_Init(SDL_INIT_VIDEO))
        errx(EXIT_FAILURE, "SDL_Init");

    atexit(SDL_Quit);

    sw = 100;
    sh = 100;

    window =
        SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         sw, sh, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    for (;;)
    {
        if (SDL_PollEvent(&e))
        {
            if (SDL_QUIT == e.type)
                break;

            if (SDL_KEYDOWN == e.type)
                if (SDLK_ESCAPE == e.key.keysym.sym)
                    break;

            if (SDL_WINDOWEVENT == e.type)
                if (SDL_WINDOWEVENT_RESIZED == e.window.event)
                {
                    sw = e.window.data1;
                    sh = e.window.data2;
                }
        }

        for (i = 0; i < numpoints; ++i)
        {
            float x, y;

            x = (float)i / (numpoints - 1);
            y = samples[i];

            points[i].x = sw * x;
            points[i].y = (sh - (sh * y)) / 2;
        }

        SDL_RenderClear(renderer);
        SDL_RenderDrawLines(renderer, points, numpoints);
        SDL_RenderPresent(renderer);
    }

    return EXIT_SUCCESS;
}
