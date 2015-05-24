#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>

int
main(void)
{
    int x, y, dx, dy;
    int b, numbits;
    unsigned char octet;
    Display *dpy;
    Window root;

    if (NULL == (dpy = XOpenDisplay(NULL)))
        errx(EXIT_FAILURE, "Cannot open display");

    root = DefaultRootWindow(dpy);

    x = 0;
    y = 0;

    octet = 0;
    numbits = 0;

    for (;;)
    {
        int rx, ry, wx, wy, m;
        Window wr, wc;

        usleep(1000);

        XQueryPointer(dpy, root, &wr, &wc, &rx, &ry, &wx, &wy, &m);
        dx = rx - x;
        dy = ry - y;

        if (!dx && !dy)
            continue;

        x = rx;
        y = ry;

        b = (dx * dy) & 3;
        octet = (octet << 2) | b;
        ++numbits;

        if (numbits < 8)
            continue;

        fprintf(stdout, "%c", octet);
        fflush(stdout);

        numbits = 0;
        octet = 0;
    }

    return EXIT_SUCCESS;
}
