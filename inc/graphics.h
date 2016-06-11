#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common.h"

#define WIN_W		800
#define WIN_H		600
#define REND_OPS	SDL_RENDERER_PRESENTVSYNC

#define FONT_SIZE_X	5
#define FONT_SIZE_Y	12
#define FONT_COLS	20

#define COLOR_WHITE ((SDL_Color) { 0xFF, 0xFF, 0xFF, 0xFF })
#define COLOR_BLACK ((SDL_Color) { 0x00, 0x00, 0x00, 0x00 })

#define draw_rect(x, y, w, h) ({                                                               \
	SDL_Rect r = { x, y, w, h };                                                               \
	SDL_RenderDrawRect(renderer, &r);                                                          \
})

#define fill_rect(x, y, w, h) ({                                                               \
	SDL_Rect r = { x, y, w, h };                                                               \
	SDL_RenderFillRect(renderer, &r);                                                          \
})

#define draw_texture(tex, x, y) ({                                                             \
	SDL_Rect r = { x, y, 0, 0 };                                                               \
	SDL_QueryTexture(tex, NULL, NULL, &r.w, &r.h);                                             \
	SDL_RenderCopy(renderer, tex, NULL, &r);                                                   \
})

#define draw_tile(tex, sx, sy, tx, ty, w, h) ({                                                \
	SDL_Rect sr = { sx, sy, w, h };                                                            \
	SDL_Rect tr = { tx, ty, w, h };                                                            \
	SDL_RenderCopy(renderer, tex, &sr, &tr);                                                   \
})

SDL_Window *window;
SDL_Renderer *renderer;

void graphics_init();

SDL_Texture* graphics_load_texture(const char *filename);

void graphics_draw_text(const char *text, int x, int y, SDL_Color color);

void graphics_present();

void graphics_close();

#endif
