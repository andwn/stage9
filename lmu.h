#ifndef INC_LMU_H_
#define INC_LMU_H_

#include "common.h"

/*
 * Import/Export for RPG Maker 2000/2003 LMU format
 */

S9File* lmu_import(const char *filename);

void lmu_export(const char *filename, S9File *s9layout);

#endif
