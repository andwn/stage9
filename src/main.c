#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include "common.h"
#include "log.h"

#define WIN_W		1024
#define WIN_H		768

#define MLEFT SDL_BUTTON_LEFT
#define MRIGHT 6 // SDL_BUTTON_RIGHT is the middle mouse button... yeah

#define mouse_left_pressed() (mstate & MLEFT)
#define mouse_left_released() (omstate & MLEFT && !(mstate & MLEFT))
#define mouse_right_pressed() (mstate & MRIGHT)
#define mouse_right_released() (omstate & MRIGHT && !(mstate & MRIGHT))

// SDL stuff
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
long frameTime;

// Input
u32 mstate = 0, omstate;
int mousex = 0, mousey = 0;

// Tileset
char *tsFilename = NULL;
SDL_Texture *tsTexture = NULL;
char *tsOpFilename = NULL;
S9TileOpFile *tsOpFile = NULL;

// Map
char *mapFilename = NULL;
S9MapFile *mapFile = NULL;

// Interface
int mapCamX = 0, mapCamY = 0;
int tsScrollY = 0;
int selectedTile = 0;

void tileset_load(const char *filename) {
	SDL_Surface *surface = IMG_Load(filename);
	tsTexture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
}

int main(int argc, char *argv[]) {
	lopen("log.txt");
	atexit(lclose);
	// Init SDL, don't need audio or network
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		lprintf(FATAL, SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		lprintf(FATAL, IMG_GetError());
		exit(1);
	}
	atexit(IMG_Quit);
	if(TTF_Init() < 0) {
		lprintf(FATAL, TTF_GetError());
		exit(1);
	}
	atexit(TTF_Quit);
	// Window and renderer
	window = SDL_CreateWindow("Stage9 - Untitled", -1, -1, WIN_W, WIN_H, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	frameTime = SDL_GetTicks();
	bool running = true;
	while(running) {
		SDL_RenderClear(renderer);
		// Poll input - do things when the user presses stuff
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				printf("fuc\n");
				running = false;
				break;
			} else if(event.type == SDL_KEYDOWN) {
				int key = event.key.keysym.sym;
				if(key == SDLK_LEFT) {
					
				} else if(key == SDLK_UP) {
					
				} else if(key == SDLK_RIGHT) {
					
				} else if(key == SDLK_DOWN) {
					
				}
			}
		}
		// Mouse state
		omstate = mstate;
		mstate = SDL_GetMouseState(&mousex, &mousey);
		
		// Next frame
		SDL_Delay(1000 / 60 - SDL_GetTicks() + frameTime);
		frameTime = SDL_GetTicks();
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
