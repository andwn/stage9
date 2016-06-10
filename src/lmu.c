#include "lmu.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"

// See: https://github.com/EasyRPG/liblcf/blob/master/src/generated/lmu_map.cpp

S9Map* lmu_import(const char *filename) {
	FILE *lmu;
	S9Map *s9m;
	u16 tile;
	char buf, str[12];
	int offset;
	lprintf(DEBUG, "Begin RPG Maker 2000 LMU import for: %s", filename);
	lmu = fopen(filename, "rb");
	if(lmu == NULL) {
		lprintf(ERROR, "Failed to open file");
		return NULL;
	}
	fseek(lmu, 1, SEEK_CUR);
	fread(str, 1, 10, lmu);
	str[10] = '\0';
	if(strcmp(str, "LcfMapUnit") != 0) {
		lprintf(ERROR, "Invalid header \"%s\", expected \"LcfMapUnit\"", str);
	}
	s9m = calloc(sizeof(S9Map), 1);
	s9m->version = S9M_VERSION;
	s9m->flags |= S9M_UPPERLAYER;
	// Map size - 4 byte ints
    fseek(lmu, 6, SEEK_CUR);
    fread(&s9m->layoutWidth, 2, 1, lmu);
    fseek(lmu, 2, SEEK_CUR);
    fread(&s9m->layoutHeight, 2, 1, lmu);
    lprintf(DEBUG, "Layout Width: %hu, Height: %hu", s9m->layoutWidth, s9m->layoutHeight);
    if(s9m->layoutWidth > 999 || s9m->layoutHeight > 999) {
		lprintf(ERROR, "That's a pretty big map... something is wrong");
		return NULL;
	}
    // Allocate the tiles array now that we know the size
    s9m->tiles = calloc(2, s9m->layoutWidth * s9m->layoutHeight * 2);
    // Parallax flag
    //fread(&buf, 1, 1, lmu);
    //if(buf) s9m->flags |= S9M_USEPLANA;
    // There is a string or something I assume here which always ends with '>'
    // Exactly 40 bytes after is where the tilemap starts
    for(buf = '!'; buf != '>'; fread(&buf, 1, 1, lmu));
    offset = ftell(lmu);
    fseek(lmu, offset + 40, SEEK_SET);
    lprintf(DEBUG, "Tile data at offset: %d", offset + 40);
	// The tiles
    for(u16 x = 0; x < s9m->layoutWidth; x++) {
        for(u16 y = 0; y < s9m->layoutHeight; y++) {
            fread(&tile, 2, 1, lmu);
            tile %= 1000;
			//if(tile >= 256) {
			//	lprintf(TRACE, "LMU tile index can't fit in a byte: %hu\n", tile);
			//}
			s9m->tiles[y * s9m->layoutWidth + x] = tile;
			fseek(lmu, 2, SEEK_CUR);
		}
	}
	lprintf(DEBUG, "Upper layer at offset: %d", ftell(lmu));
	// This is the upper layer
	int l = s9m->layoutWidth * s9m->layoutHeight; // Start of layer index
	for(u16 x = 0; x < s9m->layoutWidth; x++) {
        for(u16 y = 0; y < s9m->layoutHeight; y++) {
			fread(&tile, 2, 1, lmu);
			tile %= 1000;
			s9m->tiles[l + y * s9m->layoutWidth + x] = tile;
		}
    }
	fclose(lmu);
	lprintf(INFO, "Successfully loaded LMU");
	return s9m;
}
