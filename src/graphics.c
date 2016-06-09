#include "graphics.h"
#include "log.h"

void graphics_init() {
	// Init SDL, don't need audio or network
	lprintf(DEBUG, "Initializing SDL");
	init(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0, SDL_GetError);
	atexit(SDL_Quit);
	lprintf(DEBUG, "Initializing SDL_image");
	init(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG, IMG_GetError);
	atexit(IMG_Quit);
	// Window and renderer
	lprintf(DEBUG, "Creating Window");
	init((window = SDL_CreateWindow("Stage9", -1, -1, WIN_W, WIN_H, 0)) == NULL, SDL_GetError);
	lprintf(DEBUG, "Creating Renderer");
	init((renderer = SDL_CreateRenderer(window, -1, REND_OPS)) == NULL, SDL_GetError);
	// Load font
	//lprintf(DEBUG, "Loading font.png");
	//init((font = load_texture("font.png")) == NULL, SDL_GetError);
	// Alpha blending
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	frameTime = SDL_GetTicks();
}

void graphics_present() {
	SDL_Delay(1000 / 60 - SDL_GetTicks() + frameTime);
	frameTime = SDL_GetTicks();
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0x22, 0x22, 0x22, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void graphics_close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
