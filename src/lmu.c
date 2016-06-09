#include "lmu.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

// See: https://github.com/EasyRPG/liblcf/blob/master/src/generated/lmu_map.cpp

S9Map* lmu_import(const char *filename) {
	FILE *lmu;
	S9Map *s9m;
	u16 tile;
	char buf;
	int offset;
	
	lmu = fopen(filename, "rb");
	s9m = calloc(sizeof(S9Map), 1);
	s9m->version = S9M_VERSION;
	s9m->flags |= S9M_UPPERLAYER;
	// Map size - 4 byte ints
    fseek(lmu, 10 + 2, SEEK_SET);
    fread(&s9m->layoutWidth, 1, 2, lmu);
    fseek(lmu, 2, SEEK_CUR);
    fread(&s9m->layoutHeight, 1, 2, lmu);
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
	return s9m;
}
