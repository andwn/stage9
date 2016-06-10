#include "graphics.h"
#include "log.h"

u64 frameTime;
SDL_Texture *font;

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

SDL_Texture* graphics_load_texture(const char *filename) {
	lprintf(DEBUG, "Loading texture: %s", filename);
	SDL_Surface *surface = IMG_Load(filename);
	if(surface == NULL) {
		lprintf(ERROR, SDL_GetError());
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void graphics_draw_text(const char *text, int x, int y, SDL_Color color) {
	int i, line = 0;
	SDL_Rect srect = {0, 0, FONT_SIZE_X, FONT_SIZE_Y}, 
		drect = {x, y, FONT_SIZE_X, FONT_SIZE_Y};
	SDL_SetTextureAlphaMod(font, color.a);
	for(i = 0; text[i] != '\0'; i++) {
		if(text[i] > 0x20) {
			drect.x = x + FONT_SIZE_X * (i - line);
			srect.x = ((text[i] - 0x20) % FONT_COLS) * FONT_SIZE_X;
			srect.y = ((text[i] - 0x20) / FONT_COLS) * FONT_SIZE_Y;
			SDL_SetTextureColorMod(font, color.r, color.g, color.b);
			SDL_RenderCopy(renderer, font, &srect, &drect);
		} else if(text[i] == '\n') {
			drect.x = x;
			drect.y += FONT_SIZE_Y;
			line = i + 1;
		}
	}
}

void graphics_present() {
	int delay = 1000 / 60 - SDL_GetTicks() + frameTime;
	if(delay > 0) SDL_Delay(delay);
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
