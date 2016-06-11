#ifndef INC_COMMON_H_
#define INC_COMMON_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned long u64;

#define TILE_SIZE	16

typedef unsigned char bool;
enum { false, true };

#define init(cond, err) ({ if(cond) { lprintf(FATAL, err()); exit(1); } })

#endif
