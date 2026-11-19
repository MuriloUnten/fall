build:
	gcc -I./raylib/include -L./raylib/lib -Wl,-rpath,'$$ORIGIN/raylib/lib' -o main main.c clay_renderer_raylib.c -lraylib -lm

run:
	./main

all: build run
