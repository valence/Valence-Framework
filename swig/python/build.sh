#! /bin/bash
swig -python example.i
gcc -c example.c example_wrap.c \
        -I/usr/include/python2.6
ld -shared example.o example_wrap.o -o _example.so 
