#!/bin/sh

export MBD_LISTEN="0.0.0.0:8080"
export MBD_MPD_SERVER="localhost:6600"
export MBD_MUSIC_DIRECTORY="/home/mastensg/lib/audio"

./mbd
