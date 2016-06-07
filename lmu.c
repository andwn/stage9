#include "lmu.h"

#include <stdio.h>
#include "log.h"

S9File* lmu_import(const char *filename) {
	FILE *lmu;
	S9File *s9layout;
	u16 tile;
	int offset;
	
	lmu = fopen(filename, "rb");
	s9layout = calloc(sizeof(S9File), 0);
	// Map size - height then width, 4 byte ints
    fseek(lmu, 12, SEEK_SET);
    fread(s9layout.layoutHeight, 1, 2, lmu);
    fseek(lmu, 2, SEEK_CUR);
    fread(s9layout.layoutWidth, 1, 2, lmu);
    // Allocate the tiles array now that we know the size
    s9layout->tiles = calloc(s9layout.layoutWidth * s9layout.layoutHeight, 0);
    // There is a string or something I assume here which always ends with '>'
    // Exactly 40 bytes after is where the tilemap starts
    for(char buf = '!'; buf != '>'; fread(&buf, 1, 1, lmu));
    offset = ftell(lmu);
    fseek(lmu, offset + 40, SEEK_SET);
	// The tiles
    for(u16 x = 0; x < s9layout->layoutWidth; x++) {
        for(u16 y = 0; y < s9layout->layoutHeight; y++) {
            fread(&tile, 1, 2, lmu);
            tile %= 1000;
			if(tile >= 256) {
				lprintf(WARN, "LMU tile index can't fit in a byte: %hu\n", tile);
			}
			s9layout->tiles[y * s9layout->layoutWidth + x] = tile;
			fseek(lmu, 2, SEEK_CUR);
		}
	}
	// This is another layer, probably
	for(u16 x = 0; x < s9layout->layoutWidth; x++) {
        for(u16 y = 0; y < s9layout->layoutHeight; y++) {
			fread(&tile, 1, 2, lmu);
			// Hi
		}
    }
	fclose(lmu);
}
