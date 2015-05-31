#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    char *ks;
    size_t kl;
    int ki;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s key\n", argv[0]);
        return EXIT_FAILURE;
    }

    ks = argv[1];
    kl = strlen(ks);
    ki = 0;

    for (;;)
    {
        char ic, kc, oc;
        int r;

        r = getchar();

        if (EOF == r)
            break;

        ic = r;
        kc = ks[ki];
        oc = ic ^ kc;

        putchar(oc);

        ki = (ki + 1) % kl;
    }

    return EXIT_SUCCESS;
}
