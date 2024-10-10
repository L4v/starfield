#! /bin/bash

cc -o build/starfield src/*.c  -Iinclude/ -Llib $(pkg-config --cflags --libs glfw3) -lglad -lm
