CC ?= gcc
GTKVER ?= gtk+-2.0
#CC = mingw-w64-x86_64-gcc
#CC = clang

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)
CFLAGS = -std=c99 -O2 -g -Wall -Wextra
INCS = -Iinc
LIBS = -lSDL2 -lSDL2_image
OUTPUT = stage9

ifeq ($(OS),Windows_NT)
	LIBS := -L3rdparty/gtk/win32 -lmingw32 -lSDL2main $(LIBS)
	INCS += -I3rdparty/gtk/include
	GTKVER := gtk+-2.0
endif

CFLAGS += `pkg-config --cflags $(GTKVER)`
LIBS += `pkg-config --libs $(GTKVER)`

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCS) -o $@ $<
	
clean:
	rm src/*.o
	rm $(OUTPUT)
