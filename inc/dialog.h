#ifndef INC_DIALOG_H_
#define INC_DIALOG_H_

#include "common.h"

/**
 * Launches a GTK file chooser to select a map file to open
 * Returns the filename selected or NULL if it was cancelled
 */
char* dialog_open_map();

char* dialog_open_tileset();

char* dialog_open_tileprop();

/**
 * Launches a GTK file chooser to select where to save the map
 * Parameter is the starting location (Save As), or null for OS default
 * Returns the filename selected or NULL if it was cancelled
 */
char* dialog_save_map(const char *defaultFilename);

char* dialog_save_tileprop();

#endif
