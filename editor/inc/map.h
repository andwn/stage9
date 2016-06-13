#ifndef INC_S9MAP_H_
#define INC_S9MAP_H_

#include "common.h"

typedef struct {
	u16 version; // This is the verion of the format, not the editor
	bool upperLayer, planA, byteTiles;
	char *name;
	u16 width, height;
	u16 *tiles;
} Map;

Map* map_create(const char *name, u16 width, u16 height);

void map_save(const char *filename, Map *s9m);

Map* map_open(const char *filename);

void map_free(Map *map);

#endif
