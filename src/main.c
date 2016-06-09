#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "common.h"
#include "log.h"
#include "s9map.h"

#define WIN_W		1024
#define WIN_H		768
#define REND_OPS	SDL_RENDERER_PRESENTVSYNC

#define TILE_SIZE	16

#define FONT_SIZE_X	5
#define FONT_SIZE_Y	12
#define FONT_COLS	20

// Interface sizes
#define BORDER_SIZE	2

#define TOOLBAR_X	BORDER_SIZE
#define TOOLBAR_Y	BORDER_SIZE
#define TOOLBAR_W	(WIN_W - 2 * BORDER_SIZE)
#define TOOLBAR_H	24

#define MAP_X		BORDER_SIZE
#define MAP_Y		(TOOLBAR_Y + TOOLBAR_H + BORDER_SIZE)
#define MAP_W		640
#define MAP_H		448

#define MINIMAP_X	(MAP_X + MAP_W + BORDER_SIZE)
#define MINIMAP_Y	(TOOLBAR_Y + TOOLBAR_H + BORDER_SIZE)
#define MINIMAP_W	128
#define MINIMAP_H	96

#define TILESET_X	(MAP_X + MAP_W + BORDER_SIZE)
#define TILESET_Y	(MINIMAP_Y + MINIMAP_H + BORDER_SIZE)
#define TILESET_W	128
#define TILESET_H	384

#define MLEFT 	SDL_BUTTON_LEFT
#define MRIGHT 	6 // SDL_BUTTON_RIGHT is the middle mouse button... yeah

#define COLOR_WHITE ((SDL_Color) { 0xFF, 0xFF, 0xFF, 0xFF })
#define COLOR_BLACK ((SDL_Color) { 0x00, 0x00, 0x00, 0x00 })

#define init(cond, err) ({ if(cond) { lprintf(FATAL, err()); exit(1); } })

#define mouse_left_pressed() (mstate & MLEFT)
#define mouse_left_released() (omstate & MLEFT && !(mstate & MLEFT))
#define mouse_right_pressed() (mstate & MRIGHT)
#define mouse_right_released() (omstate & MRIGHT && !(mstate & MRIGHT))

#define draw_rect(x, y, w, h) ({                                                               \
	SDL_Rect r = { x, y, w, h };                                                               \
	SDL_RenderDrawRect(renderer, &r);                                                          \
})

#define fill_rect(x, y, w, h) ({                                                               \
	SDL_Rect r = { x, y, w, h };                                                               \
	SDL_RenderFillRect(renderer, &r);                                                          \
})

#define draw_texture(tex, x, y) ({                                                             \
	SDL_Rect r = { x, y, 0, 0 };                                                               \
	SDL_QueryTexture(tex, NULL, NULL, &r.w, &r.h);                                             \
	SDL_RenderCopy(renderer, tex, NULL, &r);                                                   \
})

#define draw_tile(tex, sx, sy, tx, ty, w, h) ({                                                \
	SDL_Rect sr = { sx, sy, w, h };                                                            \
	SDL_Rect tr = { tx, ty, w, h };                                                            \
	SDL_RenderCopy(renderer, tex, &sr, &tr);                                                   \
})

// SDL stuff
SDL_Window *window;
SDL_Renderer *renderer;
u64 frameTime;

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
S9Map *mapFile = NULL;

// Interface
SDL_Texture *font = NULL;
int mapCamX = 0, mapCamY = 0;
int tsScrollY = 0;
int selectedTile = 0;

