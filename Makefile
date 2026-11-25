# build:
# 	gcc -I./raylib/include -L./raylib/lib -Wl,-rpath,'$$ORIGIN/raylib/lib' -o main main.c clay_renderer_raylib.c -lraylib -lm
#
# run:
# 	./main
#
# all: build run

CC = gcc

TARGET = main

CFLAGS = -Wall -Wextra -I./raylib/include
LDFLAGS = -L./raylib/lib -Wl,-rpath,'$$ORIGIN/raylib/lib'
LDLIBS = -lraylib -lm

SOURCES = main.c clay_renderer_raylib.c tinyfiledialogs/tinyfiledialogs.c pdi/pdi.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: run

run: $(TARGET)
	./$(TARGET)
