#include "s9map.h"

#include <stdio.h>
#include <stdlib.h>

void s9map_save(const char *filename, S9Map *s9m) {
	FILE *file = fopen(filename, "wb");
	// Version
	fwrite(&s9m->version, 2, 1, file);
	// Flags
	fwrite(&s9m->flags, 2, 1, file);
	// Name
	fwrite(&s9m->nameLen, 1, 1, file);
	s9m->name = calloc(1, s9m->nameLen);
	fwrite(&s9m->name, 1, s9m->nameLen, file);
	// Width, Height
	fwrite(&s9m->layoutWidth, 2, 1, file);
	fwrite(&s9m->layoutHeight, 2, 1, file);
	// Size doubled if there is an upper layer
	int tsize = s9m->layoutWidth * s9m->layoutHeight * ((s9m->flags & S9M_UPPERLAYER) ? 2 : 1);
	// Store as 1 or 2 bytes depending on setting
	for(int i = 0; i < tsize; i++) {
		fwrite(&s9m->tiles[i], 2 - ((s9m->flags & S9M_BYTETILES) > 0), 1, file);
	}
	fclose(file);
}

S9Map* s9map_open(const char *filename) {
	FILE *file = fopen(filename, "rb");
	S9Map *s9m = calloc(1, sizeof(S9Map));
	// Version
	fread(&s9m->version, 2, 1, file);
	// Flags
	fread(&s9m->flags, 2, 1, file);
	// Name
	fread(&s9m->nameLen, 1, 1, file);
	s9m->name = calloc(1, s9m->nameLen);
	fread(&s9m->name, 1, s9m->nameLen, file);
	// Width, Height
	fread(&s9m->layoutWidth, 2, 2, file);
	// Total size in memory, doubled if there is an upper layer
	int tsize = s9m->layoutWidth * s9m->layoutHeight * ((s9m->flags & S9M_UPPERLAYER) ? 2 : 1);
	s9m->tiles = calloc(2, tsize);
	// Tiles in file may be 1 or 2 bytes, in memory always 2
	for(int i = 0; i < tsize; i++) {
		fread(&s9m->tiles[i], 2 - ((s9m->flags & S9M_BYTETILES) > 0), 1, file);
	}
	fclose(file);
	return s9m;
}
