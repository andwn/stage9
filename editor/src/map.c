#include "map.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define min(a, b) (a > b ? b : a)

#define S9M_VERSION 1

#define FLAG_UPPERLAYER	0x01
#define FLAG_PLANA		0x02
#define FLAG_BYTETILES	0x04
#define FLAG_WRAPH		0x08
#define FLAG_WRAPV		0x10

Map* map_create(const char *name, u16 width, u16 height,
		bool upperLayer, bool planA, bool byteTiles, bool wrapH, bool wrapV) {
	lprintf(DEBUG, "Creating new map \"%s\" - %hu, %hu", name, width, height);
	Map *map = calloc(sizeof(Map), 1);
	map->version = S9M_VERSION;
	map->name = calloc(1, strlen(name) + 1);
	strcpy(map->name, name);
	map->upperLayer = upperLayer;
	map->planA = planA;
	map->byteTiles = byteTiles;
	map->wrapH = wrapH;
	map->wrapV = wrapV;
	map->width = width;
	map->height = height;
	map->tiles = calloc(2, width * height * (map->upperLayer ? 2 : 1));
	return map;
}

Map* map_create_default() {
	return map_create("Untitled", 40, 28, 1, 0, 1, 0, 0);
}

void map_copy_tiles(Map *from, Map *to) {
	// Tiles will be truncated to whichever map is smaller
	int lmax = min(from->upperLayer, to->upperLayer);
	int xmax = min(from->width, to->width);
	int ymax = min(from->height, to->height);
	for(int l = 0; l <= lmax; l++) {
		for(int y = 0; y < ymax; y++) {
			for(int x = 0; x < xmax; x++) {
				to->tiles[l * to->width * to->height + y * to->width + x] =
					from->tiles[l * from->width * from->height + y * from->width + x];
			}
		}
	}
}

void map_save(const char *filename, Map *map) {
	SDL_RWops *file = SDL_RWFromFile(filename, "wb");
	if(file == NULL) {
		lprintf(ERROR, "Unable to open or create file: %s", filename);
		return;
	}
	// Version
	SDL_WriteBE16(file, map->version);
	// Flags
	SDL_WriteU8(file, (map->upperLayer ? FLAG_UPPERLAYER : 0) +
					(map->planA ? FLAG_PLANA : 0) +
					(map->byteTiles ? FLAG_BYTETILES : 0) +
					(map->wrapH ? FLAG_WRAPH : 0) +
					(map->wrapV ? FLAG_WRAPV : 0));
	// Name
	u8 nameLen = strlen(map->name);
	SDL_WriteU8(file, nameLen);
	if(nameLen > 0) SDL_RWwrite(file, map->name, 1, nameLen);
	// Width, Height
	SDL_WriteBE16(file, map->width);
	SDL_WriteBE16(file, map->height);
	// Size doubled if there is an upper layer
	u32 tsize = map->width * map->height * (map->upperLayer ? 2 : 1);
	// Store as 1 or 2 bytes depending on setting
	for(u32 i = 0; i < tsize; i++) {
		if(map->byteTiles) SDL_WriteU8(file, map->tiles[i]);
		else SDL_WriteBE16(file, map->tiles[i]);
	}
	SDL_RWclose(file);
}

Map* map_open(const char *filename) {
	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if(file == NULL) {
		lprintf(ERROR, "Unable to open file: %s", filename);
		return NULL;
	}
	Map *map = calloc(sizeof(Map), 1);
	// Version
	map->version = SDL_ReadLE16(file);
	if(map->version > S9M_VERSION) {
		lprintf(ERROR, "Map version mismatch (%hu) expected %hu", map->version, S9M_VERSION);
		free(map);
		SDL_RWclose(file);
		return NULL;
	} else if(map->version < S9M_VERSION) {
		// Only one version, so nothing here
	}
	// Flags
	u8 flags = SDL_ReadU8(file);
	map->upperLayer = (flags & FLAG_UPPERLAYER) ? 1 : 0;
	map->planA = (flags & FLAG_PLANA) ? 1 : 0;
	map->byteTiles = (flags & FLAG_BYTETILES) ? 1 : 0;
	map->wrapH = (flags & FLAG_WRAPH) ? 1 : 0;
	map->wrapV = (flags & FLAG_WRAPV) ? 1 : 0;
	if(map->upperLayer && map->planA) {
		lprintf(WARN, "Mutually exclusive options \"Enable Upper Layer\" and \"Use Plan A\"");
	}
	// Name
	u8 nameLen = SDL_ReadU8(file);
	map->name = calloc(1, nameLen + 1);
	if(nameLen > 0) {
		size_t size = SDL_RWread(file, map->name, 1, nameLen);
		if(size < nameLen) {
			lprintf(ERROR, "Couldn't open map, unexpected EOF");
			SDL_RWclose(file);
			free(map->name);
			free(map);
			return NULL;
		}
	}
	map->name[nameLen] = '\0';
	// Width, Height
	map->width = SDL_ReadLE16(file);
	map->height = SDL_ReadLE16(file);
	if(map->width < 20 || map->width > 999 || map->height < 14 || map->height > 999) {
		lprintf(ERROR, "Invalid map dimensions (%hu, %hu) - must be between 20x40 and 999x999",
			map->width, map->height);
			SDL_RWclose(file);
			free(map->name);
			free(map);
			return NULL;
	}
	// Total size in memory, doubled if there is an upper layer
	u32 tsize = map->width * map->height * (map->upperLayer ? 2 : 1);
	map->tiles = calloc(2, tsize);
	// Tiles in file may be 1 or 2 bytes, in memory always 2
	for(u32 i = 0; i < tsize; i++) {
		map->tiles[i] = map->byteTiles ? SDL_ReadU8(file) : SDL_ReadBE16(file);
	}
	SDL_RWclose(file);
	lprintf(INFO, "Loaded map \"%s\" (%hu, %hu) - %hx", 
		map->name, map->width, map->height, flags);
	return map;
}

void map_free(Map *map) {
	free(map->tiles);
	free(map->name);
	free(map);
}
