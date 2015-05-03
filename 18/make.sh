#!/bin/sh

gcc -O3 -flto -funroll-loops -march=native -mtune=native -o 18 main.c
