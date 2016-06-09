#ifndef INC_LMU_H_
#define INC_LMU_H_

#include "common.h"
#include "s9map.h"

/*
 * Import/Export for RPG Maker 2000/2003 LMU format
 */

S9Map* lmu_import(const char *filename);

void lmu_export(const char *filename, S9Map *s9m);

#endif