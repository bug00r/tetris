#ifndef TETRIS_GFX_H
#define TETRIS_GFX_H

#include <stdlib.h>
#include <stdbool.h>

#include "tetris_defs.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

typedef struct {
	SDL_Color line_remove_color;
} tetris_gfx_state_t;

typedef struct {
	tetris_gfx_state_t state;
	TTF_Font *headline_font;//SDL_GFX
	TTF_Font *score_font;	//SDL_GFX
	TTF_Font *level_font;	//SDL_GFX
	SDL_Surface *texture;
	SDL_Surface *background;
	SDL_Color color_headline; //SDL_GFX
	SDL_Color color_label;	  //SDL_GFX
	SDL_Color color_value;    //SDL_GFX
} tetris_gfx_t;

bool tetris_gfx_init(tetris_gfx_t *gfx);

SDL_Surface * tetris_gfx_create_text(const char *text, TTF_Font *font, SDL_Color *color);
SDL_Surface * tetris_gfx_create_uint_text(unsigned int number, TTF_Font *font, SDL_Color *color);
void tetris_gfx_draw_text_at_position(SDL_Surface *text, SDL_Surface *text_target, int x, int y);
void tetris_gfx_draw_rect_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void tetris_gfx_draw_rect_outline_rgba(SDL_Renderer *renderer, int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif