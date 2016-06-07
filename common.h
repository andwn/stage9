#ifndef INC_COMMON_H_
#define INC_COMMON_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef struct {
	u16 version, layoutWidth, layoutHeight, entityCount;
	u8 *tiles;
	S9Entity *entities;
} S9File;

typedef struct {
	u16 x, y, id, event, type, flags;
} S9Entity;

typedef struct {
	u8 isWind : 1, 
		isForeground : 1, 
		isWater : 1, 
		isSlope : 1,
		value : 4;
} S9TileOp;

#endif
