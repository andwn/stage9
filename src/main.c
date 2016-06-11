#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "common.h"
#include "log.h"
#include "s9map.h"
#include "dialog.h"
#include "graphics.h"

#define map_tile(x, y) (map->tiles[(x) + (y) * map->layoutWidth])

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

#define MAP_SCROLL_SPEED 4

// Menu
#define MENU_COUNT			6
#define MENU_MAXITEMCOUNT	6
#define MENU_MAXLABELSIZE	32
enum { 
	MENU_MAP, 
	MENU_TSET,
	MENU_ATTR,
	MENU_VIEW,
	MENU_TOOL,
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
	MENU_TSET_ATTROPEN, 
	MENU_TSET_ATTRSAVE, 
	MENU_TSET_ATTRSAVEAS, 
	MENU_TSET_ATTREDIT 
};
enum {
	MENU_VIEW_SHOWGRID,
	MENU_VIEW_TILEATTR,
	MENU_VIEW_LOWERLAYER,
	MENU_VIEW_UPPERLAYER
};
enum {
	MENU_TOOL_TSETOPT
};
enum {
	MENU_HELP_MANUAL,
	MENU_HELP_GITHUB,
	MANU_HELP_ABOUT
};
const u8 menuItemCount[MENU_COUNT] = { 6, 2, 4, 4, 1, 3 };
const char menu[MENU_COUNT][MENU_MAXITEMCOUNT + 1][MENU_MAXLABELSIZE] = {
	{ "Map", "New", "Open", "Save", "SaveAs", "Properties", "Close" },
	{ "Tileset", "Open", "Close" },
	{ "Tile Attr", "Open", "Save", "SaveAs", "Edit" },
	{ "View", "Show Grid", "Show Attributes", "Show Lower Layer", "Show Upper Layer" },
	{ "Tools", "Optimize Tileset" },
	{ "Help", "Manual", "Github", "About" }
};

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
//S9TileOpFile *tsProp = NULL;
int tsWidth = 0, tsHeight = 0;

// Map
char *mapFilename = NULL;
S9Map *map = NULL;

// Current states of things
int mapCamX = 0, mapCamY = 0;
int mapHoverX = -1, mapHoverY = -1;
int tsScrollY = 0;
int tsSelected = 0, tsHover = -1;
int menuHover = -1, menuOpen = -1, menuSubHover = -1;

void do_menu_action(int menuIndex, int item) {
	switch(menuIndex) {
		case MENU_MAP:
		switch(item) {
			case MENU_MAP_NEW: {
				S9Map *newMap = map_create("Untitled", 40, 28);
				if(newMap != NULL) {
					map_free(map);
					map = newMap;
				}
				break;
			}
			case MENU_MAP_OPEN: {
				char *newFilename = dialog_map_open(mapFilename);
				if(newFilename != NULL) {
					S9Map *newMap = map_open(newFilename);
					if(newMap != NULL) {
						map_free(map);
						map = newMap;
						// Make another copy of the string to keep the GTK stuff separate
						free(mapFilename);
						mapFilename = malloc(strlen(newFilename) + 1);
						strcpy(mapFilename, newFilename);
						g_free(newFilename);
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
				char *newFilename = dialog_map_save(mapFilename);
				if(newFilename != NULL) {
					map_save(newFilename, map);
					// Make another copy of the string to keep the GTK stuff separate
					free(mapFilename);
					mapFilename = malloc(strlen(newFilename) + 1);
					strcpy(mapFilename, newFilename);
					g_free(newFilename);
				}
				break;
			}
			case MENU_MAP_PROPERTIES:
			break;
			case MENU_MAP_CLOSE:
			map_free(map);
			map = NULL;
			break;
		}
		break;
		case MENU_TSET: break;
		case MENU_ATTR: break;
		case MENU_VIEW: break;
		case MENU_TOOL: break;
		case MENU_HELP: break;
	}
}

bool update_input() {
	// Mouse state
	omstate = mstate;
	mstate = SDL_GetMouseState(&mousex, &mousey);
	// Poll input - do things when the user presses stuff
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {
			return true;
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
	return false;
}

// These update functions occur if the mouse is over a specific area
void update_submenu() {
	int menux = TOOLBAR_X + menuOpen * MENU_W,
		menuy = TOOLBAR_Y + TOOLBAR_H,
		menuw = MENUITEM_W,
		menuh = MENUITEM_H * menuItemCount[menuOpen];
	menuHover = menuOpen;
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
	if(mouse_left_down()) { // Draw tile
		map_tile(mapHoverX, mapHoverY) = tsSelected;
	} else if(mouse_right_down()) { // Erase
		map_tile(mapHoverX, mapHoverY) = 0;
	}
}

void update_tileset() {
	// Get which tile in the tileset is underneath the mouse
	u16 mx = (mousex - TILESET_X) / TILE_SIZE, 
		my = (mousey - TILESET_Y + tsScrollY) / TILE_SIZE;
	tsHover = mx + my * (TILESET_W / TILE_SIZE);
	if(mouse_left_down()) { // Select hovering tile
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
	SDL_SetRenderDrawColor(renderer, 0x66, 0x44, 0x88, 0x7F);
	fill_rect(menux, menuy, menuw, menuh);
	for(int i = 0; i < menuItemCount[menuIndex]; i++) {
		if(menuSubHover == i) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x22, 0xAA, 0xFF);
			fill_rect(menux, menuy + i * MENUITEM_H, menuw, MENUITEM_H);
		}
		graphics_draw_text(menu[menuIndex][i + 1], 
			menux + 4, menuy + i * MENUITEM_H + 4, COLOR_WHITE);
	}
}

void draw_toolbar() {
	int x = TOOLBAR_X, y = TOOLBAR_Y;
	for(int i = 0; i < MENU_COUNT; i++) {
		if(menuHover == i || menuOpen == i) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x22, 0xAA, 0xFF);
		} else {
			SDL_SetRenderDrawColor(renderer, 0x66, 0x44, 0x88, 0xFF);
		}
		fill_rect(x, y, MENU_W, TOOLBAR_H);
		graphics_draw_text(menu[i][0], x + 4, y + 4, COLOR_WHITE);
		if(menuOpen == i) draw_submenu(i);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		x += MENU_W;
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
	//map = lmu_import("sample/mado.lmu");
	map = map_create("Untitled", 40, 28);
	// Tileset
	tsTexture = graphics_load_texture("sample/main.png");
	SDL_QueryTexture(tsTexture, NULL, NULL, &tsWidth, &tsHeight);
	tsWidth /= TILE_SIZE;
	tsHeight /= TILE_SIZE;
	
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
