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

#define map_tile(x, y) (map->tiles[(x) + (y) * map->layoutWidth])

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

#define TILESET_X	(MAP_X + MAP_W + BORDER_SIZE)
#define TILESET_Y	(TOOLBAR_Y + TOOLBAR_H + BORDER_SIZE)
#define TILESET_W	128
#define TILESET_H	MAP_H

// Interface / Selection values
#define MAP_SCROLL_SPEED 4

int mapCamX = 0, mapCamY = 0;
int mapHoverX = -1, mapHoverY = -1;
int tsScrollY = 0;
int tsSelected = 0, tsHover = -1;

// Input
#define MLEFT 	SDL_BUTTON_LEFT
#define MRIGHT 	SDL_BUTTON_RIGHT

#define mouse_left_pressed() (mstate & MLEFT && !(omstate & MLEFT))
#define mouse_left_down() (mstate & MLEFT)
#define mouse_left_released() (omstate & MLEFT && !(mstate & MLEFT))
#define mouse_right_pressed() (mstate & MRIGHT && !(omstate & MRIGHT))
#define mouse_right_down() (mstate & MRIGHT)
#define mouse_right_released() (omstate & MRIGHT && !(mstate & MRIGHT))
#define mouse_within(x1, y1, x2, y2) (mousex > x1 && mousex < x2 && mousey > y1 && mousey < y2)

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

void draw_tileset() {
	if(tsTexture == NULL) {
		graphics_draw_text("No Tileset Loaded", TILESET_X + 32, TILESET_Y + 32, COLOR_WHITE);
		return;
	}
	// The tileset could be any size, so wrap it to fit into the tileset area
	int columns = TILESET_W / TILE_SIZE;
	int firstTile = tsScrollY / TILE_SIZE * columns;
	int lastTile = firstTile + TILESET_H / TILE_SIZE * columns;
	// Source cursor that is moved left to right up to down on the tileset texture
	int sx = (firstTile % tsProp->tilesetWidth) * TILE_SIZE;
	int sy = (firstTile / tsProp->tilesetWidth) * TILE_SIZE;
	// When halfway scrolled betweet rows need to draw an extra one
	if(tsScrollY % TILE_SIZE != 0) lastTile += columns;
	for(int i = firstTile; i < lastTile; i++) {
		draw_tile(tsTexture, sx, sy, 
			TILESET_X + ((i % columns) * TILE_SIZE), 
			TILESET_Y + (i / columns * TILE_SIZE),
			TILE_SIZE, TILE_SIZE);
		if(i == tsHover || i == tsSelected) {
			draw_rect(TILESET_X + ((i % columns) * TILE_SIZE), 
				TILESET_Y + (i / columns * TILE_SIZE), TILE_SIZE, TILE_SIZE);
		}
		// Move source cursor in tileset texture to next tile
		sx += TILE_SIZE;
		if(sx / TILE_SIZE >= tsProp->tilesetWidth) {
			sx = 0;
			sy += TILE_SIZE;
			if(sy / TILE_SIZE >= tsProp->tilesetHeight) break;
		}
	}
}

void draw_border() {
	
}

void draw_toolbar() {
	
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
		// Mouse state
		omstate = mstate;
		mstate = SDL_GetMouseState(&mousex, &mousey);
		// Poll input - do things when the user presses stuff
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				running = false;
				break;
			} else if(event.type == SDL_KEYDOWN) {
				int key = event.key.keysym.sym;
				if(key == SDLK_LEFT) {
					mapCamX -= MAP_SCROLL_SPEED;
				} else if(key == SDLK_UP) {
					mapCamY -= MAP_SCROLL_SPEED;
				} else if(key == SDLK_RIGHT) {
					mapCamX += MAP_SCROLL_SPEED;
				} else if(key == SDLK_DOWN) {
					mapCamY += MAP_SCROLL_SPEED;
				}
				if(map != NULL) {
					if(mapCamX > map->layoutWidth * TILE_SIZE - MAP_W)
						mapCamX = map->layoutWidth * TILE_SIZE - MAP_W;
					if(mapCamY > map->layoutHeight * TILE_SIZE - MAP_H)
						mapCamY = map->layoutHeight * TILE_SIZE - MAP_H;
					if(mapCamX < 0 || map->layoutWidth <= 40) mapCamX = 0;
					if(mapCamY < 0 || map->layoutHeight <= 28) mapCamY = 0;
				}
			}
		}
		// Update mouse stuff
		mapHoverX = mapHoverY = tsHover = -1;
		if(mouse_within(MAP_X, MAP_Y, MAP_X + MAP_W, MAP_Y + MAP_H)) {
			// Get the map tile x and y underneath the mouse
			mapHoverX = (mousex - MAP_X + mapCamX) / TILE_SIZE; 
			mapHoverY = (mousey - MAP_Y + mapCamY) / TILE_SIZE;
			if(mouse_left_down()) { // Draw tile
				map_tile(mapHoverX, mapHoverY) = tsSelected;
			} else if(mouse_right_down()) { // Erase
				map_tile(mapHoverX, mapHoverY) = 0;
			}
		} else if(mouse_within(TILESET_X, TILESET_Y, TILESET_X+TILESET_W, TILESET_Y+TILESET_H)) {
			// Get which tile in the tileset is underneath the mouse
			u16 mx = (mousex - TILESET_X) / TILE_SIZE, 
				my = (mousey - TILESET_Y + tsScrollY) / TILE_SIZE;
			tsHover = mx + my * (TILESET_W / TILE_SIZE);
			if(mouse_left_down()) { // Select hovering tile
				tsSelected = tsHover;
			}
		}
		// Draw
		draw_map();
		draw_tileset();
		draw_border();
		draw_toolbar();
		graphics_present();
	}
	graphics_close();
	return 0;
}
