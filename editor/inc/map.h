#ifndef INC_MAP_H_
#define INC_MAP_H_

#include "common.h"

typedef struct {
	u16 version; // This is the verion of the format, not the editor
	bool upperLayer, planA, byteTiles, wrapH, wrapV;
	char *name;
	u16 width, height;
	u16 *tiles;
} Map;

Map* map_create(const char *name, u16 width, u16 height,
		bool upperLayer, bool planA, bool byteTiles, bool wrapH, bool wrapV);

Map* map_create_default();

void map_save(const char *filename, Map *s9m);

Map* map_open(const char *filename);

void map_free(Map *map);

#endif
