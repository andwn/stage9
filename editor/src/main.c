/**
 *  Simple-ish editor for tilemaps. Meant for MegaDrive but the format is portable.
 *  Copyright (C) 2016 andwn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "common.h"
#include "log.h"
#include "map.h"
#include "dialog.h"
#include "graphics.h"
#ifdef _WIN32
#include <windows.h>
#endif

#define map_tile(l, x, y) (map->tiles[(x) + (y) * map->width + (l) * map->width * map->height])

// Interface sizes
#define BORDER_SIZE	2

#define TOOLBAR_X	BORDER_SIZE
#define TOOLBAR_Y	BORDER_SIZE
#define TOOLBAR_W	(WIN_W - 2 * BORDER_SIZE)
#define TOOLBAR_H	20

#define MAP_X		BORDER_SIZE
#define MAP_Y		(TOOLBAR_Y + TOOLBAR_H + BORDER_SIZE)
#define MAP_W		640
#define MAP_H		448

#define MAPVSCROLL_X	(MAP_X + MAP_W)
#define MAPVSCROLL_Y	(MAP_Y)
#define MAPVSCROLL_W	12
#define MAPVSCROLL_H	MAP_H

#define MAPHSCROLL_X	(MAP_X)
#define MAPHSCROLL_Y	(MAP_Y + MAP_H)
#define MAPHSCROLL_W	MAP_W
#define MAPHSCROLL_H	12

#define TILESET_X	(MAPVSCROLL_X + MAPVSCROLL_W + BORDER_SIZE)
#define TILESET_Y	(TOOLBAR_Y + TOOLBAR_H + BORDER_SIZE)
#define TILESET_W	128
#define TILESET_H	MAP_H

#define TSETVSCROLL_X	(TILESET_X + TILESET_W)
#define TSETVSCROLL_Y	(TILESET_Y)
#define TSETVSCROLL_W	12
#define TSETVSCROLL_H	TILESET_H

#define MENU_W		96
#define MENUITEM_W	128
#define MENUITEM_H	20

#define BUTTONS_X	MAP_X
#define BUTTONS_Y	(MAP_Y + MAP_H + BORDER_SIZE)
#define BUTTON_W	96
#define BUTTON_H	24

#define MAP_SCROLL_SPEED 4

// Menu
#define MENU_COUNT			4
#define MENU_MAXITEMCOUNT	6
#define MENU_MAXLABELSIZE	32
enum { 
	MENU_MAP, 
	MENU_TSET,
	MENU_ATTR,
	MENU_HELP
};
enum { 
	MENU_MAP_NEW, 
	MENU_MAP_OPEN, 
	MENU_MAP_SAVE, 
	MENU_MAP_SAVEAS, 
	MENU_MAP_PROPERTIES,
	MENU_MAP_CLOSE
};
enum { 
	MENU_TSET_OPEN,
	MENU_TSET_CLOSE
};
enum {
	MENU_ATTR_OPEN, 
	MENU_ATTR_SAVE, 
	MENU_ATTR_SAVEAS, 
	MENU_ATTR_EDIT 
};
enum {
	MENU_HELP_MANUAL,
	MENU_HELP_SOURCE
};
const u8 menuItemCount[MENU_COUNT] = { 6, 2, 4, 2 };
const char menu[MENU_COUNT][MENU_MAXITEMCOUNT + 1][MENU_MAXLABELSIZE] = {
	{ "Map", "New", "Open", "Save", "SaveAs", "Properties", "Close" },
	{ "Tileset", "Open", "Close" },
	{ "Tile Attributes", "Open", "Save", "SaveAs", "Edit" },
	{ "Help", "Manual", "Source" }
};
u8 menuGlow[MENU_COUNT] = { 0, 0, 0, 0 };
u8 subMenuGlow[MENU_MAXITEMCOUNT] = { 0, 0, 0, 0, 0, 0 };

// Buttons
#define BUTTON_COUNT 2
const char buttonText[BUTTON_COUNT][MENU_MAXLABELSIZE] = { "Lower", "Upper" };
u8 buttonGlow[BUTTON_COUNT] = { 0, 0 };

// Input
#define MLEFT 	SDL_BUTTON_LEFT
#define MRIGHT 	6 //SDL_BUTTON_RIGHT

#define mouse_left_pressed() (mstate & MLEFT && !(omstate & MLEFT))
#define mouse_left_down() (mstate & MLEFT)
#define mouse_left_released() (omstate & MLEFT && !(mstate & MLEFT))
#define mouse_right_pressed() (mstate & MRIGHT && !(omstate & MRIGHT))
#define mouse_right_down() (mstate & MRIGHT)
#define mouse_right_released() (omstate & MRIGHT && !(mstate & MRIGHT))
#define mouse_within(x1, y1, x2, y2) (mousex > x1 && mousex < x2 && mousey > y1 && mousey < y2)

// When the mouse is clicked over a control it locks to that
// control to avoid accidental things from happening
enum { LOCK_NONE, LOCK_TOOLBAR, LOCK_MAP, LOCK_TILESET, 
	LOCK_MAPVSCROLL, LOCK_MAPHSCROLL, LOCK_TSETVSCROLL };

u32 mstate = 0, omstate;
int mousex = 0, mousey = 0;
int mlock = LOCK_NONE;

// Tileset
char *tsFilename = NULL;
SDL_Texture *tsTexture = NULL;
int tsWidth = 0, tsHeight = 0;

// Tile Attributes
char *attrFilename = NULL;
u8 *attr = NULL;

// Map
char *mapFilename = NULL;
Map *map = NULL;

// Current states of things
int mapCamX = 0, mapCamY = 0;
int mapHoverX = -1, mapHoverY = -1;
int tsScrollY = 0;
int tsSelected = 0, tsHover = -1;
int menuHover = -1, menuOpen = -1, menuSubHover = -1;
int buttonHover = -1;
bool upperSelected = false;

// Nested switch statements aren't very organized but whatever
// Some cases are given blocks because variables
void do_menu_action(int menuIndex, int item) {
	switch(menuIndex) {
		case MENU_MAP:
		switch(item) {
			case MENU_MAP_NEW: {
				// Create a new map dialog (NULL means new map)
				MapDialogResult *result = dialog_map_edit(NULL);
				// And create a map if it wasn't cancelled
				if(!result->cancelled) {
					Map *newMap = map_create(result->name, result->width, result->height,
						result->upperLayer, result->planA, result->byteTiles,
						result->wrapH, result->wrapV);
					if(newMap != NULL) {
						// There might be a map already loaded so clean it up first
						if(map != NULL) map_free(map);
						map = newMap;
						if(mapFilename != NULL) {
							free(mapFilename);
							mapFilename = NULL;
						}
					}
				}
				free(result);
				break;
			}
			case MENU_MAP_OPEN: {
				// Browse to a file and open it
				char *newFilename = dialog_map_open();
				if(newFilename != NULL) {
					Map *newMap = map_open(newFilename);
					if(newMap != NULL) {
						if(map != NULL) map_free(map);
						map = newMap;
						if(mapFilename != NULL) free(mapFilename);
						mapFilename = newFilename;
					}
				}
				break;
			}
			case MENU_MAP_SAVE:
			if(mapFilename != NULL) {
				map_save(mapFilename, map);
				break;
			} // Fall into SaveAs if the filename is NULL
			case MENU_MAP_SAVEAS: {
				char *newFilename = dialog_map_save();
				if(newFilename != NULL) {
					map_save(newFilename, map);
					if(mapFilename != NULL) free(mapFilename);
					mapFilename = newFilename;
				}
				break;
			}
			case MENU_MAP_PROPERTIES: {
				// Edit an existing map, pass what is loaded to the dialog
				if(map == NULL) break;
				MapDialogResult *result = dialog_map_edit(map);
				// Create a new map if the dialog wasn't cancelled
				if(!result->cancelled) {
					Map *newMap = map_create(result->name, result->width, result->height,
						result->upperLayer, result->planA, result->byteTiles,
						result->wrapH, result->wrapV);
					if(newMap != NULL) {
						// Copy the current map's tile data into the new one
						map_copy_tiles(map, newMap);
						// Finally replace current map with the new one
						map_free(map);
						map = newMap;
					}
				}
				free(result);
			}
			break;
			case MENU_MAP_CLOSE:
			if(map != NULL) map_free(map);
			map = NULL;
			if(mapFilename != NULL) free(mapFilename);
			mapFilename = NULL;
			break;
		}
		break;
		case MENU_TSET: 
		switch(item) {
			case MENU_TSET_OPEN: {
				char *newFilename = dialog_tileset_open();
				if(newFilename != NULL) {
					if(tsTexture != NULL) SDL_DestroyTexture(tsTexture);
					tsTexture = graphics_load_texture(newFilename, true);
					if(tsTexture != NULL) {
						SDL_QueryTexture(tsTexture, NULL, NULL, &tsWidth, &tsHeight);
						tsWidth /= TILE_SIZE;
						tsHeight /= TILE_SIZE;
						// Make another copy of the string to keep the GTK stuff separate
						if(tsFilename != NULL) free(tsFilename);
						tsFilename = newFilename;
					}
				}
			}
			break;
			case MENU_TSET_CLOSE:
			if(tsTexture != NULL) SDL_DestroyTexture(tsTexture);
			tsTexture = NULL;
			break;
		}
		break;
		case MENU_ATTR: break;
		case MENU_HELP: 
		switch(item) {
			case MENU_HELP_MANUAL:
			#ifdef _WIN32
			ShellExecute(0, 0, L"wordpad ../doc/MANUAL.md", 0, 0 , SW_SHOW);
			#elif __APPLE__
			system("open '../doc/MANUAL.md'");
			#else
			system("xdg-open '../doc/MANUAL.md'");
			#endif
			break;
			case MENU_HELP_SOURCE:
			#ifdef _WIN32
			ShellExecute(0, 0, L"https://github.com/andwn/stage9", 0, 0 , SW_SHOW);
			#elif __APPLE__
			system("open 'https://github.com/andwn/stage9'");
			#else
			system("xdg-open 'https://github.com/andwn/stage9'");
			#endif
			break;
		}
		break;
	}
}

bool update_input() {
	// Poll input - do things when the user presses stuff
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {
			return true;
		}
	}
	// Mouse state
	omstate = mstate;
	mstate = SDL_GetMouseState(&mousex, &mousey);
	// Keyboard
	const u8 *key = SDL_GetKeyboardState(NULL);
	if(key[SDL_SCANCODE_LEFT]) mapCamX -= MAP_SCROLL_SPEED;
	if(key[SDL_SCANCODE_UP]) mapCamY -= MAP_SCROLL_SPEED;
	if(key[SDL_SCANCODE_RIGHT]) mapCamX += MAP_SCROLL_SPEED;
	if(key[SDL_SCANCODE_DOWN]) mapCamY += MAP_SCROLL_SPEED;
	if(map != NULL) { // Stay in bounds
		if(mapCamX > map->width * TILE_SIZE - MAP_W)
			mapCamX = map->width * TILE_SIZE - MAP_W;
		if(mapCamY > map->height * TILE_SIZE - MAP_H)
			mapCamY = map->height * TILE_SIZE - MAP_H;
		if(mapCamX < 0 || map->width <= 40) mapCamX = 0;
		if(mapCamY < 0 || map->height <= 28) mapCamY = 0;
	}
	return false;
}

// These update functions occur if the mouse is over a specific area
void update_submenu() {
	int menux = TOOLBAR_X + menuOpen * MENU_W,
		menuy = TOOLBAR_Y + TOOLBAR_H,
		menuw = MENUITEM_W,
		menuh = MENUITEM_H * menuItemCount[menuOpen];
	menuHover = menuOpen;
	mlock = LOCK_TOOLBAR;
	if(mouse_within(menux, menuy, menux+menuw, menuy+menuh)) { // Mouse inside menu
		menuSubHover = (mousey - menuy) / MENUITEM_H;
		if(mouse_left_pressed()) {
			do_menu_action(menuOpen, menuSubHover);
			menuOpen = -1;
		}
	} else { // Outside menu, unhighlight, close if clicked
		menuSubHover = -1;
		if(mouse_left_pressed()) menuOpen = -1;
	}
}

void update_map() {
	// Get the map tile x and y underneath the mouse
	mapHoverX = (mousex - MAP_X + mapCamX) / TILE_SIZE; 
	mapHoverY = (mousey - MAP_Y + mapCamY) / TILE_SIZE;
	if(mouse_left_pressed() && mlock == LOCK_NONE) {
		mlock = LOCK_MAP;
	}
	if(mouse_left_down() && mlock == LOCK_MAP) { // Draw tile
		map_tile(upperSelected, mapHoverX, mapHoverY) = tsSelected;
	}
}

void update_tileset() {
	// Get which tile in the tileset is underneath the mouse
	u16 mx = (mousex - TILESET_X) / TILE_SIZE, 
		my = (mousey - TILESET_Y + tsScrollY) / TILE_SIZE;
	tsHover = mx + my * (TILESET_W / TILE_SIZE);
	if(mouse_left_pressed() && mlock == LOCK_NONE) {
		mlock = LOCK_TILESET;
	}
	if(mouse_left_down() && mlock == LOCK_TILESET) { // Select hovering tile
		tsSelected = tsHover;
	}
}

void update_toolbar() {
	menuHover = (mousex - TOOLBAR_X) / MENU_W;
	if(menuHover >= MENU_COUNT) {
		menuHover = -1;
	} else if(mouse_left_pressed()) {
		menuOpen = menuHover;
	}
}

void update_buttons() {
	buttonHover = (mousex - BUTTONS_X) / (BUTTON_W + BORDER_SIZE);
	if(!map->upperLayer) buttonHover += 2;
	if(buttonHover >= BUTTON_COUNT || mousey >= BUTTONS_Y + BUTTON_H) {
		buttonHover = -1;
	} else if(mouse_left_pressed()) {
		if(buttonHover == 0) {
			upperSelected = 0;
		} else if(buttonHover == 1) {
			upperSelected = 1;
		}
	}
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
	for(int l = 0; l <= map->upperLayer; l++) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 
			(l != 0 && !upperSelected) ? 0x7F : 0xFF);
		for(int y = by; y < by + bh && y < map->height; y++) {
			for(int x = bx; x < bx + bw && x < map->width; x++) {
				//lprintf(TRACE, "Tile at index: %d, %d", x, y);
				// Tileset index for this map tile
				u16 ind = map_tile(l, x, y);
				// Where to draw it on the screen
				int drawx = MAP_X + x * TILE_SIZE - mapCamX, 
					drawy = MAP_Y + y * TILE_SIZE - mapCamY;
				// If no tileset is loaded, draw the index number
				if(tsTexture == NULL) {
					char str[8];
					sprintf(str, "%d", ind);
					graphics_draw_text(str, drawx + 1, drawy + 1, COLOR_WHITE);
				} else {
					draw_tile(tsTexture, 
						(ind % tsWidth) * TILE_SIZE, 
						(ind / tsWidth) * TILE_SIZE,
						drawx, drawy, TILE_SIZE, TILE_SIZE
					);
				}
				if(mapHoverX == x && mapHoverY == y) {
					if(mouse_left_down()) fill_rect(drawx, drawy, TILE_SIZE, TILE_SIZE);
					else draw_rect(drawx, drawy, TILE_SIZE, TILE_SIZE);
				}
			}
		}
	}
	// Grid
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x7F);
	for(int x = MAP_X + (mapCamX % TILE_SIZE); x < MAP_X + MAP_W; x += TILE_SIZE) {
		fill_rect(x, MAP_Y, 1, MAP_H);
	}
	for(int y = MAP_Y + (mapCamY % TILE_SIZE); y < MAP_Y + MAP_H; y += TILE_SIZE) {
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
	int sx = (firstTile % tsWidth) * TILE_SIZE;
	int sy = (firstTile / tsWidth) * TILE_SIZE;
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
		if(sx / TILE_SIZE >= tsWidth) {
			sx = 0;
			sy += TILE_SIZE;
			if(sy / TILE_SIZE >= tsHeight) break;
		}
	}
}

void draw_border() {
	
}

void draw_submenu(int menuIndex) {
	//lprintf(TRACE, "Drawing submenu %d", menuIndex);
	int menux = TOOLBAR_X + menuIndex * MENU_W,
		menuy = TOOLBAR_Y + TOOLBAR_H,
		menuw = MENUITEM_W,
		menuh = MENUITEM_H * menuItemCount[menuIndex];
	//SDL_SetRenderDrawColor(renderer, 0x66, 0x44, 0x88, 0x7F);
	//fill_rect(menux, menuy, menuw, menuh);
	for(int i = 0; i < menuItemCount[menuIndex]; i++) {
		if(subMenuGlow[i] > 0) subMenuGlow[i]--;
		if(menuSubHover == i) subMenuGlow[i] = 20;
		SDL_SetRenderDrawColor(renderer, 0x60 + subMenuGlow[i] * 2, 
			0x40 + subMenuGlow[i], 0x80 + subMenuGlow[i], 0x9F);
		fill_rect(menux, menuy + i * MENUITEM_H, menuw, MENUITEM_H);
		graphics_draw_text(menu[menuIndex][i + 1], 
			menux + 4, menuy + i * MENUITEM_H + 4, COLOR_WHITE);
	}
}

void draw_toolbar() {
	int x = TOOLBAR_X, y = TOOLBAR_Y;
	for(int i = 0; i < MENU_COUNT; i++) {
		if(menuGlow[i] > 0) menuGlow[i]--;
		if(menuHover == i || menuOpen == i) menuGlow[i] = 20;
		SDL_SetRenderDrawColor(renderer, 
			0x60 + menuGlow[i] * 2, 0x40 + menuGlow[i], 0x80 + menuGlow[i], 0xFF);
		fill_rect(x, y, MENU_W, TOOLBAR_H);
		graphics_draw_text(menu[i][0], x + 4, y + 4, COLOR_WHITE);
		if(menuOpen == i) draw_submenu(i);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		x += MENU_W;
	}
}

void draw_buttons() {
	int x = BUTTONS_X, y = BUTTONS_Y;
	if(map->upperLayer) {
		for(int i = 0; i < 2; i++) {
			if(buttonGlow[i] > 0) buttonGlow[i]--;
			if(buttonHover == i || upperSelected == i) buttonGlow[i] = 20;
			SDL_SetRenderDrawColor(renderer, 0x60 + buttonGlow[i] * 2, 
				0x40 + buttonGlow[i], 0x80 + buttonGlow[i], 0xFF);
			fill_rect(x, y, BUTTON_W, BUTTON_H);
			graphics_draw_text(buttonText[i], x + 4, y + 4, COLOR_WHITE);
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			x += BUTTON_W + BORDER_SIZE;
		}
	}
}

int main(int argc, char *argv[]) {
	lopen("log.txt");
	atexit(lclose);
	// Graphics
	graphics_init();
	// GTK - must be init after SDL or crash
	gtk_init(&argc, &argv);
	// Load empty map
	map = map_create_default();
	while(!update_input()) {
		// Update sections when mouse is over
		mapHoverX = mapHoverY = tsHover = menuHover = -1;
		if(menuOpen != -1) {
			update_submenu();
		} else if(mouse_within(MAP_X, MAP_Y, MAP_X + MAP_W, MAP_Y + MAP_H)) {
			update_map();
		} else if(mouse_within(TILESET_X, TILESET_Y, TILESET_X+TILESET_W, TILESET_Y+TILESET_H)) {
			update_tileset();
		} else if(mouse_within(TOOLBAR_X, TOOLBAR_Y, TOOLBAR_X+TOOLBAR_W, TOOLBAR_Y+TOOLBAR_H)) {
			update_toolbar();
		} else if(mousey >= BUTTONS_Y) {
			update_buttons();
		}
		if(mouse_left_released()) mlock = LOCK_NONE;
		// Draw
		draw_map();
		draw_tileset();
		draw_border();
		draw_toolbar();
		draw_buttons();
		graphics_present();
	}
	graphics_close();
	return 0;
}
