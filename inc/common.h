#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#define S9M_VERSION 1

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef struct S9MapFile S9MapFile;
typedef struct S9Entity S9Entity;
typedef struct S9EntityMini S9EntityMini;

// Information stored in an S9M file
struct S9MapFile {
	u16 version; // This is the verion of the format, not the editor
	// These are flags which specify
	u16 hasUpperLayer : 1, // If true a second "upper" layer of tiles is included
		tilesAreTwoBytes : 1, // If tile indexes need to go over 255
		minifiedEntities : 1, // Entities are compacted to use 4 less bytes each
		reserved : 13;
	u8 *name; // Null terminated string
	u16 layoutWidth, layoutHeight, entityCount;
	u8 *tiles;
	S9Entity *entities;
};

struct S9Entity {
	u16 x, y, id, event, type, flags;
};

struct S9EntityMini {
	u32 x : 10, y : 10, id : 12, 
		event : 10, type : 10, flags : 12;
};

typedef struct S9TileOpFile S9TileOpFile;
typedef struct S9TileOp S9TileOp;

// Information stored in an S9A file
struct S9TileOpFile {
	u16 version;
	u16 reserved;
	u8 tilesetWidth, tilesetHeight;
	S9TileOp *ops;
};

struct S9TileOp {
	u8 isWind : 1, 
		isForeground : 1, 
		isWater : 1, 
		isSlope : 1,
		value : 4;
};

typedef unsigned char bool;
enum { false, true };

#endif
