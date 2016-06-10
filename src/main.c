#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "common.h"
#include "log.h"
#include "s9map.h"
#include "lmu.h"
#include "dialog.h"
#include "graphics.h"

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

#define mouse_left_pressed() (mstate & MLEFT)
#define mouse_left_released() (omstate & MLEFT && !(mstate & MLEFT))
#define mouse_right_pressed() (mstate & MRIGHT)
#define mouse_right_released() (omstate & MRIGHT && !(mstate & MRIGHT))

// Input
u32 mstate = 0, omstate;
int mousex = 0, mousey = 0;

// Tileset
char *tsFilename = NULL;
SDL_Texture *tsTexture = NULL;
char *tsPropFilename = NULL;
S9TileOpFile *tsProp = NULL;

// Map
char *mapFilename = NULL;
S9Map *map = NULL;

// Interface
int mapCamX = 0, mapCamY = 0;
int tsScrollY = 0;
int selectedTile = 0;

void draw_toolbar() {
	
}

void draw_map() {
	if(map == NULL) {
		graphics_draw_text("No Map Loaded", MAP_X + 32, MAP_Y + 32, COLOR_WHITE);
		return;
	}
	//lprintf(TRACE, "Drawing map tiles");
	// Draw map tiles
	int bx = mapCamX / TILE_SIZE, by = mapCamY / TILE_SIZE,
		bw = MAP_W / TILE_SIZE, bh = MAP_H / TILE_SIZE;
	if(mapCamX % TILE_SIZE != 0) bw++;
	if(mapCamY % TILE_SIZE != 0) bh++;
	for(int y = by; y < bh && y < map->layoutHeight && y < by + bw; y++) {
		for(int x = bx; x < bw && x < map->layoutWidth && x < by + bw; x++) {
			//lprintf(TRACE, "Tile at index: %d, %d", x, y);
			// Tileset index for this map tile
			u16 ind = map->tiles[y * map->layoutWidth + x];
			// Where to draw it on the screen
			int drawx = MAP_X + x * TILE_SIZE - (mapCamX % TILE_SIZE), 
				drawy = MAP_Y + y * TILE_SIZE - (mapCamY % TILE_SIZE);
			// If no tileset is loaded, draw the index number
			if(tsTexture == NULL) {
				char str[8];
				sprintf(str, "%d", ind);
				graphics_draw_text(str, drawx + 1, drawy + 1, COLOR_WHITE);
			} else {
				draw_tile(tsTexture, 
					(ind % tsProp->tilesetWidth) * TILE_SIZE, 
					(ind / tsProp->tilesetWidth) * TILE_SIZE,
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
		graphics_draw_text("No Tileset Loaded", TILESET_X + 32, TILESET_Y + 32, COLOR_WHITE);
		return;
	}
	
}

int main(int argc, char *argv[]) {
	lopen("log.txt");
	atexit(lclose);
	// GTK
	gtk_init(&argc, &argv);
	// Graphics
	graphics_init();
	// Load empty map
	//map = lmu_import("sample/mado.lmu");
	map = map_create("Untitled", 40, 28);
	tsTexture = graphics_load_texture("sample/main.png");
	//tsProp = tileprop_new_from_texture(tsTexture);
	tsProp = calloc(sizeof(S9TileOpFile), 1);
	tsProp->tilesetWidth = 480 / 16;
	tsProp->tilesetHeight = 256 / 16;
	bool running = true;
	while(running) {
		// Poll input - do things when the user presses stuff
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				running = false;
				break;
			} /*else if(event.type == SDL_KEYDOWN) {
				int key = event.key.keysym.sym;
				if(key == SDLK_LEFT) {
					
				} else if(key == SDLK_UP) {
					
				} else if(key == SDLK_RIGHT) {
					
				} else if(key == SDLK_DOWN) {
					
				}
			}*/
		}
		// Mouse state
		omstate = mstate;
		mstate = SDL_GetMouseState(&mousex, &mousey);
		// Draw
		draw_toolbar();
		draw_map();
		draw_minimap();
		draw_tileset();
		graphics_present();
	}
	graphics_close();
	return 0;
}
