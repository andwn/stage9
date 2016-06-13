#ifndef INC_DIALOG_H_
#define INC_DIALOG_H_

#include "common.h"

/**
 * File chooser dialogs
 * Returns the filename selected or NULL if it was cancelled
 */
char* dialog_map_open();

char* dialog_tileset_open();

char* dialog_tileattr_open();

char* dialog_map_save();

char* dialog_tileattr_save();

/**
 * Other windows go here eventually, like map properties and tile attribute edit
 */

#endif
