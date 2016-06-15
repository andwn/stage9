#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

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

void map_save(const char *filename, Map *map) {
	FILE *file = fopen(filename, "wb");
	if(file == NULL) {
		lprintf(ERROR, "Unable to open or create file: %s", filename);
		return;
	}
	// Version
	fwrite(&map->version, 2, 1, file);
	// Flags
	u8 flags = (map->upperLayer ? FLAG_UPPERLAYER : 0) +
				(map->planA ? FLAG_PLANA : 0) +
				(map->byteTiles ? FLAG_BYTETILES : 0) +
				(map->wrapH ? FLAG_WRAPH : 0) +
				(map->wrapV ? FLAG_WRAPV : 0);
	fwrite(&flags, 1, 1, file);
	// Name
	u8 nameLen = strlen(map->name);
	fwrite(&nameLen, 1, 1, file);
	if(nameLen > 0) fwrite(map->name, 1, nameLen, file);
	// Width, Height
	fwrite(&map->width, 2, 1, file);
	fwrite(&map->height, 2, 1, file);
	// Size doubled if there is an upper layer
	u32 tsize = map->width * map->height * (map->upperLayer ? 2 : 1);
	// Store as 1 or 2 bytes depending on setting
	for(u32 i = 0; i < tsize; i++) {
		fwrite(&map->tiles[i], 2 - map->byteTiles, 1, file);
	}
	fclose(file);
}

Map* map_open(const char *filename) {
	lprintf(TRACE, "map_open: begin");
	FILE *file = fopen(filename, "rb");
	if(file == NULL) {
		lprintf(ERROR, "Unable to open file: %s", filename);
		return NULL;
	}
	Map *map = calloc(sizeof(Map), 1);
	// Version
	lprintf(TRACE, "map_open: version");
	fread(&map->version, 2, 1, file);
	if(map->version > S9M_VERSION) {
		lprintf(ERROR, "Map was created in a newer version. Update!");
		free(map);
		fclose(file);
		return NULL;
	}
	// Flags
	lprintf(TRACE, "map_open: flags");
	u8 flags;
	fread(&flags, 1, 1, file);
	map->upperLayer = (flags & FLAG_UPPERLAYER) ? 1 : 0;
	map->planA = (flags & FLAG_PLANA) ? 1 : 0;
	map->byteTiles = (flags & FLAG_BYTETILES) ? 1 : 0;
	map->wrapH = (flags & FLAG_WRAPH) ? 1 : 0;
	map->wrapV = (flags & FLAG_WRAPV) ? 1 : 0;
	if(map->upperLayer && map->planA) {
		lprintf(WARN, "Mutually exclusive options \"Enable Upper Layer\" and \"Use Plan A\"");
	}
	// Name
	lprintf(TRACE, "map_open: name");
	u8 nameLen;
	fread(&nameLen, 1, 1, file);
	map->name = calloc(1, nameLen + 1);
	if(nameLen > 0) fread(map->name, 1, nameLen, file);
	map->name[nameLen] = '\0';
	// Width, Height
	lprintf(TRACE, "map_open: size");
	fread(&map->width, 2, 1, file);
	fread(&map->height, 2, 1, file);
	// Total size in memory, doubled if there is an upper layer
	lprintf(TRACE, "map_open: tiles");
	u32 tsize = map->width * map->height * (map->upperLayer ? 2 : 1);
	map->tiles = calloc(2, tsize);
	// Tiles in file may be 1 or 2 bytes, in memory always 2
	for(u32 i = 0; i < tsize; i++) {
		fread(&map->tiles[i], 2 - map->byteTiles, 1, file);
	}
	fclose(file);
	lprintf(TRACE, "map_open: finish");
	return map;
}

void map_free(Map *map) {
	free(map->tiles);
	free(map->name);
	free(map);
}
