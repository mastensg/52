plot
====

This program plots a list of numbers [-1,1].

Building
--------

You need SDL2.

    autoreconf -i
    ./configure
    make

Running
-------

    seq 1000 | awk '{print sin(0.01*$1)}' | ./plot
