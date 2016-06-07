CC?=gcc
#CC=i686-w64-mingw32-gcc
#CC=clang

SRC=$(wildcard *.c)
OBJS=$(SRC:*.c=.o)

CFLAGS=-std=c99 -O2 -g -Wall
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
OUTPUT=stage9

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	
clean:
	rm *.o
	rm $(OUTPUT)
