#ifndef INC_DIALOG_H_
#define INC_DIALOG_H_

#include "common.h"

/**
 * File chooser dialogs
 * Returns the filename selected or NULL if it was cancelled
 */
char* dialog_map_open(const char *defaultFilename);

char* dialog_tileset_open(const char *defaultFilename);

char* dialog_tileattr_open(const char *defaultFilename);

char* dialog_map_save(const char *defaultFilename);

char* dialog_tileattr_save(const char *defaultFilename);

/**
 * Other windows go here eventually, like map properties and tile attribute edit
 */

#endif
