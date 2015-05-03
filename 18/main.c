#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ANSI 1
#define SIDE 64

static const size_t world_size = SIDE * SIDE;
static const char colors[8] = " .,:;!@#";

static void
print_world_ansi(const uint64_t *world)
{
    static const char *ants = ">^<v";
    char path[256];
    uint8_t state;
    uint64_t p, x, y;

    printf("\033[2J\033[;H");
    p = 0;
    for (y = 0; y < SIDE; ++y)
    {
        for (x = 0; x < SIDE; ++x)
        {
            state = world[SIDE * y + x];
            putchar(colors[state % 8]);
            ++p;
        }

        printf("\033[0m\n");
    }
}

int
main()
{
    uint64_t *world;
    uint64_t state, x, y;

    if (!(world = calloc(1, world_size)))
        errx(EXIT_FAILURE, "cannot allocate memory");

    x = SIDE / 2;
    y = SIDE / 2;

    world[0] = 5;

    for (;;)
    {
        print_world_ansi(world);

        usleep(100000);

        for (y = 1; y < SIDE-1; ++y)
        {
            for (x = 1; x < SIDE-1; ++x)
            {
                state = world[SIDE * y + x];

                state += world[SIDE * (y - 1) + (x - 1)];
                state -= world[SIDE * (y - 1) + (x - 0)];
                state += world[SIDE * (y - 1) + (x + 1)];

                //state += 1;

                world[SIDE * y + x] = state;
            }
        }
    }

    return EXIT_SUCCESS;
}
