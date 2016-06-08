CC?=gcc
#CC=mingw-w64-x86_64-gcc
#CC=clang

SRC=$(wildcard src/*.c)
OBJS=$(SRC:.c=.o)

CFLAGS=-std=c99 -O2 -Wall
INCS=-Iinc
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
OUTPUT=stage9

ifeq ($(OS),Windows_NT)
	LIBS := -lmingw32 -lSDL2main $(LIBS)
endif

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCS) -o $@ $<
	
clean:
	rm src/*.o
	rm $(OUTPUT)
