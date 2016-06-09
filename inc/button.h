#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "common.h"
#include <SDL2/SDL.h>

typedef struct Button Button;

struct Button {
	int type, state;
	int x, y, w, h;
	SDL_Color cNormal, cHover, cClick, cDisabled;
	char *text;
	int dropButtonCount;
	Button *dropButtons;
};

#endif
