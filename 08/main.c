#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ANSI 1
#define SIDE 32

typedef enum
{
    RIGHT,
    UP,
    LEFT,
    DOWN,
} dir_t;

static const size_t world_size = SIDE * SIDE / 32 * sizeof(uint64_t);

static void
print_world_ansi(const uint64_t *world, uint64_t iterations, uint64_t antp,
                 dir_t antd)
{
    static const char *ants = ">^<v";
    char path[256];
    uint8_t state;
    uint64_t p, x1, x2, y;

    printf("\033[2J\033[;H%12lu\n", iterations);
    p = 0;
    for (y = 0; y < SIDE; ++y)
    {
        for (x1 = 0; x1 < SIDE / 32; ++x1)
        {
            for (x2 = 0; x2 < 32; ++x2)
            {
                state = (world[p >> 5] >> (2 * (p & 31))) & 3;
                printf("\033[4%dm%c", state + 0, antp == p ? ants[antd] : ' ');
                ++p;
            }
        }

        printf("\033[0m\n");
    }
}

static void
print_world_pgm(const uint64_t *world, uint64_t iterations)
{
    char path[256];
    FILE *f;
    uint8_t s;
    uint64_t b, c, x1, x2, y;

    sprintf(path, "langton_%04luB.pgm", iterations);

    fprintf(stderr, "%s\n", path);

    f = fopen(path, "w");

    fprintf(f, "P2\n%d %d\n4\n", SIDE, SIDE);

    c = 0;
    for (y = 0; y < SIDE; ++y)
    {
        for (x1 = 0; x1 < SIDE / 32; ++x1)
        {
            b = world[c++];

            for (x2 = 0; x2 < 32; ++x2)
            {
                s = (b >> (2 * x2)) & 3;
                fprintf(f, "%2d", s);
            }
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

static void
print_world_raw(const uint64_t *world, uint64_t iterations)
{
    char path[256];
    FILE *f;
    uint8_t s;
    uint64_t b, c, x1, x2, y;

    sprintf(path, "langton_%04luB.raw", iterations);

    fprintf(stderr, "%s\n", path);

    f = fopen(path, "w");

    fwrite(world, world_size, 1, f);

    fclose(f);
}

int
main()
{
    uint64_t *world;
    uint64_t i, p, state;
    dir_t dir;
    int16_t dd, dp;

    if (!(world = calloc(1, world_size)))
        errx(EXIT_FAILURE, "cannot allocate memory");

    p = (SIDE * SIDE / 2 + SIDE / 2);

    dir = RIGHT;

    for (i = 0;; ++i)
    {
#if ANSI
        print_world_ansi(world, i, p, dir);
        // getchar();
        usleep(100000);
#else
        if (0 == i % 1000000000ul)
        {
            print_world_pgm(world, i / 1000000000ul);
        }
#endif

        // forward
        switch (dir)
        {
        case RIGHT:
            dp = 1;
            break;
        case UP:
            dp = -SIDE;
            break;
        case LEFT:
            dp = -1;
            break;
        case DOWN:
            dp = SIDE;
            break;
        }

        p += dp;

        state = (world[p >> 5] >> (2 * (p & 31))) & 3;

        // turn
        // RLR
        switch (state)
        {
        case 0:
            dd = -1;
            state = 1;
            break;
        case 1:
            dd = 1;
            state = 2;
            break;
        case 2:
            dd = -1;
            state = 3;
            break;
        case 3:
            dd = 1;
            state = 1;
            break;
        }
        /*
        // LLRR
        switch (state)
        {
        case 0:
            dd = 1;
            state = 1;
            break;
        case 1:
            dd = 1;
            state = 2;
            break;
        case 2:
            dd = -1;
            state = 3;
            break;
        case 3:
            dd = -1;
            state = 0;
            break;
        }
        */

        dir = (dir + dd) & 3;

        // update cell state
        world[p >> 5] &= ~(3ULL << (2 * (p & 31)));
        world[p >> 5] |= (state << (2 * (p & 31)));
    }

    return EXIT_SUCCESS;
}
