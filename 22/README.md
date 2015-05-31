opak
====

This program encrypts or decrypts streams.

Building
--------

    autoreconf -i
    ./configure
    make

Running
-------

    echo attack at dawn >plaintext
    ./opak secret <plaintext >encrypted
    ./opak secret <encrypted >decrypted
