#!/bin/sh

gcc -O3 -flto -funroll-loops -march=native -mtune=native -o 8 main.c