SDL_Texture* load_texture(const char *filename) {
	SDL_Surface *surface = IMG_Load(filename);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void draw_text(const char *text, int x, int y, SDL_Color color) {
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

void draw_toolbar() {
	
}

void draw_map() {
	if(mapFile == NULL) {
		draw_text("No Map Loaded", MAP_X + 32, MAP_Y + 32, COLOR_WHITE);
		return;
	}
	// Draw map tiles
	int bx = mapCamX / TILE_SIZE, by = mapCamY / TILE_SIZE,
		bw = MAP_W / TILE_SIZE, bh = MAP_H / TILE_SIZE;
	if(mapCamX % TILE_SIZE != 0) bw++;
	if(mapCamY % TILE_SIZE != 0) bh++;
	for(int y = by; y < bh && y < mapFile->layoutHeight && y < by + bw; y++) {
		for(int x = bx; x < bw && x < mapFile->layoutWidth && x < by + bw; x++) {
			// Tileset index for this map tile
			u16 ind = mapFile->tiles[y * mapFile->layoutWidth + x];
			// Where to draw it on the screen
			int drawx = MAP_X + x * TILE_SIZE - (mapCamX % TILE_SIZE), 
				drawy = MAP_Y + y * TILE_SIZE - (mapCamY % TILE_SIZE);
			// If no tileset is loaded, draw the index number
			if(tsTexture == NULL) {
				char str[8];
				sprintf(str, "%d", ind);
				draw_text(str, drawx + 1, drawy + 1, COLOR_WHITE);
			} else {
				draw_tile(tsTexture, 
					(ind % tsOpFile->tilesetWidth) * TILE_SIZE, 
					(ind / tsOpFile->tilesetWidth) * TILE_SIZE,
					drawx, drawy, TILE_SIZE, TILE_SIZE
				);
			}
		}
	}
	// Grid
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x7F);
	for(int x = MAP_X + mapCamX % TILE_SIZE; x < MAP_X + MAP_W; x += TILE_SIZE) {
		fill_rect(x, MAP_Y, 1, MAP_H);
	}
	for(int y = MAP_Y + mapCamY % TILE_SIZE; y < MAP_Y + MAP_H; y += TILE_SIZE) {
		fill_rect(MAP_X, y, MAP_W, 1);
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void draw_minimap() {
	
}

void draw_tileset() {
	if(tsTexture == NULL) {
		draw_text("No Tileset Loaded", TILESET_X + 32, TILESET_Y + 32, COLOR_WHITE);
		return;
	}
}

int main(int argc, char *argv[]) {
	lopen("log.txt");
	atexit(lclose);
	// Init SDL, don't need audio or network
	lprintf(DEBUG, "Initializing SDL\n");
	init(SDL_Init(SDL_INIT_VIDEO) < 0, SDL_GetError);
	atexit(SDL_Quit);
	lprintf(DEBUG, "Initializing SDL_image\n");
	init(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG, IMG_GetError);
	atexit(IMG_Quit);
	// GTK
	gtk_init(&argc, &argv);
	// Window and renderer
	lprintf(DEBUG, "Creating Window\n");
	init((window = SDL_CreateWindow("Stage9", -1, -1, WIN_W, WIN_H, 0)) == NULL, SDL_GetError);
	lprintf(DEBUG, "Creating Renderer\n");
	init((renderer = SDL_CreateRenderer(window, -1, REND_OPS)) == NULL, SDL_GetError);
	// Load font
	lprintf(DEBUG, "Loading font.png\n");
	font = load_texture("font.png");
	// Load empty map
	mapFile = calloc(sizeof(S9Map), 1);
	mapFile->version = S9M_VERSION;
	mapFile->name = calloc(1, sizeof("Untitled"));
	sprintf(mapFile->name, "Untitled");
	mapFile->layoutWidth = 40;
	mapFile->layoutHeight = 28;
	mapFile->tiles = calloc(2, mapFile->layoutWidth * mapFile->layoutHeight);
	//mapFile->entityCount = 0;
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	frameTime = SDL_GetTicks();
	bool running = true;
	while(running) {
		SDL_SetRenderDrawColor(renderer, 0x22, 0x22, 0x22, 0xFF);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		// Poll input - do things when the user presses stuff
		SDL_Event event;
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
		// Draw
		draw_toolbar();
		draw_map();
		draw_minimap();
		draw_tileset();
		// Next frame
		SDL_Delay(1000 / 60 - SDL_GetTicks() + frameTime);
		frameTime = SDL_GetTicks();
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
