#ifndef INC_S9MAP_H_
#define INC_S9MAP_H_

#include "common.h"

#define S9M_UPPERLAYER	1
#define S9M_USEPLANA	2
#define S9M_BYTETILES	4

typedef struct S9Map S9Map;

// Information stored in an S9M file
struct S9Map {
	u16 version; // This is the verion of the format, not the editor
	u16 flags;
	u8 nameLen;
	char *name;
	u16 layoutWidth, layoutHeight;
	u16 *tiles;
};

#endif
